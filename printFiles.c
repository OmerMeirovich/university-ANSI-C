#include "header.h"
/*
 * printFiles.c
 *
 *  Created on: Jul 25, 2018
 *      Author: student
 */
/*This file contains function used for file handling*/

/*openInputFile function - used to open files.
 takes 3 char pointers - filename, mode and extension.
 returns a pointer to the opened file, or null if failed*/
FILE * openFile(char* filename, char * mode, char * extension) {
	/*Temp file to be returned*/
	FILE * tmp = NULL;
	/*Full filename string*/
	char full_filename[MAX_FILENAME];
	/*Copy the filename from argv to the full filename string*/
	strcpy(full_filename, filename);
	/*Add the .as extension to the full filename.*/
	strcat(full_filename, extension);
	/*Open the file. if fopen return null, exit the function and print error to screen*/
	if (!(tmp = fopen(full_filename, mode))) {
		printf("Can't open file %s.\n", full_filename);
		return NULL;
	}
	/*Return pointer to the file*/
	return tmp;
}

/*This function exports the output files*/
void exportFiles(int IC, int DC, char * name) {
	/*FILE pointers*/
	FILE * object = NULL;
	FILE * entries = NULL;
	FILE * externs = NULL;
	/*Word holder*/
	unsigned short int * toPrint;
	/*Extern flag*/
	int nExtern = FALSE;
	/*Entry flag*/
	int nEntry = 0;
	/*Pointer to wordNode*/
	wordNode * p = NULL;
	/*Pointer to entry*/
	entry * pe = NULL;
	/*Pointer to data*/
	data * pd = NULL;
	/*open an object file in write mode*/
	object = openFile(name, "w", OB_EXTENSION);
	/*assign wordNode pointer with wordNode head*/
	p = getHeadWord();
	/*Print the number of words and number of data in binary*/
	fprintf(object, "\t\t%d %d\n", countWords(), countData());
	/*go through the entire words list*/
	while (p) {
		/*Assign toPrint with corresponding word*/
		switch (p->method) {
		case NONE:
			toPrint = (unsigned short int *) (&(p->holder.first));
			break;
		case IMMEDIATE:
			toPrint = (unsigned short int *) (&(p->holder.immediate));
			break;
		case JUMP:
			toPrint = (unsigned short int *) (&(p->holder.immediate));
			break;
		case DIRECT:
			toPrint = (unsigned short int *) (&(p->holder.direct));
			break;
		case REGISTER:
			toPrint = (unsigned short int *) (&(p->holder.reg));
			break;
		}
		/*Print the address and word in binary */
		printWordsBinary(object, p, *toPrint);
		/*If we found an external word, set flag to 0*/
		if (p->isExternal == TRUE) {
			nExtern = TRUE;
		}
		p = p->next;
	}
	/*Assign pointer with data list head*/
	pd = getHeadData();
	/*print all the data & addresses in binary*/
	while (pd) {
		/*fprintf(object, "%d %d\n", pd->address, pd->value);*/
		printDataBinary(object, pd->address, pd->value);
		pd = pd->next;
	}
	/*Assign pointer with entry list head*/
	pe = getHeadEntry();
	/*If the list is not empty*/
	if (pe) {
		/*open a file in write mode for entries*/
		entries = openFile(name, "w", ENT_EXTENSION);
		/*go through the entry list and write the labels and addresses in binary*/
		while (pe) {
			fprintf(entries, "%s %d\n", pe->label, pe->address);
			pe = pe->next;
			nEntry++;
		}
	}
	/*If we found an extern word earlier*/
	if (nExtern) {
		/*open an extern file in write mode*/
		externs = openFile(name, "w", EXT_EXTENSION);
		/*assign pointer with word list head*/
		p = getHeadWord();
		/*Go through the list*/
		while (p) {
			/*if the word is external*/
			if (p->isExternal == TRUE) {
				/*print the label and address in binary*/
				fprintf(externs, "%s %d\n", p->externalLabel, p->address);
			}
			p = p->next;
		}
	}
	printf("Success! files exported.\n");
	/*Clean up*/
	if (object)
		fclose(object);
	if (entries)
		fclose(entries);
	if (externs)
		fclose(externs);
}
/*Internal Function for printing the file.ob , In order to print all BITS as intended
 * printDataBinary -> Handles the Data that uses short int (can get negative numbers)
 * printtWordsBinary -> Handles the words that uses unsigned short int (no noegative numbers)
 * reverseStr -> Reverse a string, used only in the two functions above
 * */

