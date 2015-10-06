// Linked List data structure of commands

#include "command.h"
#include <error.h>
#include <stdio.h>
#include <stdlib.h>


typedef struct command_node {
	struct command_node *next;
	command_t command;
} command_node;

typedef struct command_list {
	command_node* head;
	command_node* tail;
	command_node* current;
} command_list;

void append_to_list(command_t, command_list*);

command_t remove_last_node(command_list*);

// int is_empty(command_list* list) { return list->head == NULL; };

void print_list(command_list*);
