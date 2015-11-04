// UCLA CS 111 Lab 1 main program

#include <errno.h>
#include <error.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "linked-list.h"
#include "command-internals.h"
#include "command.h"

static char const *program_name;
static char const *script_name;

int dependency_exists(char** list1, char** list2) {
  while (list1[0] != NULL) {
   // printf("list item: %s\n", *list1);
    while (list2[0] != NULL) {
      // two words are the same
      if (strcmp(list1[0], list2[0]) == 0) {
        return 1;
      }   
      list2++;
    }
    list1++;
  }
  return 0;
}

static void
usage (void)
{
  error (1, 0, "usage: %s [-pt] SCRIPT-FILE", program_name);
}

static int
get_next_byte (void *stream)
{
  return getc (stream);
}

int
main (int argc, char **argv)
{
  int opt;
  int command_number = 1;
  int print_tree = 0;
  int time_travel = 0;
  program_name = argv[0];

  for (;;)
    switch (getopt (argc, argv, "pt"))
      {
      case 'p': print_tree = 1; break;
      case 't': time_travel = 1; break;
      default: usage (); break;
      case -1: goto options_exhausted;
      }
 options_exhausted:;

  // There must be exactly one file argument.
  if (optind != argc - 1)
    usage ();

  script_name = argv[optind];
  FILE *script_stream = fopen (script_name, "r");
  if (! script_stream)
    error (1, errno, "%s: cannot open", script_name);
  command_stream_t command_stream =
    make_command_stream (get_next_byte, script_stream);

  command_t last_command = NULL;
  command_t command;
  command_node* inner_current;

  int read_array_len = 0;

  if(!print_tree) {

    // iterate through command stream to build dependency list, information is stored in command nodes
    while (command_stream->current != NULL) {
      command_stream->current->read_list = malloc(sizeof(char*) * 32);
      command_stream->current->write_list = malloc(sizeof(char*) * 32);
      command_stream->current->dependencies = malloc(sizeof(struct command_node*)*20);
      command_stream->current->num_dependencies = 0;
      if(command_stream->current->command->input != NULL)
      {
          command_stream->current->read_list[0] = malloc(sizeof(char) * 100);
          read_array_len++;
          strcpy(command_stream->current->read_list[0], command_stream->current->command->input);
      }
      if(command_stream->current->command->output != NULL)
      {
          command_stream->current->write_list[0] = malloc(sizeof(char) * 100);
          strcpy(command_stream->current->write_list[0], command_stream->current->command->output);
      }
      // index starts at 1 so we don't include the command name
      int iterator = 1;
      while(command_stream->current->command->u.word[iterator]!=NULL)
      {
        command_stream->current->read_list[read_array_len]=malloc(sizeof(char*) * 40);
        strcpy(command_stream->current->read_list[read_array_len],command_stream->current->command->u.word[iterator]);
        iterator++;
        read_array_len++;
      }

      inner_current = command_stream->head;
      while (inner_current != command_stream->current) {
        if ((dependency_exists(inner_current->read_list, command_stream->current->write_list))  || // "possible RAW data race"
            (dependency_exists(inner_current->write_list, command_stream->current->write_list)) || // "possible WAR data race"
            (dependency_exists(inner_current->write_list, command_stream->current->read_list)))    // "possible WAW data race"
          {
            // add pointer to inner_current in current's dependencies
            command_stream->current->dependencies[command_stream->current->num_dependencies++] = inner_current;
         //   printf("Yes\n");
          }
        inner_current = inner_current->next;
      }

 /*     printf("the readlist is %s\n", command_stream->current->read_list[0]);
      printf("the writelist is %s\n", command_stream->current->write_list[0]);
      printf("dependency: %d\n", dependency_exists(command_stream->current->read_list, command_stream->current->write_list));*/
      command_stream->current = command_stream->current->next;
    }
  
    command_stream->current = command_stream->head;

    // loop through
    while (command_stream->current != NULL) {
      pid_t child_pid = fork();
      if(child_pid == 0) //child
      {
        int ind = 0;
        //check dependency
        for(ind; ind<command_stream->current->num_dependencies; ind++)
        {
          if(command_stream->current->dependencies[ind]->command->status == -1)
          {
            ind--;
          }
        }
        execute_command(command_stream->current->command, 1);
      }
      else //parent
      {
          ;
      }
      command_stream->current = command_stream->current->next;
    }

    last_command = command_stream->tail->command;
  }  //end of no print_tree


  else 
  {
      while ((command = read_command_stream (command_stream))) {
      printf ("# %d\n", command_number++);
      print_command (command);
    } 
  }  // end of print tree

/*

  while ((command = read_command_stream (command_stream)))
    {
      if (print_tree)
	{
	  printf ("# %d\n", command_number++);
	  print_command (command);
	}
      else
	{
	  last_command = command;
	  execute_command (command, time_travel);
	}
    }*/

  return print_tree || !last_command ? 0 : command_status (last_command);
}
