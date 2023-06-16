/*
 * entryList.c
 *
 *  Created on: Jul 12, 2018
 *      Author: Omer meirovich
 */
#include "header.h"

/*Head of entry list*/
static entry * head = NULL;

/*Add an entry to the entry list*/
void addEntry(char * label, int line) {
	/*Temp pointer to entry*/
	entry * tmp = NULL;
	/*Check if a same label in  entry exists*/
	if (!(checkEntry(label))) {
		addError("More than one entry for the same label.", line, label);
		return;
	}
	/*Allocate memory for new node*/
	tmp = malloc(sizeof(entry));
	checkAllocation(tmp);
	/*Copy the label*/
	tmp->label = malloc(sizeof(char) * MAX_LINE);
	strcpy(tmp->label, label);
	/*Assign parameters*/
	tmp->address = 0;
	tmp->line = line;
	tmp->hasAddress = FALSE;
	tmp->next = NULL;
	/*If head is null*/
	if (!head) {
		head = tmp;
	}
	/*Get to last node and add the entry to the end of the list*/
	else {
		entry * ptr = head;
		while (ptr->next)
			ptr = ptr->next;
		ptr->next = tmp;
	}
}

/*Check if entry with the same label exists, If it exists return 0 else return 1*/
int checkEntry(char * label) {
	entry * ptr = head;
	/*Go through the list*/
	while (ptr) {
		if (!strcmp(ptr->label, label)) {
			/*If label was found return 0*/
			return 0;
		}
		ptr = ptr->next;
	}
	/*If no such label was found return 1*/
	return 1;
}

/*Get the head of the entry list*/
entry * getHeadEntry() {
	entry * tmp = head;
	return tmp;
}

/*Empty entry list*/
void clearEntries() {
	entry * ptr = head;
	if (!ptr)
		return;
	while (ptr) {
		entry * tmp = ptr;
		ptr = ptr->next;
		free(tmp);
	}
	head = NULL;
}

/*Return an entry without an assigned address, used in second pass to assign an address to each entry*/
entry * getEntry() {
	entry *ptr = head;
	while (ptr) {
		if (!ptr->hasAddress) {
			return ptr;
		}
		ptr = ptr->next;
	}
	return NULL;
}

