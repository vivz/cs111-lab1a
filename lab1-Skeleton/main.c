// UCLA CS 111 Lab 1 main program

#include <errno.h>
#include <error.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#include "linked-list.h"
#include "command-internals.h"
#include "command.h"

static char const *program_name;
static char const *script_name;

void populate_read_list(command_node* curr_node, command_t cmd, int* read_array_len, int* write_array_len)
{
      if(cmd->input != NULL)
      {
          curr_node->read_list[*read_array_len] = malloc(sizeof(char) * 100);
          strcpy(curr_node->read_list[*read_array_len], cmd->input);
       //   printf("added %s at position %d\n", curr_node->read_list[*read_array_len], *read_array_len);
          *read_array_len = *read_array_len + 1;
      }
      if(cmd->output != NULL)
      {
          curr_node->write_list[*write_array_len] = malloc(sizeof(char) * 100);
          strcpy(curr_node->write_list[*write_array_len], cmd->output);
          *write_array_len = *write_array_len + 1;
      }
      // index starts at 1 so we don't include the command name
     
      if(cmd->type == SIMPLE_COMMAND)
      {
          int iterator = 1; 
          while(cmd->u.word[iterator]!=NULL)
          {
             curr_node->read_list[*read_array_len]=malloc(sizeof(char*) * 40);
             strcpy(curr_node->read_list[*read_array_len],cmd->u.word[iterator]);
       //      printf("added %s at position %d\n", curr_node->read_list[*read_array_len], *read_array_len);
             iterator++;
             *read_array_len = *read_array_len + 1;
          }
      }
      else if (cmd->type == SUBSHELL_COMMAND)
      {
        if(cmd->u.subshell_command != NULL)
          populate_read_list(curr_node, cmd->u.subshell_command, read_array_len, write_array_len);
      }
      else
      {
        if(cmd->u.command[0] != NULL)
        {
          populate_read_list(curr_node, cmd->u.command[0], read_array_len, write_array_len);
        }
        if(cmd->u.command[1] != NULL)
        {
          populate_read_list(curr_node, cmd->u.command[1], read_array_len, write_array_len);
        }
      }
}

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


  if(!print_tree && time_travel) {
    int read_array_len = 0;
    int write_array_len = 0;
    // iterate through command stream to build dependency list, information is stored in command nodes
    while (command_stream->current != NULL) {

      read_array_len = 0;
      write_array_len = 0;
      command_stream->current->read_list = malloc(sizeof(char*) * 32);
      command_stream->current->write_list = malloc(sizeof(char*) * 32);
      command_stream->current->dependencies = malloc(sizeof(struct command_node*)*20);
      command_stream->current->num_dependencies = 0;
      populate_read_list(command_stream->current, command_stream->current->command, &read_array_len, &write_array_len);
      /*
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
        if(command_stream->current->command->type == SIMPLE_COMMAND)
              strcpy(command_stream->current->read_list[read_array_len],command_stream->current->command->u.word[iterator]);
        iterator++;
        read_array_len++;
      }
      */
      inner_current = command_stream->head;
      while (inner_current != command_stream->current) {
        if ((dependency_exists(inner_current->read_list, command_stream->current->write_list))  || // "possible RAW data race"
            (dependency_exists(inner_current->write_list, command_stream->current->write_list)) || // "possible WAR data race"
            (dependency_exists(inner_current->write_list, command_stream->current->read_list)))    // "possible WAW data race"
          {
            // add pointer to inner_current in current's dependencies
          //  printf("hi\n");
            command_stream->current->dependencies[command_stream->current->num_dependencies] = inner_current;
         //   printf("Yes\n");
            command_stream->current->num_dependencies++;
          }
        inner_current = inner_current->next;
      }

      /*printf("the readlist is ================\n");
      int haha = 0 ;
      for(haha; haha<read_array_len; haha++)
        printf("%s\n",command_stream->current->read_list[haha]);

      printf("the writelist is ================\n");
      for(haha = 0; haha<write_array_len; haha++)
        printf("%s\n",command_stream->current->write_list[haha]);

      printf("dependency: %d\n", dependency_exists(command_stream->current->read_list, command_stream->current->write_list));*/
      command_stream->current = command_stream->current->next;
    } 
  
    command_stream->current = command_stream->head;

    // loop through
    while (command_stream->current != NULL) {
   //  printf("the command is %s\n",command_stream->current->command->u.word[1]);
   
     // printf("child_pid is %d\n",child_pid );
      //pid_t child_pid = fork();
    //  if(child_pid == 0) //child
      //{
        int ind = 0;
        int ind2 = 0;
        int num_depp = command_stream->current->num_dependencies;
        //check if dependencies have been started
        for(ind; ind<num_depp; ind++)
        {
          if(command_stream->current->dependencies[ind]->pid == -1)
          {
            ind--;
          }
        }
        //check if dependencies are done
        int eStatus;
        for(ind2; ind2< num_depp; ind2++)
        {
            waitpid(command_stream->current->dependencies[ind2]->pid, &eStatus,0);
            command_stream->current->num_dependencies--;
        }

       
    // }
     pid_t child_pid = fork();
     if(child_pid == 0)
     {
       // printf("command_stream is %s\n", command_stream->current->command->u.word[1]);
        execute_command(command_stream->current->command, 1);
        _exit(0);
    }

      else  //parent
     {
        command_stream->current->pid = child_pid;
        
     }
     command_stream->current = command_stream->current->next;  
    }

    command_stream->current = command_stream->head;

    while (command_stream->current != NULL)
        {
        int exit_status = 0;
        waitpid(command_stream->current->pid, &exit_status, 0);
        command_stream->current->command->status=WEXITSTATUS(exit_status);
        command_stream->current = command_stream->current->next;
        }

    last_command = command_stream->tail->command;
  }  //end of no print_tree

  else if (print_tree)
  {
      while ((command = read_command_stream (command_stream))) {
      printf ("# %d\n", command_number++);
      print_command (command);
    } 
  }  // end of print tree

  else {
      while ((command = read_command_stream (command_stream))) {
        last_command = command;
        execute_command(command, time_travel);
      }
  }
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
