#include "linked-list.h"
#include "command.h"
#include "command-internals.h"

void append_to_list(command_t insert_me, command_list* list) {

	//creating a node
	command_node node_to_insert;
	node_to_insert.command = insert_me;
	node_to_insert.next = NULL;

	//if empty
	if (list->head == NULL) {
		list->head = &node_to_insert;
		list->tail = &node_to_insert;
	}

	//if not empty
	else {
		list->tail->next = &node_to_insert;
		list->tail = list->tail->next;
		list->tail->next = NULL;
	}
/*
	if(list->head->command->type==SIMPLE_COMMAND)
	{
		printf("dude.\n");
	}*/
	printf("type in append is %i\n",list->head->command->type);
}
/*
int is_empty(command_list list) {
	return (list.head == NULL);
}*/

void print_list(command_list* list) {
	
	printf("type in print is %i\n",list->head->command->type);
//	print_command(list->head->command);
//	command_node* curr = list->head;
//	print_command(curr->command);
	/*
	while (curr != NULL) {
		printf("in while loop\n");
		print_command(curr->command);
		//printf("%s/n",curr->command->u.word[0]);
		curr = curr->next;
	}*/
}