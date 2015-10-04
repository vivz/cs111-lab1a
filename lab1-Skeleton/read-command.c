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
  int status;
  int word_len;

  simple_command->type = SIMPLE_COMMAND;
  simple_command->u.word = malloc(20*sizeof(char*));
  char word_to_store[256] = ""; 
  for (i = 0; i < chunk_len; i++) {

    switch(chunk[i]) {
      case '<':
        if (status == NEXT_IS_OUTPUT) {
          simple_command->output = word_to_store;
        }
        else 
          simple_command->u.word[num_words++] = word_to_store;
        word_to_store[0] = '\0';
        status = NEXT_IS_INPUT;
        break;
      case '>':
        if (status == NEXT_IS_INPUT) {
          simple_command->input = word_to_store;
        }
        else 
          simple_command->u.word[num_words++] = word_to_store;
        word_to_store[0] = '\0';
        status = NEXT_IS_OUTPUT;
        break;
      case ' ':
        if (word_to_store[0] == '\0') {
          ;
        }
        else if (status == NEXT_IS_OUTPUT) {
          simple_command->output = word_to_store;
          status = NEXT_IS_WORD;
        }
        else if (status == NEXT_IS_INPUT) {
          simple_command->input = word_to_store;
          status = NEXT_IS_WORD;
        }
        else {
          // we have a word
          simple_command->u.word[num_words++] = word_to_store;
        }
        break;
      default:
        word_len = strlen(word_to_store);
        word_to_store[word_len] = chunk[i];
        word_to_store[++word_len] = '\0';
    }
  }
  simple_command->u.word[num_words] = NULL;
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
  command_t new_command0 = malloc(sizeof(struct command));
  new_command0->type = SIMPLE_COMMAND;
  new_command0->status = -1;
  new_command0->u.word = malloc(3 * sizeof(char*));
  new_command0->u.word[0] = "echo";
  new_command0->u.word[1] = "a";
  new_command0->u.word[2] = NULL;
  new_command0->output = "out";

  command_t new_command1 = malloc(sizeof(struct command));
  new_command1->type = SIMPLE_COMMAND;
  new_command1->status = -1;
  new_command1->u.word = malloc(3 * sizeof(char*));
  new_command1->u.word[0] = "cat";
  new_command1->u.word[1] = "b";
  new_command1->u.word[2] = NULL;

  command_t new_command2 = malloc(sizeof(struct command));
  new_command2->type = OR_COMMAND;
  new_command2->status = -1;
  new_command2->u.command[0] = new_command0;
  new_command2->u.command[1] = new_command1;


  command_list command_list_test;
  command_list_test.head=NULL;
  command_list_test.tail=NULL;



  //populate list
  printf("pre append to list\n");
  append_to_list(new_command0, &command_list_test);
  append_to_list(new_command1, &command_list_test);
  append_to_list(new_command2, &command_list_test);
  printf("printing list after append\n");
  print_list(&command_list_test);

  //pop
  printf("removing last node\n");
  remove_last_node(&command_list_test);

  //print list
  printf("pre print list\n");
  print_list(&command_list_test);

  
  */

  char* test_string = "sort   a < b  > c";
  // should output "sort a<b>c"
  command_t test_command = malloc(sizeof(struct command));
  printf("pre parse\n");
  parse_chunk_to_command(test_string, test_command);
  printf("pre print\n");
  print_command(test_command);
  printf("post print command\n");


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
  printf("buffer: %s\n", buffer);

  char pair[2]; 
  char chunk[256] = "";
  int chunk_len;
  int i = 0;

  // is command complete
  // i.e. did we just see an AND, OR, or PIPE operator
  int state;

  for (i = 0; i < len; i++) {
    pair[0] = buffer[i];
    pair[1] = buffer[i+1];


    if (strcmp(pair, "&&") == 0 || strcmp(pair, "||") == 0)  {
      //push to the stack 
      printf("chunk: %s\n", chunk);
      chunk[0] = '\0';
      printf("token pair: %s\n", pair);
      i++;
    } 
    else if (pair[0] == '|') {
      //push to t
      printf("chunk: %s\n", chunk);
      chunk[0] = '\0';
      printf("token pair: %c\n", pair[0]);
    }
    else if (pair[0] == '\n') {
      //make a new tree/node in the linked list
      printf("chunk: %s\n", chunk);
      chunk[0] = '\0';
      printf("newline found\n");
    }
    else {
      chunk_len = strlen(chunk);
      chunk[chunk_len] = pair[0];
      chunk[++chunk_len] = '\0';
    }

  }

  printf("last chunk: %s ", chunk);
  return 0;
}



command_t read_command_stream (command_stream_t s)
{

  /* FIXME: Replace this with your implementation too.  */
  error (1, 0, "command reading not yet implemented");
  return 0;
}
