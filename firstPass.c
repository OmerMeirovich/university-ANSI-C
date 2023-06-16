#include "header.h"

/*Get file name and open the file and run through each line
 * Each line check for Label -> Directive.
 * If no directive was found search for an actual command*/
void firstPassFunc(char fileName[], int* IC, int* DC) {

	FILE *fp;
	char line[MAX_LINE], *token, lineCounter, *label, lineCopy[MAX_LINE];
	int directive;

	lineCounter = 0;
	fp = fopen(fileName, "r");

	if (!fp) {
		fprintf(stderr, "%s", "error: opening file from path failed!\n");
	} else {
		/*Run each line from the file*/
		while (fgets(line, MAX_LINE, fp)) {
			/*Increment line counter*/
			lineCounter++;
			/*Trim the line given*/
			trim(line);
			/*Pointer to token*/
			token = NULL;
			/*Pointer to label*/
			label = NULL;
			/*Directive flag*/
			directive = 0;
			/*Save a copy of the line for validation in operations.c*/
			strcpy(lineCopy, line);
			/*Get first token*/
			token = strtok(line, STRTOK_DELIMS);
			/*If line is empty, go to next line*/
			if (!token) {
				continue;
			}
			/*If line is comment, go to next line*/
			if (isComment(token) == TRUE) {
				continue;
			}
			/*Check two consecutive commas / comma at the end etc...*/
			if (CommaCheck(line) == FALSE) {
				addError("Two consecutive commas/Comma at the end was found",
						lineCounter, NULL);
				continue;
			}
			if ((label = getLabel(token))) {
				/*If getLabel returned error string*/
				if (!strcmp(label, ILLEGAL_LABEL)) {
					addError("Illegal label", lineCounter, token);
					/*set label back to null*/
					label = NULL;
				} else if (!strcmp(label, RESERVED_LABEL)) {
					addError("Label name is reserved", lineCounter, NULL);
				}
				/*Token was a valid label, Get next token.*/
				token = strtok(NULL, STRTOK_DELIMS);
				/*If next token is null, add error and go to next line*/
				if (!token) {
					addError(" Label has no extra arguments", lineCounter,
							NULL);
					continue;
				}
			}
			/*Label was checked, Now check if token is directive Directive*/
			if ((directive = getDirective(token))) {

				/*if getDirective returned an error*/
				if (directive == DIRECTIVE_ERROR) {
					/*Add error*/
					addError("Invalid directive", lineCounter, token);
					continue;
				}
				/*If directive is data directive*/
				else if (directive == DIRECTIVE_DATA) {
					/*If there's a label*/
					if (label) {
						addSymbol(label, *DC, FALSE, FALSE, lineCounter);
						splitNumbers(token, lineCounter, DC);
					}
					/*If there isn't a label*/
					else {
						splitNumbers(token, lineCounter, DC);
					}

				}
				/*If directive is string directive*/
				else if (directive == DIRECTIVE_STRING) {
					/*Get next token*/
					token = strtok(NULL, "\n");
					/*If token is null*/
					if (!token) {
						addError("Must specify a string", lineCounter, NULL);
						continue;
					}
					/*If there's a label*/
					if (label) {
						/*Add to symbols list*/
						addSymbol(label, *DC, FALSE, FALSE, lineCounter);
						/*Add to date*/
						addString(token, lineCounter, DC);
						/*Go to next line*/
						continue;
					}
					/*If there is no label*/
					else {
						/*Add to data*/
						addString(token, lineCounter, DC);
						/*Go to next line*/
						continue;
					}
				}
				/*If directive is entry*/
				else if (directive == DIRECTIVE_ENTRY) {
					/*If we have a label, print a warning*/
					if (label)
						printf(
								"Label before entry directive is redundant, In line %d Label: %s\n",
								lineCounter, label);
					/*Get next token*/
					if (!(token = strtok(NULL, " \t\n"))) {
						addError("Entry directive must have a label parameter",
								lineCounter, NULL);
						continue;
					}
					/*Sdd an entry node*/
					addEntry(token, lineCounter);
					/*If there's a next token*/
					if ((token = strtok(NULL, "\n"))) {
						addError("Entry directive can only take one parameter",
								lineCounter, NULL);
						continue;
					}
					/*If next token is null*/
					else {
						/*Go to next line*/
						continue;
					}
				}
				/*If directive is extern*/
				else if (directive == DIRECTIVE_EXTERN) {
					/*If there's a label, print a warning*/
					if (label)
						printf(
								"Label before entry directive is redundant, In line %d Label: %s\n",
								lineCounter, label);
					/*Get next token*/
					if (!(token = strtok(NULL, " \t\n"))) {
						addError("Extern directive must have a label parameter",
								lineCounter, NULL);
						continue;
					}
					/*Add extern symbol*/
					addSymbol(token, 0, TRUE, FALSE, lineCounter);
					/*If next token is not null*/
					if ((token = strtok(NULL, "\n"))) {
						addError("Extern directive can only take one parameter",
								lineCounter, NULL);
						continue;
					}
					/*If next token is null*/
					else {
						/*go to next line*/
						continue;
					}
				}
			} else {
				/*In case command is not a directive check for valid words, analyze command and act accordingly*/
				evaluateOperation(token, lineCounter, IC, label, lineCopy);
			}
		}
	}
	/*If while loop didn't run once file was empty.*/
	if (lineCounter == 0)
		addError("ERROR: File is empty.", -1, NULL);
	/*If file pointer is not null and firstpass was finished close file pointer*/
	if (fp) {
		fclose(fp);
	}
}
/*Check if there are two consecutive commas or a comma at the end
 * Function will help us determine if any errors will be found in the .data commands
 * or any other commands with commas in them*/
int CommaCheck(char string[]) {
	int sLen, i;

	sLen = strlen(string);
	/*Last char in line is , is not allowed*/
	if (string[sLen - 1] == ',') {
		return FALSE;
	}
	/*Check if there are two consecutive commas*/
	for (i = 0; string[i + 1] != '\0' || i < sLen; i++) {
		/*Two consecutive commas were found*/
		if (string[i] == ',' && string[i + 1] == ',') {
			return FALSE;
		}
	}
	return TRUE;
}

