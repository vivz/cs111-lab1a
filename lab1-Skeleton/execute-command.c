// UCLA CS 111 Lab 1 command execution

#include "command.h"
#include "command-internals.h"

#include <error.h>
#include <unistd.h>
#include <sys/stat.h> 
#include <fcntl.h>
#include <sys/wait.h>
#include <stdio.h>
/* FIXME: You may need to add #include directives, macro definitions,
   static function definitions, etc.  */

int
command_status (command_t c)
{
  return c->status;
}


// 
int handle_io(command_t c) {
  int input_redir, output_redir;
  //handles input
  if (c->input != NULL) {
    input_redir = open(c->input, O_RDONLY);
    if (input_redir < 0) 
     // error(1,0,"couldn't find or read input file");
      return -1; 
  
    if (dup2(input_redir, 0) < 0)
      return -1;
      // error(1,0,"can't change standard in");
    
    if(close (input_redir) < 0)
      return -1;
      // error(1,0,"can't close input");
  }
  //handles output
  if (c->output != NULL) {
    output_redir = open(c->output, O_CREAT|O_WRONLY|O_TRUNC, 0644);
    // printf("output: %s\n", c->output);
    if (output_redir < 0) 
        // error(1,0,"don't know where to output");
        return -1;
    if (dup2(output_redir, 1) < 0)
        // error(1,0,"can't change standard out");
        return -1;
    if(close(output_redir) < 0)
        // error(1,0,"can't close output");
        return -1;
  }
  return 0;
}

void execute_simple_command(command_t simple_command) {
    pid_t child_pid;
    int exit_status;
    int io_status=0;

    // make a child process to run the simple command
    child_pid = fork();
    // in the child
    if (child_pid == 0)
    {
        io_status=handle_io(simple_command);
        if(io_status!=0)
        {
          simple_command->status=-1;
          error(1,0,"error reading");
        }
        else {
        execvp(simple_command->u.word[0],simple_command->u.word);
        _exit(simple_command->status);
      }
    }
    //if parent
    if(child_pid > 0)
    {
      waitpid(child_pid, &exit_status, 0);
      simple_command->status = WEXITSTATUS(exit_status);
      if(io_status!=0)
        simple_command->status=-1;
    }
    return;
}

void execute_seq_command(command_t seq_command){
  int exit_status;
  pid_t child_pid = fork();
  pid_t other_child;
  if(child_pid == 0) //child process
  {
    execute_command(seq_command->u.command[0],0);
    _exit(seq_command->u.command[0]->status);
  }
  if(child_pid > 0)//parent process
  {
    waitpid(child_pid,&exit_status,0);
    other_child=fork();
    if(other_child == 0)
    {
      execute_command(seq_command->u.command[1],0);
      _exit(seq_command->u.command[1]->status);
    }
    if(other_child > 0)
    {
      waitpid(other_child, &exit_status, 0);
      seq_command->status=WEXITSTATUS(exit_status);

    }
  }
  return;
}

void execute_or_command(command_t or_command){
  execute_command(or_command->u.command[0], 0);
  if (or_command->u.command[0]->status !=0) {
    execute_command(or_command->u.command[1], 0);
    or_command->status = or_command->u.command[1]->status;
  } else {
    or_command->status = or_command->u.command[0]->status;
  }

}

void execute_and_command(command_t and_command) {
  execute_command(and_command->u.command[0], 0);
  // printf("first status: %d\n", and_command->u.command[0]->status);
  if (and_command->u.command[0]->status == 0) {
    execute_command(and_command->u.command[1], 0);
    and_command->status = and_command->u.command[1]->status;
  } else {
    and_command->status = and_command->u.command[0]->status;
  }
}

void execute_sub_command(command_t sub_command){
  int io_status = 0;
  
  // printf("pre io %s\n", sub_command->output);
  io_status=handle_io(sub_command);
  
  if(io_status!=0)
  {
      sub_command->status=-1;
      error(1,0,"error reading");
  }
  else 
  { 
      // printf("sub words %s\n", sub_command->u.subshell_command->u.word[0]);
      execute_command(sub_command->u.subshell_command,0);
      sub_command->status=sub_command->u.subshell_command->status;
  }
}

void execute_pipe_command(command_t pipe_command){
  pid_t child_pid;
  pid_t other_child;
  pid_t returned;
  int exit_status;

  int io[2];

  pipe(io);

  child_pid = fork();

  if(child_pid == 0)  //child of the first child
  {
    close(io[1]);
    if(dup2(io[0],0) < 0)
    {
      error(1,0, "can't change stdin");
    }
    execute_command(pipe_command->u.command[1], 0);
    _exit(pipe_command->u.command[1]->status);
  }
  else if(child_pid > 0)  //parent of the first child 
  {
    other_child=fork(); //child of the second child
    if(other_child == 0)
    {
      close(io[0]);
      if(dup2(io[1],1) < 0) 
      {
        error(1,0,"can't change stdout");
      }
      execute_command(pipe_command->u.command[0], 0);
      _exit(pipe_command->u.command[0]->status);
    }
    else if(other_child > 0) //parent of the second child
    {
      returned = waitpid (-1, &exit_status,0);
      close(io[0]);
      close(io[1]);
      if(other_child == returned)
      {
        waitpid(child_pid, &exit_status,0);
        pipe_command->status=WEXITSTATUS(exit_status);
      }
      if(child_pid == returned)
      {
          waitpid(other_child,&exit_status,0);
          pipe_command->status = WEXITSTATUS(exit_status);
      }
    }
  }
  return;
}

void
execute_command (command_t c, int time_travel)
{
  /* FIXME: Replace this with your implementation.  You may need to
     add auxiliary functions and otherwise modify the source code.
     You can also use external functions defined in the GNU C Library.  */

  switch (c->type) {
    case AND_COMMAND: 
        execute_and_command(c);
        break;
    case SEQUENCE_COMMAND:
        execute_seq_command(c);
        break;
    case OR_COMMAND:
        execute_or_command(c);
        break;
    case PIPE_COMMAND:
        execute_pipe_command(c);
        break;
    case SIMPLE_COMMAND:
        execute_simple_command(c);
        break;
    case SUBSHELL_COMMAND:
        execute_sub_command(c);
        break;
    default:
        error(1,0,"invalid command");
  }
}
