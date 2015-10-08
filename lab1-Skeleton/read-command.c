// UCLA CS 111 Lab 1 command reading

#include "command.h"
#include "command-internals.h"

#include <error.h>

/* FIXME: You may need to add #include directives, macro definitions,
   static function definitions, etc.  */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "linked-list.h"

/* FIXME: Define the type 'struct command_stream' here.  This should
   complete the incomplete type declaration in command.h.  */

enum chunk_status {
  NEXT_IS_INPUT,
  NEXT_IS_OUTPUT,
  NEXT_IS_WORD
};


void
parse_chunk_to_command(char* chunk, command_t simple_command) {
  int chunk_len = strlen(chunk);
  int i;
  int num_words = 0;
  int status = NEXT_IS_WORD;
  int word_len;

  // TODO: check for parenthesis maybe

  simple_command->type = SIMPLE_COMMAND;
  simple_command->u.word = malloc(20*sizeof(char*));
  simple_command->input = (char *) malloc(256);
  simple_command->input = NULL;
  simple_command->output = (char *) malloc(256);
  simple_command->output = NULL;
  char word_to_store[256] = ""; 

  for (i = 0; i < chunk_len; i++) {

    switch(chunk[i]) {
      case '<':
        // signals end of word
        if (word_to_store[0] == '\0') {
          ;
        }
        else if (status == NEXT_IS_OUTPUT) {
          simple_command->output = (char*) malloc(256);
          strcpy(simple_command->output, word_to_store);
        }
        else {
          simple_command->u.word[num_words] = (char*) malloc(256);
          strcpy(simple_command->u.word[num_words++], word_to_store);
        }
        word_to_store[0] = '\0';
        status = NEXT_IS_INPUT;
        break;
      case '>':
        if (word_to_store[0] == '\0') {
          ;
        }
        else if (status == NEXT_IS_INPUT) {
          simple_command->input = (char*) malloc(256);          
          strcpy(simple_command->input, word_to_store);
        }
        else {
          simple_command->u.word[num_words] = (char*) malloc(256);
          strcpy(simple_command->u.word[num_words++], word_to_store);
        }
        word_to_store[0] = '\0';
        status = NEXT_IS_OUTPUT;
        break;
      case ' ':
        if (word_to_store[0] == '\0') {
          ;
        }
        else if (status == NEXT_IS_OUTPUT) {
          simple_command->output = (char*) malloc(256);
          strcpy(simple_command->output, word_to_store);
          word_to_store[0] = '\0';
          status = NEXT_IS_WORD;
        }
        else if (status == NEXT_IS_INPUT) {
          simple_command->input = (char*) malloc(256);
          strcpy(simple_command->input, word_to_store);
          word_to_store[0] = '\0';
          status = NEXT_IS_WORD;
        }
        else {
          // we have a word
          simple_command->u.word[num_words] = (char*) malloc(256);
          strcpy(simple_command->u.word[num_words],word_to_store);
          num_words++;
          word_to_store[0] = '\0';
        }
        break;
      case '\n':
        break;
      default:
        word_len = strlen(word_to_store);
        word_to_store[word_len] = chunk[i];
        word_to_store[++word_len] = '\0';
    }
  }

  if (strcmp(word_to_store, "`") == 0) {
    error(1, 0, "command not valid");
  }

  switch(status) {
    case NEXT_IS_WORD:
      if (word_to_store[0] != '\0') {
        simple_command->u.word[num_words] = (char*) malloc(256);
        strcpy(simple_command->u.word[num_words],word_to_store);
        num_words++;
      }
      break;
    case NEXT_IS_INPUT:
      if (word_to_store[0] == '\0') {
        error(1,0, "< seen with no input");
      }
      else {
        simple_command->input = (char*) malloc(256);
        strcpy(simple_command->input, word_to_store);
      }
      break;
    case NEXT_IS_OUTPUT:
      if (word_to_store[0] == '\0') {
        error(1,0, "> seen with no output");
      }
      else {
        simple_command->output = (char*) malloc(256);
        strcpy(simple_command->output, word_to_store);
      }
      break;
  }

  simple_command->u.word[num_words] = NULL;
}

