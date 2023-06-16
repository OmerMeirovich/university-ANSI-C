#include "header.h"

/*Array of register string, for comparing with input*/
static char * registers[8] = { "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7" };

/*Array holding information about each opcode, for internal use
 * First parameter is the name of the operation
 * Second parameter is it's place on the list 0-15 (16 operations)
 * Third paramter the group of the opcode, used to know llegal operands.
 */
static opcode opCodes[16] = { { "mov", 0, 2 }, { "cmp", 1, 2 }, { "add", 2, 2 },
		{ "sub", 3, 2 }, { "lea", 6, 2 }, { "not", 4, 1 }, { "clr", 5, 1 }, {
				"inc", 7, 1 }, { "dec", 8, 1 }, { "jmp", 9, 1 },
		{ "bne", 10, 1 }, { "red", 11, 1 }, { "prn", 12, 1 }, { "jsr", 13, 1 },
		{ "rts", 14, 0 }, { "stop", 15, 0 } };

/* Get a token and analyze the input, Check the command, the group and operands and validate
 * that input given fits the commands operand required etc.. If it does add it to the word list
 * else add an error and do nothing. */
void evaluateOperation(char * token, int line, int *IC, char* label,
		char wholeLine[]) {
	/*index to check opCodes*/
	int i;
	/*opcode found flag*/
	int opFound = FALSE;
	/*How many commands did we have so we can increment IC*/
	int increment_IC;
	/*Does the line has brackets in it? used for internal validation*/
	int brackets;
	/*WordNode to add*/
	wordNode *firstWord = NULL;
	wordNode *secondWord = NULL;
	wordNode *thirdWord = NULL;
	/*Fourth word needed only if jump word with mixed parameters*/
	wordNode *fourthWord = NULL;

	/*Get number of brackets for the line*/
	brackets = hasBrackets(wholeLine);

	for (i = 0; i < NUM_OF_OPCODES; i++) {
		/*if it is, create a new word object, initiate it and assign it with everything*/
		if (!strcmp(token, opCodes[i].str)) {
			/*Set "found command" flag to true*/
			opFound = TRUE;
			/*Allocate memory for the first word*/
			firstWord = malloc(sizeof(wordNode));
			checkAllocation(firstWord);
			/*Set all of first word's parameters*/
			firstWord->method = NONE;
			firstWord->isExternal = FALSE;
			firstWord->missingLabel = NULL;
			firstWord->externalLabel = NULL;
			firstWord->line = line;
			firstWord->group = opCodes[i].group;
			firstWord->holder.first.opcode = opCodes[i].code;
			firstWord->holder.first.source = 0;
			firstWord->holder.first.destination = 0;
			firstWord->holder.first.era = 0;
			firstWord->next = NULL;
			/*Add one to the increment value*/
			increment_IC++;
		}
	}
	/*If the command wasnt found, error*/
	if (!opFound) {
		addError("Unknown command", line, token);
		return;
	}
	/*
	 A command word exists.
	 If we have parameters after the command*/
	if ((token = strtok(NULL, STRTOK_DELIMS))) {
		/*If opcode group requires two parameters*/
		if (firstWord->group == 2) {
			/*If command has brackets the command is invalid, group 2 only uses commas, letters, and #*/
			if (brackets != 0) {
				addError("Brackets are no allowed in this command", line, NULL);
				return;
			}
			/*If first token is an immediate number*/
			if (*token == '#') {
				int num;
				char * ptr;
				char * errorptr;
				ptr = token;
				ptr++;
				num = strtol(ptr, &errorptr, 10);
				/*if command is LEA, addError*/
				if (firstWord->holder.first.opcode == OPCODE_LEA) {
					addError(
							"This command can't take an immediate number as a first operand",
							line, NULL);
					free(firstWord);
					return;
				}
				/*if number conversion failes*/
				if (*errorptr != 0) {
					addError("Invalid number", line, NULL);
					free(firstWord);
					return;
				}
				/*if the number isn't within range*/
				if (num > MAX_NUM_IMMEDIATE || num < MIN_NUMBER_IMMEDIATE) {
					addError("Immediate number is out of range", line, NULL);
					free(firstWord);
					return;
				}
				/*If syntax is correct*/
				else {
					/*Update first word and create a second word*/
					firstWord->holder.first.source = IMMEDIATE;
					secondWord = malloc(sizeof(wordNode));
					checkAllocation(secondWord);
					secondWord->method = IMMEDIATE;
					secondWord->isExternal = FALSE;
					secondWord->line = line;
					secondWord->missingLabel = NULL;
					secondWord->externalLabel = NULL;
					secondWord->holder.immediate.number = num;
					secondWord->holder.immediate.era = 0;
					secondWord->next = NULL;
					increment_IC++;
				}
			}
			/*if first token is register*/
			else if (isRegister(token)) {
				/*If the command is lea, add error and return*/
				if (firstWord->holder.first.opcode == OPCODE_LEA) {
					addError(
							"This command can't take a register as a first operand",
							line, NULL);
					free(firstWord);
					return;
				}
				/*Update first word and create a second word*/
				firstWord->holder.first.source = REGISTER;
				secondWord = malloc(sizeof(wordNode));
				checkAllocation(secondWord);
				secondWord->method = REGISTER;
				secondWord->isExternal = FALSE;
				secondWord->line = line;
				secondWord->missingLabel = NULL;
				secondWord->externalLabel = NULL;
				secondWord->holder.reg.source = (token[1] - '0');
				secondWord->holder.reg.destination = 0;
				secondWord->holder.reg.era = 0;
				secondWord->next = NULL;
				increment_IC++;
			}
			/*Token is not a register nor immediate number, must be a label (direct)*/
			else {
				char * ptr;
				ptr = token;
				/*check if first char is letter (valid label)*/
				if (isalpha(*ptr)) {
					/*checks for invalid chars*/
					while (*ptr != '\0') {
						if ((!isdigit(*ptr)) && (!isalpha(*ptr))) {
							addError("Invalid character", line, token);
							free(firstWord);
							return;
						}
						ptr++;
					}
					/*create memory address opcode*/
					firstWord->holder.first.source = DIRECT;
					secondWord = malloc(sizeof(wordNode));
					checkAllocation(secondWord);
					secondWord->method = DIRECT;
					secondWord->isExternal = FALSE;
					secondWord->line = line;
					secondWord->missingLabel = malloc(sizeof(char) * MAX_LINE);
					secondWord->externalLabel = NULL;
					strcpy(secondWord->missingLabel, token);
					secondWord->holder.immediate.number = 0;
					secondWord->holder.immediate.era = 1;
					secondWord->next = NULL;
					increment_IC++;
				} else {
					addError("Labels must start with a letter", line, token);
					free(firstWord);
					return;
				}
			}
			/*Move to second token*/
			if ((token = strtok(NULL, STRTOK_DELIMS))) {
				/*Only CMP command is allowed to have a direct number as destination operand*/
				if (*token
						== '#'&& firstWord->holder.first.opcode == OPCODE_CMP) {int num;
				char * ptr;
				char * errorptr;
				ptr = token;
				ptr++;
				num = strtol(ptr, &errorptr, 10);
				/*if number conversion failes*/
				if (*errorptr != 0) {
					addError("Invalid number", line, NULL);
					free(firstWord);
					free(secondWord);
					return;
				}
				/*if the number isn't within range*/
				if (num > MAX_NUM_IMMEDIATE || num < MIN_NUMBER_IMMEDIATE) {
					addError("Immediate number is out of range", line,
							NULL);
					free(firstWord);
					return;
				}
				/*If syntax is correct*/
				else {
					firstWord->holder.first.destination = IMMEDIATE;
					thirdWord = malloc(sizeof(wordNode));
					checkAllocation(thirdWord);
					thirdWord->method = IMMEDIATE;
					thirdWord->isExternal = FALSE;
					thirdWord->line = line;
					thirdWord->missingLabel = NULL;
					thirdWord->externalLabel = NULL;
					thirdWord->holder.immediate.number = num;
					thirdWord->holder.immediate.era = 0;
					thirdWord->next = NULL;
					increment_IC++;
				}
			} else if (isRegister(token)) {
				if (secondWord->method == REGISTER) {
					firstWord->holder.first.destination = REGISTER;
					secondWord->holder.reg.destination = (token[1] - '0');
				} else {
					firstWord->holder.first.destination = REGISTER;
					thirdWord = malloc(sizeof(wordNode));
					checkAllocation(thirdWord);
					thirdWord->method = REGISTER;
					thirdWord->isExternal = FALSE;
					thirdWord->line = line;
					thirdWord->missingLabel = NULL;
					thirdWord->externalLabel = NULL;
					thirdWord->holder.reg.source = 0;
					thirdWord->holder.reg.destination = (token[1] - '0');
					thirdWord->holder.reg.era = 0;
					thirdWord->next = NULL;
					increment_IC++;
				}
			}
			/*if token is not immediate/index/register*/
			else {
				char * ptr;
				ptr = token;
				/*check if first char is letter (valid label)*/
				if (isalpha(*ptr)) {
					/*checks for invalid chars*/
					while (*ptr != '\0') {
						if ((!isdigit(*ptr)) && (!isalpha(*ptr))) {
							addError("Invalid character", line, token);
							free(firstWord);
							return;
						}
						ptr++;
					}
					/*create memory address opcode*/
					firstWord->holder.first.destination = DIRECT;
					thirdWord = malloc(sizeof(wordNode));
					checkAllocation(thirdWord);
					thirdWord->method = DIRECT;
					thirdWord->isExternal = FALSE;
					thirdWord->line = line;
					thirdWord->missingLabel = malloc(
							sizeof(char) * MAX_LINE);
					thirdWord->externalLabel = NULL;
					strcpy(thirdWord->missingLabel, token);
					thirdWord->holder.immediate.number = 0;
					thirdWord->holder.immediate.era = 1;
					thirdWord->next = NULL;
					increment_IC++;
				}
				/*if the label didn't start with a letter*/
				else {
					addError("Labels must start with a letter", line,
							token);
					free(firstWord);
					free(secondWord);
					return;
				}
			}
			/*check if the string has more arguments*/
			if ((token = strtok(NULL, STRTOK_DELIMS))) {
				addError("Too many arguments", line, NULL);
				free(firstWord);
				free(secondWord);
				free(thirdWord);
				return;
			}
		}
		/*Less than 2 arguments*/
		else {
			addError("This command must take 2 parameters", line, NULL);
			free(firstWord);
			free(secondWord);
			return;
		}
	}
	/*If opcode group require one parameter*/
	else if (firstWord->group == 1) {
		/*If opcode is bne,jmp and jsr check word with jumping parameters*/
		if (firstWord->holder.first.opcode == OPCODE_BNE
				|| firstWord->holder.first.opcode == OPCODE_JMP
				|| firstWord->holder.first.opcode == OPCODE_JSR) {

			/*One of the three JSR,BNE and JMP commands*/

			/*Command has no brackets so register or direct only*/
			if (brackets == 0) {
				/*If register*/
				if (isRegister(token)) {
					/*Update first word and create a second word*/
					firstWord->holder.first.source = REGISTER;
					secondWord = malloc(sizeof(wordNode));
					checkAllocation(secondWord);
					secondWord->method = REGISTER;
					secondWord->isExternal = FALSE;
					secondWord->line = line;
					secondWord->missingLabel = NULL;
					secondWord->externalLabel = NULL;
					secondWord->holder.reg.source = (token[1] - '0');
					secondWord->holder.reg.destination = 0;
					secondWord->holder.reg.era = 0;
					secondWord->next = NULL;
					increment_IC++;
				}
				/*If not a register must be direct*/
				else {/*ptr pointer to run on the token chars*/
					char * ptr;

					ptr = token;

					if (isalpha(*ptr)) {
						/*checks for invalid chars*/
						while (*ptr != '\0') {
							if ((!isdigit(*ptr)) && (!isalpha(*ptr))) {
								addError(
										"Label must not have any special chars",
										line, NULL);
								free(firstWord);
								return;
							}
							ptr++;
						}
					}
					/*Invalid first char in label operand in jump word*/
					else {
						addError("First char of label must be a letter", line,
								NULL);
						free(firstWord);
						return;
					}
					firstWord->holder.first.destination = DIRECT;
					secondWord = malloc(sizeof(wordNode));
					checkAllocation(secondWord);
					secondWord->method = DIRECT;
					secondWord->isExternal = FALSE;
					secondWord->line = line;
					secondWord->missingLabel = malloc(sizeof(char) * MAX_LINE);
					secondWord->externalLabel = NULL;
					strcpy(secondWord->missingLabel, token);
					secondWord->holder.immediate.number = 0;
					secondWord->holder.immediate.era = 1;
					secondWord->next = NULL;
					increment_IC++;
				}}
			/*Command has brackets so must be a jump word, 3 */
			else if (brackets == 2) {
				/*Use ptr to check if first token is a valid label*/
				char *ptr;
				ptr = token;
				/*First token is a label only, 2nd and 3rd tokens are one of the 3 (register,immediate,direct)*/
				/*check if first char is letter (valid label)*/
				if (isalpha(*ptr)) {
					/*checks for invalid chars*/
					while (*ptr != '\0') {
						if ((!isdigit(*ptr)) && (!isalpha(*ptr))) {
							addError("Invalid label", line, NULL);
							return;
						}
						ptr++;
					}
					/*Label is valid insert to second word*/
					firstWord->holder.first.destination = JUMP;
					secondWord = malloc(sizeof(wordNode));
					checkAllocation(secondWord);
					secondWord->method = DIRECT;
					secondWord->isExternal = FALSE;
					secondWord->line = line;
					secondWord->missingLabel = malloc(
							sizeof(char) * MAX_LINE);
					secondWord->externalLabel = NULL;
					strcpy(secondWord->missingLabel, token);
					secondWord->holder.immediate.number = 0;
					secondWord->holder.immediate.era = 1;
					secondWord->next = NULL;
					increment_IC++;
				} else {
					addError("Label must start with a letter", line, token);
					return;
				}
				/*Get the next token, the 1st parameter*/
				token = strtok(NULL, STRTOK_DELIMS);
				if (!(token)) {
					addError(
							"Two parameters inside a jump word is required, Only label was given",
							line, NULL);
					return;
				}
				/*Check the 2nd token, First parameter inside the brackets*/
				if (*token == '#') {
					int num;
					char * ptr;
					char * errorptr;
					ptr = token;
					ptr++;
					num = strtol(ptr, &errorptr, 10);
					/*If number conversion fails*/
					if (*errorptr != 0) {
						addError("Invalid number", line, NULL);
						free(firstWord);
						free(secondWord);
						return;
					}
					/*if the number isn't within range*/
					if (num > MAX_NUM_IMMEDIATE
							|| num < MIN_NUMBER_IMMEDIATE) {
						addError("Immediate number is out of range", line,
								NULL);
						free(firstWord);
						return;
					}
					/*If syntax is correct*/
					else {
						firstWord->holder.first.parameterOne = IMMEDIATE;
						thirdWord = malloc(sizeof(wordNode));
						checkAllocation(thirdWord);
						thirdWord->method = IMMEDIATE;
						thirdWord->isExternal = FALSE;
						thirdWord->line = line;
						thirdWord->missingLabel = NULL;
						thirdWord->externalLabel = NULL;
						thirdWord->holder.immediate.number = num;
						thirdWord->holder.immediate.era = 0;
						thirdWord->next = NULL;
						increment_IC++;
					}
				} else if (isRegister(token)) {
					firstWord->holder.first.parameterOne = REGISTER;
					thirdWord = malloc(sizeof(wordNode));
					checkAllocation(thirdWord);
					thirdWord->method = REGISTER;
					thirdWord->isExternal = FALSE;
					thirdWord->line = line;
					thirdWord->missingLabel = NULL;
					thirdWord->externalLabel = NULL;
					thirdWord->holder.reg.destination = 0;
					thirdWord->holder.reg.source = (token[1] - '0');
					thirdWord->holder.reg.era = 0;
					thirdWord->next = NULL;
					increment_IC++;
				}
				/*if token is not immediate/direct/register*/
				else {
					char * ptr;
					ptr = token;
					/*check if first char is letter (valid label)*/
					if (isalpha(*ptr)) {
						/*checks for invalid chars*/
						while (*ptr != '\0') {
							if ((!isdigit(*ptr)) && (!isalpha(*ptr))) {
								addError("Invalid character", line, token);
								free(firstWord);
								return;
							}
							ptr++;
						}
						/*create memory address opcode*/
						firstWord->holder.first.parameterOne = DIRECT;
						thirdWord = malloc(sizeof(wordNode));
						checkAllocation(thirdWord);
						thirdWord->method = DIRECT;
						thirdWord->isExternal = FALSE;
						thirdWord->line = line;
						thirdWord->missingLabel = malloc(
								sizeof(char) * MAX_LINE);
						thirdWord->externalLabel = NULL;
						strcpy(thirdWord->missingLabel, token);
						thirdWord->holder.immediate.number = 0;
						thirdWord->holder.immediate.era = 1;
						thirdWord->next = NULL;
						increment_IC++;
					}
					/*if the label didn't start with a letter*/
					else {
						addError("Labels must start with a letter", line,
								token);
						free(firstWord);
						free(secondWord);
						return;
					}
				}
				/*Move to 3rd token, the 2nd parameter inside the brackets*/
				token = strtok(NULL, STRTOK_DELIMS);
				/*No additional parameter was given*/
				if (!(token)) {
					addError(
							"Not enough parameters, Jump requires 2 parameters, only 1 was found",
							line, NULL);
					return;
				}
				if (*token == '#') {
					int num;
					char * ptr;
					char * errorptr;
					ptr = token;
					ptr++;
					num = strtol(ptr, &errorptr, 10);
					/*if number conversion failes*/
					if (*errorptr != 0) {
						addError("Invalid number", line, NULL);
						free(firstWord);
						free(secondWord);
						free(thirdWord);
						return;
					}
					/*if the number isn't within range*/
					if (num > MAX_NUM_IMMEDIATE
							|| num < MIN_NUMBER_IMMEDIATE) {
						addError("Immediate number is out of range", line,
								NULL);
						free(firstWord);
						return;
					}
					/*If syntax is correct*/
					else {
						firstWord->holder.first.parameterTwo = IMMEDIATE;
						fourthWord = malloc(sizeof(wordNode));
						checkAllocation(fourthWord);
						fourthWord->method = IMMEDIATE;
						fourthWord->isExternal = FALSE;
						fourthWord->line = line;
						fourthWord->missingLabel = NULL;
						fourthWord->externalLabel = NULL;
						fourthWord->holder.immediate.number = num;
						fourthWord->holder.immediate.era = 0;
						fourthWord->next = NULL;
						increment_IC++;
					}
				} else if (isRegister(token)) {
					if (thirdWord->method == REGISTER) {
						firstWord->holder.first.parameterTwo = REGISTER;
						thirdWord->holder.reg.destination =
						(token[1] - '0');
					} else {
						firstWord->holder.first.parameterTwo = REGISTER;
						fourthWord = malloc(sizeof(wordNode));
						checkAllocation(fourthWord);
						fourthWord->method = REGISTER;
						fourthWord->isExternal = FALSE;
						fourthWord->line = line;
						fourthWord->missingLabel = NULL;
						fourthWord->externalLabel = NULL;
						fourthWord->holder.reg.source = 0;
						fourthWord->holder.reg.destination =
						(token[1] - '0');
						fourthWord->holder.reg.era = 0;
						fourthWord->next = NULL;
						increment_IC++;
					}
				}
				/*Not a register\direct\immediate must be a label*/
				else {
					char * ptr;
					ptr = token;
					/*check if first char is letter (valid label)*/
					if (isalpha(*ptr)) {
						/*checks for invalid chars*/
						while (*ptr != '\0') {
							if ((!isdigit(*ptr)) && (!isalpha(*ptr))) {
								addError("Invalid character", line, token);
								free(firstWord);
								return;
							}
							ptr++;
						}
						/*create memory address opcode*/
						firstWord->holder.first.parameterTwo = DIRECT;
						fourthWord = malloc(sizeof(wordNode));
						checkAllocation(fourthWord);
						fourthWord->method = DIRECT;
						fourthWord->isExternal = FALSE;
						fourthWord->line = line;
						fourthWord->missingLabel = malloc(
								sizeof(char) * MAX_LINE);
						fourthWord->externalLabel = NULL;
						strcpy(fourthWord->missingLabel, token);
						fourthWord->holder.immediate.number = 0;
						fourthWord->holder.immediate.era = 1;
						fourthWord->next = NULL;
						increment_IC++;
					}
				}
			} else {
				addError(
						"Only one bracket was found, Two brackets are needed",
						line, NULL);
				return;
			}
			if ((token = strtok(NULL, STRTOK_DELIMS))) {
				addError("Too many arguments", line, NULL);
				free(firstWord);
				free(secondWord);
				free(thirdWord);
				free(fourthWord);
				return;
			}
		}
		/*Command is any one parameter opcode that is not a jump command (jsr,jmp,bne)*/
		else {
			/*Brackets are not allowed if its not a jump word and has only one parameter*/
			if (brackets != 0) {
				addError("Brackets are no allowed in this command", line,
						NULL);
				return;
			}
			/*Immediate number is only allowed for prn ocode*/
			if (*token
					== '#'&& firstWord->holder.first.opcode == OPCODE_PRN) {int num;
				char * ptr;
				char * errorptr;
				ptr = token;
				ptr++;
				num = strtol(ptr, &errorptr, 10);
				/*if number conversion failes*/
				if (*errorptr != 0) {
					addError("Invalid number", line, NULL);
					free(firstWord);
					return;
				}
				/*if the number isn't within range*/
				if (num > MAX_NUM_IMMEDIATE || num < MIN_NUMBER_IMMEDIATE) {
					addError("Immediate number is out of range", line,
							NULL);
					free(firstWord);
					return;
				}
				/*If syntax is correct*/
				else {
					/*Update first word and create a second word*/
					firstWord->holder.first.destination = IMMEDIATE;
					secondWord = malloc(sizeof(wordNode));
					checkAllocation(secondWord);
					secondWord->method = IMMEDIATE;
					secondWord->isExternal = FALSE;
					secondWord->line = line;
					secondWord->missingLabel = NULL;
					secondWord->externalLabel = NULL;
					secondWord->holder.immediate.number = num;
					secondWord->holder.immediate.era = 0;
					secondWord->next = NULL;
					increment_IC++;
				}
			}
			/*if first token is register*/
			else if (isRegister(token)) {
				/*Update first word and create a second word*/
				firstWord->holder.first.destination = REGISTER;
				secondWord = malloc(sizeof(wordNode));
				checkAllocation(secondWord);
				secondWord->method = REGISTER;
				secondWord->isExternal = FALSE;
				secondWord->line = line;
				secondWord->missingLabel = NULL;
				secondWord->externalLabel = NULL;
				secondWord->holder.reg.source = (token[1] - '0');
				secondWord->holder.reg.destination = 0;
				secondWord->holder.reg.era = 0;
				secondWord->next = NULL;
				increment_IC++;
			}
			/*Token is not a register nor immediate number, must be a label (direct)*/
			else {
				char * ptr;
				ptr = token;
				/*Check if first char is letter (valid label)*/
				if (isalpha(*ptr)) {
					/*Checks for invalid chars*/
					while (*ptr != '\0') {
						if ((!isdigit(*ptr)) && (!isalpha(*ptr))) {
							addError("Invalid character", line, token);
							free(firstWord);
							return;
						}
						ptr++;
					}
					/*create memory address opcode*/
					firstWord->holder.first.destination = DIRECT;
					secondWord = malloc(sizeof(wordNode));
					checkAllocation(secondWord);
					secondWord->method = DIRECT;
					secondWord->isExternal = FALSE;
					secondWord->line = line;
					secondWord->missingLabel = malloc(
							sizeof(char) * MAX_LINE);
					secondWord->externalLabel = NULL;
					strcpy(secondWord->missingLabel, token);
					secondWord->holder.immediate.number = 0;
					secondWord->holder.immediate.era = 1;
					secondWord->next = NULL;
					increment_IC++;
				} else {
					addError("Labels must start with a letter", line,
							token);
					free(firstWord);
					return;
				}
			}
			/*check if the string has more arguments*/
			if ((token = strtok(NULL, STRTOK_DELIMS))) {
				addError("Too many arguments", line, NULL);
				free(firstWord);
				free(secondWord);
				free(thirdWord);
				free(fourthWord);
				return;
			}
		}
	}
	/*If the command opcode is group 0*/
	else {
		addError("This command cannot take parameters", line, NULL);
		free(firstWord);
		return;
	}
}
/*If we didn't receive any parameters after the command and command isn't RTS or STOP*/
else {
	/* RTS and STOP command opcode*/
	if (firstWord->holder.first.opcode != OPCODE_RTS
			&& firstWord->holder.first.opcode != OPCODE_STOP) {
		addError("This command must take parameters", line, NULL);
		free(firstWord);
		return;
	}
}
				/*Check if there's a label with the command. if there is, add it to the symbols table*/
	if (label) {
		addSymbol(label, *IC, FALSE, TRUE, line);
	}
	/*Add each word (as needed) to the words list*/
	if (firstWord)
		addWordNode(firstWord);
	if (secondWord)
		addWordNode(secondWord);
	if (thirdWord)
		addWordNode(thirdWord);
	if (fourthWord)
		addWordNode(fourthWord);

	/*Increment the IC as needed*/
	*IC += increment_IC;
}

