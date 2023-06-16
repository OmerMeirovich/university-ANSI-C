#include "header.h"
/*This file contains the second pass function*/

void secondPass(int IC) {
	/*Temporary word node*/
	wordNode * tmpWordNode = NULL;
	/*Temporary entry node*/
	entry * tmpEntry = NULL;
	/*Temporary symbol node*/
	symbol * tmpSymbol = NULL;
	/*If the overall count of words and data exceeds the memory limit, add error and quit*/
	if ((countWords() + countData()) > 1000) {
		addError("Exceeded memory limit", -1, NULL);
		return;
	}
	/*Loop that gets every label which is missing an address*/
	while ((tmpWordNode = getMissingLabel())) {
		/*Search for a symbol in the symbols list with the same address*/
		tmpSymbol = searchSymbol(tmpWordNode->missingLabel);
		/*If no such symbol was found, add an error*/
		if (!tmpSymbol) {
			char *tmpString;
			tmpString = malloc(sizeof(char) * MAX_LINE);
			strcpy(tmpString, "Label \"");
			strcat(tmpString, tmpWordNode->missingLabel);
			strcat(tmpString, "\" was not declared");
			addError(tmpString, tmpWordNode->line, NULL);
		}
		/*If the symbol was found*/
		else {
			/*If it's external, change the word accordingly*/
			if (tmpSymbol->external) {
				tmpWordNode->holder.immediate.era = 1;
				tmpWordNode->holder.immediate.number = 0;
				tmpWordNode->isExternal = TRUE;
				tmpWordNode->externalLabel = tmpSymbol->label;
			}
			/*If it's not external, change the word accordingly and assign the address*/
			else {
				tmpWordNode->holder.immediate.era = 2;
				tmpWordNode->holder.immediate.number = tmpSymbol->address;
			}
		}
		tmpWordNode->missingLabel = NULL;
	}
	/*Go through the entry list*/
	while ((tmpEntry = getEntry())) {
		/*Search for symbols with the same label*/
		tmpSymbol = searchSymbol(tmpEntry->label);
		/*If there is no such symbol, add an error*/
		if (!tmpSymbol) {
			char *tmpString;
			tmpString = malloc(sizeof(char) * MAX_LINE);
			strcpy(tmpString, "Label \"");
			strcat(tmpString, tmpEntry->label);
			strcat(tmpString, "\" was not declared");
			addError(tmpString, tmpEntry->line, NULL);
			tmpEntry->hasAddress = TRUE;
		}
		/*If a symbol was found, assign its address to the entry node*/
		else {
			tmpEntry->address = tmpSymbol->address;
			tmpEntry->hasAddress = TRUE;
		}
	}
}
