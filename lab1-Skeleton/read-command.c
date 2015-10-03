// UCLA CS 111 Lab 1 command reading

#include "command.h"
#include "command-internals.h"

#include <error.h>

/* FIXME: You may need to add #include directives, macro definitions,
   static function definitions, etc.  */

#include <stdio.h>
#include <stdlib.h>
#include "linked-list.h"

/* FIXME: Define the type 'struct command_stream' here.  This should
   complete the incomplete type declaration in command.h.  */

command_stream_t
make_command_stream (int (*get_next_byte) (void *),
		     void *get_next_byte_argument)
{
  /* FIXME: Replace this with your implementation.  You may need to
     add auxiliary functions and otherwise modify the source code.
     You can also use external functions defined in the GNU C Library.
  */

	printf("make_command_stream\n");
  command_t new_command0 = malloc(sizeof(command_t));
  new_command0->type = SIMPLE_COMMAND;
  new_command0->status = -1;
  new_command0->u.word = malloc(3 * sizeof(char*));
  new_command0->u.word[0] = "echo";
  new_command0->u.word[1] = "a";
  new_command0->u.word[2] = NULL;

  command_t new_command1 = malloc(sizeof(command_t));
  new_command1->type = SIMPLE_COMMAND;
  new_command1->status = -1;
  new_command1->u.word = malloc(3 * sizeof(char*));
  new_command1->u.word[0] = "cat";
  new_command1->u.word[1] = "b";
  new_command1->u.word[2] = NULL;

  command_list command_list_test;
  command_list_test.head=NULL;
  command_list_test.tail=NULL;

  //populate list
  printf("pre append to list\n");
  append_to_list(new_command0, &command_list_test);
  append_to_list(new_command1, &command_list_test);

  //pop
  remove_last_node(&command_list_test);

  //print list
  printf("pre print list\n");
  print_list(&command_list_test);

  /*
	char c = get_next_byte(get_next_byte_argument);
 	//printf("%c",c);	
	while (c != EOF)
 	{
		printf("%c",c);
		c = get_next_byte(get_next_byte_argument);
	//	printf("%c", c);
	}*/
  error (1, 0, "command reading not yet implemented");
  return 0;
}

command_t read_command_stream (command_stream_t s)
{

  /* FIXME: Replace this with your implementation too.  */
  error (1, 0, "command reading not yet implemented");
  return 0;
}