/*Check if a given string is an operation or not*/
int isOperation(char *token) {
	int i;

	for (i = 0; i < NUM_OF_OPCODES; i++) {
		if (!strcmp(token, opCodes[i].str))
			return 1;
	}

	return 0;
}

/*Check if a given sting is a register or not*/
int isRegister(char * token) {
	int i;
	if (strlen(token) == 2) {
		if (token[0] == 'r') {
			if (isdigit(token[1])) {
				for (i = 0; i < NUM_OF_REGISTERS; i++) {
					if (!strcmp(token, registers[i])) {
						return 1;
					}
				}

			}
		}
	}
	return 0;
}

/*This function determines if we received a directive.
 Enum int of a proper directive is returned, or an error flag
 if there's no such directive. if the string doesn't start with ".",
 it will return DIRECTIVE_NONE, meaning it's not a directive*/
int getDirective(char * token) {
	/*if first char is '.'*/
	if (*token == '.') {
		/*return enum corresponding to the directive*/
		if (!strcmp(token, STR_DIRECTIVE_DATA)) {
			return DIRECTIVE_DATA;
		} else if (!strcmp(token, STR_DIRECTIVE_STRING)) {
			return DIRECTIVE_STRING;
		} else if (!strcmp(token, STR_DIRECTIVE_ENTRY)) {
			return DIRECTIVE_ENTRY;
		} else if (!strcmp(token, STR_DIRECTIVE_EXTERN)) {
			return DIRECTIVE_EXTERN;
		}
		/*if string is unknown*/
		else {
			/*return error*/
			return DIRECTIVE_ERROR;
		}
	}
	/*if this isn't a directive*/
	else {
		return DIRECTIVE_NONE;
	}
}