/*Internal function to convert the data bit field numbers into a binary sequence and print it into a given file pointer*/
void printDataBinary(FILE *object, int address, short int toPrint) {
	int i = 0;
	/*Is number negative, 1 - negative , 0 - positive*/
	int negativeFlag = 0;
	/*binNum will hold the binary sequence as a char string, 14 bits is the highest amount of bits of a word*/
	char * bitNum = malloc(sizeof(char) * MAX_LINE);
	/*Temporary short int variable*/
	unsigned short int tmp = 0;

	if (toPrint < 0)
		negativeFlag = 1;

	if (negativeFlag == 1)
		tmp = (toPrint * -1);
	else
		tmp = toPrint;

	/*fprintf(object, "%d %d\n", address, (int) toPrint);*/
	fprintf(object, "%d\t", address);

	for (; tmp; tmp >>= 1) {

		if ((tmp & 1)) {
			bitNum[i] = '/';
		} else {
			bitNum[i] = '.';
		}
		i++;
	}
	/*Use the reverse string method*/
	reverseStr(bitNum);
	/*If number is NOT negative print bits normally*/
	if (negativeFlag == 0) {
		/*Print zeros to fill a 14 bit line in case the numebr represented is less than 14 bits*/
		for (i = 0; i < (14 - strlen(bitNum)); i++) {
			fprintf(object, ".");
		}
		/*Print the number representation in bits*/
		for (i = 0; i < strlen(bitNum); i++) {
			fprintf(object, "%c", bitNum[i]);
		}

		fprintf(object, "\n");
	}
	/*If number IS negative print opposite bits (if 9 = 1001 then -9 = 0110)*/
	else {
		/*Print zeros to fill a 14 bit line in case the numebr represented is less than 14 bits*/
		for (i = 0; i < (14 - strlen(bitNum)); i++) {
			fprintf(object, "/");
		}
		/*Print the number representation in bits*/
		for (i = 0; i < strlen(bitNum); i++) {
			if (bitNum[i] == '.')
				fprintf(object, "/");
			else
				fprintf(object, ".");
		}

		fprintf(object, "\n");
	}
}
/*Internal function to convert the bit field number into a binary sequence and print it into a given file pointer*/
void printWordsBinary(FILE * object, wordNode* p, unsigned short int toPrint) {

	int i = 0;
	/*binNum will hold the binary sequence as a char string, 14 bits is the highest amount of bits of a word*/
	char * bitNum = malloc(sizeof(char) * 14);
	/*Temporary short int variable*/
	unsigned short int tmp = (int) toPrint;

	/*Print the address in decimal value*/
	fprintf(object, "%d\t", (int) p->address);

	/*Loop over the number with bit operation and add the binary repesenation to the binNum*/
	for (; tmp; tmp >>= 1) {

		if ((tmp & 1)) {
			bitNum[i] = '/';
		} else {
			bitNum[i] = '.';
		}
		i++;
	}
	/*Use the reverse string method*/
	reverseStr(bitNum);
	/*Print zeros to fill a 14 bit line in case the numebr represented is less than 14 bits*/
	for (i = 0; i < (14 - strlen(bitNum)); i++) {
		fprintf(object, ".");
	}
	/*Print the number representation in bits*/
	for (i = 0; i < strlen(bitNum); i++) {
		fprintf(object, "%c", bitNum[i]);
	}
	fprintf(object, "\n");

}

/*Internal function to reverse a string pointer used for reversing the binary represantion in print binary*/
void reverseStr(char *s) {
	int length, c;
	char *begin, *end, temp;

	length = strlen(s);
	begin = s;
	end = s;

	for (c = 0; c < length - 1; c++)
		end++;

	for (c = 0; c < length / 2; c++) {
		temp = *end;
		*end = *begin;
		*begin = temp;

		begin++;
		end--;
	}
}
