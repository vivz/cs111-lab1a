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

void remove_last_node(command_list* list, command_t popped){
	popped = NULL;
	if (list->tail!= NULL){
		popped = list->tail->command;
		printf("hey!!\n");
		print_command(popped);
  		printf("popped command_type: %d\n", popped->type);
	}

	if(list->head!=NULL)
	{
		command_node* curr = list->head;
		// if only one element in list
		if(curr==list->tail)
		{
			list->head=NULL;
			list->tail=NULL;
			popped=curr->command;
		}
		while (curr->next!=list->tail) {
			curr = curr->next;
		}
		curr->next=NULL;
		list->tail=curr;
	}//potential seg fault

}




void print_list(command_list* list) {

	command_node* curr = list->head;
	
	while (curr != NULL) {
		print_command(curr->command);
		curr = curr->next;
	}
}