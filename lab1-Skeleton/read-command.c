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
      default:
        word_len = strlen(word_to_store);
        word_to_store[word_len] = chunk[i];
        word_to_store[++word_len] = '\0';
    }
  }

  switch(status) {
    case NEXT_IS_WORD:
      simple_command->u.word[num_words] = (char*) malloc(256);
      strcpy(simple_command->u.word[num_words],word_to_store);
      num_words++;
      break;
    case NEXT_IS_INPUT:
      simple_command->input = (char*) malloc(256);
      strcpy(simple_command->input, word_to_store);
      break;
    case NEXT_IS_OUTPUT:
      simple_command->output = (char*) malloc(256);
      strcpy(simple_command->output, word_to_store);
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
                               command_list* iterate_me) {

    command_to_append = malloc(sizeof(struct command));
    parse_chunk_to_command(chunk, command_to_append);
    append_to_list(command_to_append, iterate_me);
    chunk[0] = '\0';
    operator_to_append = malloc(sizeof(struct command));
    parse_pair_to_operator_command(pair, operator_to_append);
    append_to_list(operator_to_append, iterate_me);
}

void print_tree_list(command_list* printme) {
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

command_t build_command_tree(command_list* iterate_me) {
  command_list operator_stack;
  command_list command_stack;
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


  command_node* curr = iterate_me->head;
  while (curr != NULL) {
    current_type = curr->command->type;
    switch(current_type) {
      case SIMPLE_COMMAND:
        append_to_list(curr->command, &command_stack);
        break;
      case AND_COMMAND:
      case OR_COMMAND:
      case PIPE_COMMAND:
        if (operator_stack.head == NULL) {
          append_to_list(curr->command, &operator_stack);
        }
        // not empty
        else {
          top_command_type = operator_stack.tail->command->type;
          while (operator_precedence[current_type] <= operator_precedence[top_command_type] && operator_stack.head != NULL) {

          }
        }
        break;
    }


    curr = curr->next;
  }
}




command_stream_t
make_command_stream (int (*get_next_byte) (void *),
		     void *get_next_byte_argument)
{
  /* FIXME: Replace this with your implementation.  You may need to
     add auxiliary functions and otherwise modify the source code.
     You can also use external functions defined in the GNU C Library.
  */

	printf("make_command_stream\n");

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
  printf("==== buffer ====\n%s\n==== end buffer ====\n", buffer);

  char pair[3]; 
  char chunk[256] = "";
  int chunk_len;
  int i = 0;

  // is command complete
  // i.e. did we just see an AND, OR, or PIPE operator
  int state;

  command_list* command_stream = malloc(sizeof(struct command_list));
  command_list* iterate_me = malloc (sizeof(struct command_list));
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
      printf("and or or found after\n");
      print_command(command_to_append);
      chunk[0] = '\0';
      operator_to_append = malloc(sizeof(struct command));
      parse_pair_to_operator_command(pair, operator_to_append);
      append_to_list(operator_to_append, iterate_me);
      i++;
    } 
    else if (pair[0] == '|') {
      command_to_append = malloc(sizeof(struct command));
      parse_chunk_to_command(chunk, command_to_append);
      append_to_list(command_to_append, iterate_me);
      printf("pipe found after\n");
      print_command(command_to_append);
      chunk[0] = '\0';
      operator_to_append = malloc(sizeof(struct command));
      parse_pair_to_operator_command(pair, operator_to_append);
      append_to_list(operator_to_append, iterate_me);
    }
    else if (pair[0] == ';') {
      command_to_append = malloc(sizeof(struct command));
      parse_chunk_to_command(chunk, command_to_append);
      append_to_list(command_to_append, iterate_me);
      printf("semicolon found after\n");
      print_command(command_to_append);
      chunk[0] = '\0';
      operator_to_append = malloc(sizeof(struct command));
      parse_pair_to_operator_command(pair, operator_to_append);
      append_to_list(operator_to_append, iterate_me);
    }
    else if (strcmp(pair,"\n\n") == 0)
    {
      command_to_append = malloc(sizeof(struct command));
      parse_chunk_to_command(chunk, command_to_append);
      append_to_list(command_to_append, iterate_me);
      print_command(command_to_append);
      printf("print our linked list of nodes\n");
      print_tree_list(iterate_me);
      // command_t insert_me = build_command_tree(iterate_me);
      // append_to_list(insert_me, command_stream);
      iterate_me->head = NULL;
      iterate_me->tail = NULL;
      chunk[0] = '\0';
      i++;
      //pop things off and implement the algorithm
    }
    // else if (pair[0] == '\n') {
    //   //make a new tree/node in the linked list
    //   //if the previous symbol was a binary operator, ;
    //   //else
    //   //TODO: '\n\n', '\n when the command is complete', '\n when the command isn't complete.
    //   printf("breaking off command here\n");
    //   chunk[0] = '\0';
    // }
    else {
      chunk_len = strlen(chunk);
      chunk[chunk_len] = pair[0];
      chunk[++chunk_len] = '\0';
    }
  }

  return 0;


}


command_t read_command_stream (command_stream_t s)
{

  /* FIXME: Replace this with your implementation too.  */
  error (1, 0, "command reading not yet implemented");
  return 0;
}