/*Check if line starts with ';' if it does then its a comment line*/
int isComment(char* token) {
	if (token[0] == ';')
		return TRUE;
	return FALSE;
}

/*This function checks for a label.
 No label returns NULL.
 illegal label returns ILLEGAL_LABEL
 or if label is a reserved word a register or an operation returns RESERVED_LABEL.
 Legal label returns the label itself.*/
char * getLabel(char * token) {
	/*letCounter will check how many letters in a label */
	int letCounter = 0;
	char * ptr = token;

	/*if last char is ':'*/
	if (token[strlen(token) - 1] == ':') {
		/*If first char is valid*/
		if (isalpha(*token)) {
			/*Set null terminator instead of ':'*/
			token[strlen(token) - 1] = 0;
			/*If a char is not a letter or a number or if more than LABEL_LEN letters were given return illegal label*/
			while ((*ptr) != '\0') {
				if (letCounter > LABEL_LEN)
					return ILLEGAL_LABEL;
				/*If it's not a digit AND a letter*/
				if (!isalpha(*ptr)) {
					if (!isdigit(*ptr)) {
						return ILLEGAL_LABEL;
					}
				}
				ptr++;
				letCounter++;
			}
			/*If the token is an operation, return RESERVED_LABEL flag*/
			if (isOperation(token)) {
				return RESERVED_LABEL;
			}
			/*If the token is a register, return RESERVED_LABEL flag*/
			if (isRegister(token)) {
				return RESERVED_LABEL;
			}
			return token;
		}
		/*if first letter is invalid, return ILLEGAL_LABEL flag*/
		else {
			return ILLEGAL_LABEL;
		}
	}
	/*if last letter is not ':', return null*/
	else {
		return NULL;
	}
}

/*Check if input has brackets, return if command has 2 brackets, 1 bracket or 0 brackets, Internal validation func*/
int hasBrackets(char line[]) {
	/*left bracket and right bracket flags*/
	int left = 0, right = 0;
	int i;

	for (i = 0; i < strlen(line); i++) {
		if (line[i] == '(')
			left = 1;
		if (line[i] == ')')
			right = 1;
	}
	/*Two brackets were found*/
	if (left == 1 && right == 1)
		return 2;
	/*One bracket was found (this is an error)*/
	if (left == 1 || right == 1)
		return 1;
	/*No brackets were found*/
	else
		return 0;
}
