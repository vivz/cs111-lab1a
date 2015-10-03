#include "linked-list.h"

void append_to_list(command_t insert_me, command_list* list) {

	//creating a node
	command_node node_to_insert;
	printf("assigning command...\n");
	node_to_insert.command = insert_me;
	printf("assigning next...\n");
	node_to_insert.next = NULL;
	printf("assigning tail next...\n");


	if (NULL==NULL) {
		printf("Null equals null\n");
	} else {
		printf("null is not null\n");
	}
	//if empty
	if (list->head == NULL) {
		printf("empty \n");
		list->head = &node_to_insert;
		list->tail = &node_to_insert;
	}

	//if not empty
	else {
		printf("not empty\n");
		list->tail->next = &node_to_insert;
		list->tail = list->tail->next;
		list->tail->next = NULL;
	}
	printf("checking if empty\n");
}

int is_empty(command_list list) {
	return (list.head == NULL);
}

void print_list(command_list* list) {
	command_node* curr = list->head;
	while (curr != NULL) {
		printf("in while loop\n");
		print_command(curr->command);
		printf("%s/n",curr->command->u.word[0]);
		curr = curr->next;
	}
}