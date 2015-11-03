// Linked List data structure of commands

#include "command.h"
#include <error.h>
#include <stdio.h>
#include <stdlib.h>


typedef struct command_node {
	struct command_node *next;
	command_t command;
    char** read_list;
    char** write_list;

    struct command_node** dependencies;
    int num_dependencies; 
    
} command_node;

typedef struct command_stream {
	command_node* head;
	command_node* tail;
	command_node* current;
} command_stream;

void append_to_list(command_t, command_stream*);

void remove_last_node(command_stream*, command_t*);

// int is_empty(command_stream* list) { return list->head == NULL; };

void print_list(command_stream*);

typedef struct char_stack {
	char stack[1024];
	int size;
} char_stack;

void push_char_stack(char, char_stack*);

void pop_char_stack(char_stack*);

char top_char_stack(char_stack*);

int char_stack_empty(char_stack*);
