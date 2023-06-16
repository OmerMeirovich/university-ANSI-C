#include "header.h"

static symbol * head = NULL;

/*Add a symbol to the symbol list*/
void addSymbol(char * label, int address, int external, int operation, int line) {
	/*Temporary symbol*/
	symbol * tmp = NULL;
	/*Check if symbol is already in the list*/
	checkSymbol(label, line);
	/*Allocate memory for tmp symbol*/
	tmp = malloc(sizeof(symbol));
	/*Check allocation*/
	checkAllocation(tmp);
	/*Allocate memory for label string*/
	tmp->label = malloc(sizeof(char) * MAX_LINE);
	/*Check allocation*/
	checkAllocation(tmp->label);
	/*Copy the label*/
	strcpy(tmp->label, label);
	/*Copy value of DC*/
	tmp->address = address;
	/*Copy external flag*/
	tmp->external = external;
	/*Copy operation*/
	tmp->operation = operation;
	/*Set next to NULL*/
	tmp->next = NULL;

	/*If list is empty*/
	if (!head) {
		head = tmp;
	} else {
		/*P point to check every item on the list*/
		symbol * p = head;
		/*Get to last node on the list*/
		while (p->next)
			p = p->next;
		/*Set tmp to ne the new last node*/
		p->next = tmp;
	}
}

/*Check if symbol already exists*/
void checkSymbol(char * label, int line) {
	symbol * p = head;
	while (p) {
		if (!strcmp(p->label, label)) {
			addError("Same label has multiple symbols", line, label);
			return;
		}
		p = p->next;
	}
}

/*Check if a symbol with the same label parameter exists, return it if it exists other wise reuturn NULL*/
symbol * searchSymbol(char * label) {
	symbol *p = head;
	while (p) {
		if (!strcmp(p->label, label)) {
			return p;
		}
		p = p->next;
	}
	return NULL;
}

/*Update symbols list with the current IC counter given*/
void updateSymbols(int IC) {
	symbol * p = head;
	while (p) {
		if ((p->external == FALSE) && (p->operation == FALSE)) {
			p->address += IC;
		}
		p = p->next;
	}
}

/*Empty the symbol list*/
void clearSymbols() {
	symbol * p = head;
	if (!p)
		return;

	while (p) {
		symbol * tmp = p;
		p = p->next;
		free(tmp->label);
		free(tmp);
	}
	head = NULL;
}

