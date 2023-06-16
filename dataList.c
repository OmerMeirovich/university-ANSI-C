#include "header.h"
/*This file contains functions which handle the data list*/

/*pointer to the first node*/
static data * head = NULL;

/*Get a string an split it to get each number parameter, If an empty string was recieved
 * add an error, Otherwise get each string token and use the addNumber to add the number token to the list*/
void splitNumbers(char * token, int line, int * DC) {
	/*Token is null*/
	if (!(token = strtok(NULL, " ,\t\n"))) {
		addError("No numbers were given", line, NULL);
		return;
	} else {
		/*Send the first token to addNumber()*/
		addNumber(token, line, DC);
		/*Send the rest of the tokens to addNumber()*/
		while ((token = strtok(NULL, " ,\t\n"))) {
			addNumber(token, line, DC);
		}
	}
}

/*Add a number to the dataList, Check if given string is a number and if the number is in range.*/
void addNumber(char * number, int line, int * DC) {
	/*Pointer for strtol string to number error checking*/
	char * ptr = NULL;
	/*Temporary data node*/
	data * tmp = NULL;
	/*Temporary holder for the converted number*/
	long int tmpNum;

	/*Convert the string to a number*/
	tmpNum = strtol(number, &ptr, 10);
	/*If strtol failed 0 is returned*/
	if (*ptr != '\0') {
		addError("Invalid number", line, number);
		return;
	}
	/*If number is out of 12 bit range (4096)*/
	if ((tmpNum > MIN_NUMBER_DATA) || (tmpNum < MAX_NUMBER_DATA)) {
		addError("Number is out of range", line, number);
		return;
	}
	/*Create new pointer to data, allocate memory*/
	tmp = malloc(sizeof(data));
	checkAllocation(tmp);
	/*Cast tmpNum(long) to a short int type, and assign to the tmp data node*/
	tmp->value = (short int) tmpNum;
	tmp->next = NULL;
	/*If the list is empty*/
	if (!head) {
		head = tmp;
	} else {
		/*Temporary head pointer*/
		data * ptr = head;
		/*Get to last node*/
		while (ptr->next)
			ptr = ptr->next;
		/*Make tmp the last node*/
		ptr->next = tmp;
	}
	/*Increment data count*/
	(*DC)++;
}

/*Add a string to the datalist, Use addChar to add each char of the string*/
void addString(char * string, int line, int *DC) {
	/*If string given is empty, no ""*/
	if (!strcmp(string, "\"")) {
		/*add error and return*/
		addError("No string was given", line, NULL);
		return;
	}
	/*Check if string is surrounded by quotation marks*/
	if ((*string == '"') && (string[strlen(string) - 1] == '"')) {
		/*"Cut" last quotation mark*/
		string[strlen(string) - 1] = '\0';
		/*"Cut" first quotation mark*/
		string++;
	} else {
		/*If string is not wrapped with quotation mark*/
		addError("Strings must be surrounded by quotation marks", line, NULL);
		return;
	}

	/*Loop in order to add each char to the list*/
	while (*string != '\0') {
		/*Add the character to the data list*/
		addChar(*string);
		/*Point string to next char*/
		string++;
		/*Increment data count*/
		(*DC)++;
	}
	/*Add null terminator, The end of the string*/
	addChar(0);
	/*Increment data count*/
	(*DC)++;
}

/*Add a single char to the list, used when adding a string*/
void addChar(char c) {
	/*Temporary data node*/
	data * tmp = malloc(sizeof(data));
	/*Assign node with character and set next to NULL*/
	tmp->value = c;
	tmp->next = NULL;
	/*If list is empty*/
	if (!head) {
		/*make head point to temp*/
		head = tmp;
	} else {
		/*Temp pointer to head*/
		data * ptr = head;
		/*Get last node*/
		while (ptr->next)
			ptr = ptr->next;
		/*Make tmp the last node*/
		ptr->next = tmp;
	}
}

/*Count the number of nodes in the list*/
int countData() {

	int counter = 0;
	/*Temp pointer to head*/
	data * tmp = head;
	/*If head is null, return counter (0)*/
	if (!tmp)
		return counter;

	/*Go through the list and count each node*/
	while (tmp) {
		counter++;
		tmp = tmp->next;
	}
	/*Return the number of nodes*/
	return counter;
}

/*Get the head of the data list.*/
data * getHeadData() {
	data * tmp = head;
	return tmp;
}
/*Update data address for each node only after we have all of our operation words*/
void updateDataAddresses(int IC) {
	/*Temp pointer to head*/
	data * tmp;
	tmp = head;
	/*If head is null exit function*/
	if (!tmp) {
		return;
	}
	/*Go through every node and assign it an address*/
	while (tmp) {
		/*First node gets the BASE IC*/
		tmp->address = IC;
		/*Increment address to be assigned to the next node*/
		IC++;
		tmp = tmp->next;
	}
}

/*Empty the data list*/
void clearData() {
	/*Temp pointer to head*/
	data * ptr = head;
	/*If head is null*/
	if (!ptr)
		return;
	/*Free each data node*/
	while (ptr) {
		data * tmp = ptr;
		ptr = ptr->next;
		free(tmp);
	}
	/*Empty head null, Make it null*/
	head = NULL;
}