void
parse_pair_to_operator_command(char* pair, command_t operator_command) {
  if (strcmp(pair, "||") == 0) {
    operator_command->type = OR_COMMAND;
  } 
  else if (strcmp(pair, "&&") == 0) {
    operator_command->type = AND_COMMAND;
  }
  else if (pair[0] == '|') {
    operator_command->type = PIPE_COMMAND;
  }
  else if (pair[0] == ';') {
    operator_command->type = SEQUENCE_COMMAND;
  }
  else {
    printf("You shouldn't be here, something went wrong\n");
  }
}


void push_operator_and_command(char* chunk,
                               char* pair,
                               command_t command_to_append, 
                               command_t operator_to_append,
                               command_stream* iterate_me) {

    command_to_append = malloc(sizeof(struct command));
    parse_chunk_to_command(chunk, command_to_append);
    append_to_list(command_to_append, iterate_me);
    chunk[0] = '\0';
    operator_to_append = malloc(sizeof(struct command));
    parse_pair_to_operator_command(pair, operator_to_append);
    append_to_list(operator_to_append, iterate_me);
}

void print_tree_list(command_stream* printme) {
  command_node* curr = printme->head;
  int count = 0;
  char* string;
  while (curr != NULL) {
    switch(curr->command->type) {
      case SIMPLE_COMMAND:
        string = curr->command->u.word[0];
        break;
      case OR_COMMAND:
        string = "||";
        break;
      case AND_COMMAND:
        string = "&&";
        break;
      case PIPE_COMMAND:
        string = "|";
        break;
      case SEQUENCE_COMMAND:
        string = ";";
        break;

    }
    printf("node %d: type: %s\n", count++, string);
    curr = curr->next;
  }
}

command_t build_command_tree(command_stream* iterate_me) {
  command_stream operator_stack;
  operator_stack.head = NULL;
  operator_stack.tail = NULL;
  command_stream command_stack;
  command_stack.head = NULL;
  command_stack.tail = NULL;

  command_t popped_operator, right_child_command, left_child_command;
  /*
  1) If it's a simple command, push it onto the command stack
  2) If '(' push on to operator stack
  3) If operator and operator stack is empty, push operator on operator stack
  4) if operator and operator stack not empty, pop all operators with greater or equal precedence
    a) for each popped operator, pop two commands off the command stack
    b) combine them into a new command
    c) push it on the command stack
  5) If encounter ')', pop operators off the stack for each operator, pop 2 commands, do the thing
  6) Advance to next word (simple and or operator and go to #2)
  7) When all words are gone, pop each operator and combine with 2 commands, similar to 4a
  */

  int top_command_type, current_type;
  int operator_precedence[] = {1, 0, 1, 2};

  // printf("initialzing curr...\n");
  command_node* curr = iterate_me->head;
  while (curr != NULL) {
    popped_operator = malloc(3*sizeof(struct command));
    right_child_command = malloc(sizeof(struct command));
    left_child_command = malloc(sizeof(struct command));

    current_type = curr->command->type;
    switch(current_type) {
      case SIMPLE_COMMAND:
        // printf("node is simple command\n");
        // print_command(curr->command);
        append_to_list(curr->command, &command_stack);
        break;
      case AND_COMMAND:
      case OR_COMMAND:
      case PIPE_COMMAND:
      case SEQUENCE_COMMAND:
        // printf("node is operator\n");
        //if the operator stack is empty
        if (operator_stack.head == NULL) {
          append_to_list(curr->command, &operator_stack);
        }
        // not empty
        else {
          top_command_type = operator_stack.tail->command->type;
          while (operator_precedence[current_type] <= operator_precedence[top_command_type] && 
                 operator_stack.head != NULL) {
            remove_last_node(&operator_stack, &popped_operator);
            remove_last_node(&command_stack, &right_child_command);
            remove_last_node(&command_stack, &left_child_command);

            if (left_child_command->type == SIMPLE_COMMAND){
              if (left_child_command->u.word[0] == '\0'){
                error(1,0, "incomplete command");
              }
            }

            if (right_child_command->type == SIMPLE_COMMAND){
              if (right_child_command->u.word[0] == '\0'){
                error(1,0, "incomplete command");
              }
            }

            // printf("left child command\n");
            // print_command(left_child_command);
            popped_operator->u.command[0] = left_child_command;
            popped_operator->u.command[1] = right_child_command;
            // print_command(popped_operator);
            append_to_list(popped_operator, &command_stack);
            if (operator_stack.head != NULL) {
              top_command_type = operator_stack.tail->command->type;
            }
          }
          append_to_list(curr->command, &operator_stack);
        }
        break; //end of operator case
    }


    curr = curr->next;
  }//end of reading 

  while (operator_stack.head!=NULL){
    remove_last_node(&operator_stack, &popped_operator);
    remove_last_node(&command_stack, &right_child_command);
    remove_last_node(&command_stack, &left_child_command);
    // print_command(left_child_command);
    // print_command(right_child_command);

    if (left_child_command->type == SIMPLE_COMMAND){
      if (left_child_command->u.word[0] == '\0'){
        error(1,0, "incomplete command");
      }
    }
    if (right_child_command->type == SIMPLE_COMMAND){
      if (right_child_command->u.word[0] == '\0'){
        error(1,0, "incomplete command");
      }
    }

    popped_operator->u.command[0] = left_child_command;
    popped_operator->u.command[1] = right_child_command;
    append_to_list(popped_operator, &command_stack);
  }
  return command_stack.tail->command;
}




