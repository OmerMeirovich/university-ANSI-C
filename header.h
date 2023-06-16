#ifndef HEADER_H_
#define HEADER_H_

/* include all functions of project*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "defines.h"
#include "structs.h"

/*main*/
void trimFrontEnd(char string[]);
void trim(char string[]);
/*firstpass*/
void firstPassFunc(char fileName[], int* IC, int* DC);
int CommaCheck(char string[]);
/*errorFunctions*/
void addError(char * errStr, int line, char * str);
int isErrorEmpty();
int getMaxLine();
void printErrors(char * filename);
void clearErrors();
void checkAllocation(void * ptr);
/*Operations.c*/
int isOperation(char *token);
int isRegister(char * token);
int getDirective(char * token);
int isComment(char* token);
char * getLabel(char * token);
void evaluateOperation(char * token, int line, int *IC, char* label,
		char wholeLine[]);
int hasBrackets(char line[]);
/*SymbolsList.c*/
void addSymbol(char * label, int address, int external, int operation, int line);
void checkSymbol(char * label, int line);
symbol * searchSymbol(char * label);
void clearSymbols();
void updateSymbols(int IC);
/*DataList.c*/
void splitNumbers(char * token, int line, int * DC);
void addNumber(char * number, int line, int * DC);
void addChar(char c);
void addString(char * string, int line, int *DC);
void updateDataAddresses(int IC);
void clearData();
data * getHeadData();
int countData();
/*EntryList.c*/
void addEntry(char * label, int line);
int checkEntry(char * label);
entry * getHeadEntry();
void clearEntries();
entry * getEntry();
/*WordsList.c*/
void addWordNode(wordNode * node);
void updateAddresses();
int countWords();
void clearWords();
wordNode * getMissingLabel();
wordNode * getHeadWord();
/*secondPass.c*/
void secondPass(int IC);
/*printFiles.c*/
void exportFiles(int IC, int DC, char * name);
FILE * openFile(char* filename, char * mode, char * extension);
void printWordsBinary(FILE * object, wordNode* p, unsigned short int toPrint);
void printDataBinary(FILE *object, int address, short int toPrint);
void reverseStr(char *s);

#endif /* HEADER_H_ */
