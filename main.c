/*
 ============================================================================
 Name        : maman14.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "header.h"

int main(int argc, char **argv) {

	int i;
	int IC = 100;
	int DC = 0;

	if (argc < 2) {
		fprintf(stderr, "%s", "error: No file names were given!\n");
		return FALSE;
	}
	for (i = 1; i < argc; i++) {

		/*Go to first pass*/
		firstPassFunc(argv[i], &IC, &DC);
		/*update symbols*/
		updateSymbols(IC);
		/*Update words list addresses*/
		updateAddresses();
		/*Update data list addresses*/
		updateDataAddresses(IC);

		/*If no errors in first pass*/
		if (!isErrorEmpty()) {
			/*Go to second pass*/
			secondPass(IC);
			/*If no errors in secondPass export files*/
			if (!isErrorEmpty()) {
				/*ExportFiles*/
				exportFiles(IC, DC, argv[i]);
			} else
			/*Errors found in second pass*/
			{
				printErrors(argv[i]);
				/*Free the errors list*/
				clearErrors();
			}
		}
		/*Errors found in first pass*/
		else {
			printErrors(argv[i]);
			/*Free the errors list*/
			clearErrors();
		}
		/*Cleanup*/
		clearData();
		clearSymbols();
		clearEntries();
		clearWords();
	}

	return EXIT_SUCCESS;
}

/*Trim the whole row of a string from \t and whitespace to only One whitespace between each word*/
void trim(char string[]) {
	int sLen, i, lastLetter;
	char cutStr[MAX_LINE];
	/* First trim the front and end part of the string*/
	trimFrontEnd(string);

	sLen = strlen(string);
	lastLetter = -1;
	/*Format string to have defined whitespace between words*/
	for (i = 0; string[i + 1] != '\0' || i < sLen; i++) {
		/*1. if pattern of [Letter][Whitespace] is present save location of letter*/
		if (!isspace(string[i]) && isspace(string[i+1])) {
			lastLetter = i;
		}
		/*2. If pattern of [Whitespace][Letter] is present, cut string out to pattern */
		else if (isspace(string[i]) && !isspace(string[i+1])
				&& isspace(string[i-1]) && lastLetter != -1) {
			/*Add a whitespace at the start of the cutStr*/
			strcpy(cutStr, TRIM_WHITESPACE);
			/*Add string from [Whitespace][Letter] to the end*/
			strcpy(cutStr + (strlen(TRIM_WHITESPACE)), string + (i + 1));
			/*Stitch the string and the cutStr together*/
			strcpy(string + (lastLetter + 1), cutStr);
			/*Turn i to after last word formatted index*/
			i = lastLetter + 1;
			/*lastLetter = -1;*/
		}
	}
}

/*Trim the front and the end of a given string from whitespace and \t*/
void trimFrontEnd(char string[]) {

	char p[MAX_LINE];
	int i, j, sLen;
	/* Get length of unformatted string*/
	sLen = strlen(string);
	/* Cut front part of string, run from start*/
	for (i = 0; i < sLen; i++) {
		if (!isspace(string[i])) {

			strcpy(p, string + i);
			strcpy(string, p);
			break;
		}
	}
	/*Get new length of string*/
	sLen = strlen(string);
	/*Cut end part of string, start from the end*/
	for (j = sLen - 1; j >= 0; j--) {
		if (!isspace(string[j])) {
			string[j + 1] = '\0';
			break;
		}
	}
}
