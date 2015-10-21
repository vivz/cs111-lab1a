#include "linked-list.h"
#include "command.h"
#include "command-internals.h"

void append_to_list(command_t insert_me, command_stream* list) {

	//creating a node
	command_node* node_to_insert = malloc(sizeof(command_node));
	node_to_insert->command = insert_me;
	node_to_insert->next = NULL;

	//if empty
	if (list->head == NULL) {
		list->head = node_to_insert;
		list->tail = node_to_insert;
	}

	//if not empty
	else {
		list->tail->next = node_to_insert;
		list->tail = list->tail->next;
		list->tail->next = NULL;
	}
}



void remove_last_node(command_stream* list, command_t* popped){
	if (list->tail!= NULL){
		*popped = list->tail->command;
		// printf("hey!!\n");
		// print_command(*popped);
  		// printf("popped command_type: %d\n", (*popped)->type);
	}
	else 
		*popped=NULL;

	if(list->head!=NULL)
	{
		command_node* curr = list->head;
		// if only one element in list
		if(curr==list->tail)
		{
			list->head=NULL;
			list->tail=NULL;
			*popped=curr->command;
		}
		while (curr->next!=list->tail) {
			curr = curr->next;
		}
		curr->next=NULL;
		list->tail=curr;
	}//potential seg fault

}




void print_list(command_stream* list) {

	command_node* curr = list->head;
	
	while (curr != NULL) {
		print_command(curr->command);
		curr = curr->next;
	}
}

void push_char_stack(char c, char_stack* c_stack) {
	c_stack->stack[c_stack->size++] = c;
	c_stack->stack[c_stack->size] = '\0';
}

void pop_char_stack(char_stack* c_stack) {
	if (c_stack->size > 0) {
		c_stack->size--;
	} else {
		error(1,0,"popping an empty stack");
	}
}

char top_char_stack(char_stack* c_stack) {
	if (c_stack->size > 0) {
		return c_stack->stack[c_stack->size - 1];
	} else {
		error(1,0,"trying to get top of empty stack");
	}
}

int char_stack_empty(char_stack* c_stack) {
	return c_stack->size == 0;
}
