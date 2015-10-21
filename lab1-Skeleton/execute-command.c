// UCLA CS 111 Lab 1 command execution

#include "command.h"
#include "command-internals.h"

#include <error.h>

/* FIXME: You may need to add #include directives, macro definitions,
   static function definitions, etc.  */

int
command_status (command_t c)
{
  return c->status;
}

void exec_and(command_t and_command) {
    // pid_t left_child_pid;
    // pid_t right_child_pid; 
}


void
execute_command (command_t c, int time_travel)
{
  /* FIXME: Replace this with your implementation.  You may need to
     add auxiliary functions and otherwise modify the source code.
     You can also use external functions defined in the GNU C Library.  */

  switch (c->type) {
    case AND_COMMAND: 
        break;
    case SEQUENCE_COMMAND:
        break;
    case OR_COMMAND:
        break;
    case PIPE_COMMAND:
        break;
    case SIMPLE_COMMAND:
        break;
    case SUBSHELL_COMMAND:
        break;
    default:
        error(1,0,"invalid command");
  }
}
