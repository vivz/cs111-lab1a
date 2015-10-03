#include "linked-list.h"
#include "command.h"
#include "command-internals.h"

void append_to_list(command_t insert_me, command_list* list) {

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

int is_empty(command_list list) {
	return (list.head == NULL);
}

command_t remove_last_node(command_list* list){
	command_t bye=list->tail;
	if(list->head!=NULL)
	{
		command_node* curr = list->head;
		while (curr->next!=list->tail) {
			curr = curr->next;
		}
		curr->next=NULL;
		list->tail=curr;
	}//potential seg fault
	return bye;
}




void print_list(command_list* list) {

	command_node* curr = list->head;
	
	while (curr != NULL) {
		print_command(curr->command);
		curr = curr->next;
	}
}