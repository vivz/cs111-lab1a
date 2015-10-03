#include "linked-list.h"

void stack_push(command_t c, struct command_list* stack) {
	append_to_list(c, stack);
}
command_t stack_top(struct command_list* stack) {
	if (is_empty(stack)) {
		return NULL;
	} 
	else {
		return stack->tail->command;
	}
}
command_t stack_pop(struct command_list* stack) {
	return remove_last_node(stack);
}