command_stream_t
make_command_stream (int (*get_next_byte) (void *),
         void *get_next_byte_argument)
{
  /* FIXME: Replace this with your implementation.  You may need to
     add auxiliary functions and otherwise modify the source code.
     You can also use external functions defined in the GNU C Library.
  */

  // printf("make_command_stream\n");

  /*
  char* test_string = "sort   a < b  > c";
  // should output "sort a<b>c"
  command_t test_command = malloc(sizeof(struct command));
  printf("pre parse\n");
  parse_chunk_to_command(test_string, test_command);
  printf("pre print\n");
  print_command(test_command);
  printf("post print command\n");
  */

  /*
  command_stream test_list;
  command_t popped_command = malloc(sizeof(struct command));
  test_list.head = NULL;
  test_list.tail = NULL;

  command_t new_command0 = malloc(sizeof(struct command));
  new_command0->type = SIMPLE_COMMAND;
  new_command0->status = -1;
  new_command0->u.word = malloc(3 * sizeof(char*));
  new_command0->u.word[0] = "echo";
  new_command0->u.word[1] = "a";
  new_command0->u.word[2] = NULL;
  new_command0->output = "out";
  printf("append 0\n");
  append_to_list(new_command0, &test_list);

  command_t new_command1 = malloc(sizeof(struct command));
  new_command1->type = SIMPLE_COMMAND;
  new_command1->status = -1;
  new_command1->u.word = malloc(3 * sizeof(char*));
  new_command1->u.word[0] = "cat";
  new_command1->u.word[1] = "b";
  new_command1->u.word[2] = NULL;
  printf("append 1 newcommand\n");
  print_command(new_command1);
  append_to_list(new_command1, &test_list);

  command_t new_command2 = malloc(sizeof(struct command));
  new_command2->type = OR_COMMAND;
  new_command2->status = -1;
  new_command2->u.command[0] = new_command0;
  new_command2->u.command[1] = new_command1;
  printf("append 2\n");
  append_to_list(new_command2, &test_list);

  printf("testing list after appends\n");
  print_tree_list(&test_list);

  printf("removing node 1...\n");
  remove_last_node(&test_list, &popped_command);
  printf("popped command_type: %d\n", popped_command->type);
  printf("print removed node\n");
  print_command(popped_command);
  
  printf("removing node 2...\n");
  remove_last_node(&test_list, &popped_command);
  printf("popped command_type: %d\n", popped_command->type);
  printf("print removed node\n");
  print_command(popped_command);
  printf("testing list after 2 removes\n");
  print_tree_list(&test_list);
  printf("removing node 3...\n");
  remove_last_node(&test_list, &popped_command);
  
  if(test_list.head==NULL)
  printf("yayy\n");  
  else
  printf("nooo\n");


  command_stream command_stream_test;
  command_stream_test.head = NULL;
  command_stream_test.tail = NULL;

  command_t new_command3 = malloc(sizeof(struct command));
  new_command3->type = SUBSHELL_COMMAND;
  new_command3->status = -1;
  new_command3->u.subshell_command = new_command0;


  */
  char c = get_next_byte(get_next_byte_argument);
  //printf("%c",c); 

  // read into big buffer
  // iterate through buffer, capturing 2 characters (pair) at a time
  // if pair is an operator, then do stuff
  // else add pair[0] to word

  char buffer[1024] = "";
  int len = 0;
  while (c != EOF)
  {
    buffer[len]=c;
    len++;
    c = get_next_byte(get_next_byte_argument);
  }
  buffer[len++] = EOF;
  // printf("==== buffer ====\n%s\n==== end buffer ====\n", buffer);

  char pair[3]; 
  char chunk[256] = "";
  int chunk_len;
  int i = 0;

  // is command complete
  // i.e. did we just see an AND, OR, or PIPE operator


  int INCOMPLETE_COMMAND = 0;

  command_stream* command_list = malloc(sizeof(struct command_stream));
  command_stream* iterate_me = malloc (sizeof(struct command_stream));
  iterate_me->head = NULL;
  iterate_me->tail = NULL;
  command_t command_to_append;
  command_t operator_to_append;
  for (i = 0; i < len; i++) {
    pair[0] = buffer[i];
    pair[1] = buffer[i+1];
    pair[2] = '\0';

    if (strcmp(pair, "&&") == 0 || strcmp(pair, "||") == 0)  {
      //push to the stack 
      command_to_append = malloc(sizeof(struct command));
      parse_chunk_to_command(chunk, command_to_append);
      append_to_list(command_to_append, iterate_me);
      // printf("and or or found after\n");
      // print_command(command_to_append);
      chunk[0] = '\0';
      operator_to_append = malloc(sizeof(struct command));
      parse_pair_to_operator_command(pair, operator_to_append);
      append_to_list(operator_to_append, iterate_me);
      INCOMPLETE_COMMAND = 1;
      i++;
    } 
    else if (pair[0] == '|') {
      command_to_append = malloc(sizeof(struct command));
      parse_chunk_to_command(chunk, command_to_append);
      append_to_list(command_to_append, iterate_me);
      // printf("pipe found after\n");
      // print_command(command_to_append);
      chunk[0] = '\0';
      operator_to_append = malloc(sizeof(struct command));
      parse_pair_to_operator_command(pair, operator_to_append);
      append_to_list(operator_to_append, iterate_me);
      INCOMPLETE_COMMAND = 1;
    }
    else if (pair[0] == ';') {
      command_to_append = malloc(sizeof(struct command));
      parse_chunk_to_command(chunk, command_to_append);
      append_to_list(command_to_append, iterate_me);
      // printf("semicolon found after\n");
      // print_command(command_to_append);
      chunk[0] = '\0';
      operator_to_append = malloc(sizeof(struct command));
      parse_pair_to_operator_command(pair, operator_to_append);
      append_to_list(operator_to_append, iterate_me);
    }
    else if (strcmp(pair,"\n\n") == 0 || pair[0] == EOF)
    {
      if(INCOMPLETE_COMMAND && pair[0]!= EOF) {
        i++;
      }
      else {
        command_to_append = malloc(sizeof(struct command));
        parse_chunk_to_command(chunk, command_to_append);
        append_to_list(command_to_append, iterate_me);
        // print_command(command_to_append);
        // printf("print our linked list of nodes\n");
        // print_tree_list(iterate_me);



        // printf("trying to build tree\n");

        command_t insert_me = build_command_tree(iterate_me);
        // printf("built tree\n");
        // print_command(insert_me);

        append_to_list(insert_me, command_list);
        iterate_me->head = NULL;
        iterate_me->tail = NULL;
        chunk[0] = '\0';
        i++;
        INCOMPLETE_COMMAND = 0;
        //pop things off and implement the algorithm
      }
    }

    else if (pair[0] == '&') {
      error(1,0,"ampersands come in twos");
    }
    ////////////////////////////
    else if(pair[0]=='\n' )
    {
      int counter=1;
      while(buffer[i+counter]==' ')
      {
        counter++;
      }
      if (buffer[counter+i]==';'||buffer[i+counter]=='|')
        error(1,0,"you don't start a line with operator\n");
    }
    /////////////

    else if (pair[0] == '#') {
      while (buffer[i] != '\n') {
        i++;
      }
    }
    else {
      chunk_len = strlen(chunk);
      chunk[chunk_len] = pair[0];
      chunk[++chunk_len] = '\0';
      INCOMPLETE_COMMAND = 0;
    }
  }
  command_list->current = command_list->head;
  return command_list;


}


command_t read_command_stream (command_stream_t s)
{
  if (s->current == NULL) {
    return 0;
  }
  else {
    command_t return_me = s->current->command;
    s->current = s->current->next;
    return return_me;
  }
  /* FIXME: Replace this with your implementation too.  */
  // error (1, 0, "command reading not yet implemented");
  // return 0;
}