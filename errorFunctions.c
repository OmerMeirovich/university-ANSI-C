#include "header.h"

/*All of the error handling functions*/

/*static pointer to head*/
static error * head = NULL;

/*This function adds an error to the errors list.
 HOW TO USE:
 addError(err, line, str) ->
 err - String that describes the error.
 line - Line number, Negative number equals 'no line'.
 str - adds an error string (primarily for syntax errors)
 */
void addError(char * errStr, int line, char * str) {
	/*Create a new error object*/
	error * tmp = malloc(sizeof(error));
	checkAllocation(tmp);

	/*Put error in object*/
	tmp->str_error = errStr;
	/*Put error line in object*/
	tmp->line = line;
	/*str was given*/
	if (str) {
		/*Check if last char is newline, if so, remove it*/
		if (str[strlen(str) - 1] == '\n') {
			str[strlen(str) - 1] = 0;
		}
		/*Malloc str of object*/
		tmp->str = malloc(sizeof(char) * MAX_LINE);
		checkAllocation(tmp->str);
		strcpy(tmp->str, str);
	}
	/*In case str was not given*/
	else {
		/*Set str to null*/
		tmp->str = NULL;
	}
	/*Set next to null*/
	tmp->next = NULL;
	if (!head) {
		/*If head is empty point head to tmp*/
		head = tmp;
	}
	/*If head is not null*/
	else {
		error * p = head;
		while (p->next)
			p = p->next;
		/*Point last error to new error*/
		p->next = tmp;
	}
	/*Becuase malloc was not free'd we are able to malloc every time and create a chain of errors*/
}

/*Check if there are no errors, If head is null*/
int isErrorEmpty() {
	if (head)
		return TRUE;
	else
		return FALSE;
}

/*This function returns the last line an error appears in.
 used in the error printing loop*/
int getMaxLine() {
	error * p;
	int maxLine = 1;
	p = head;
	if (!p)
		return 0;
	while (p) {
		if (p->line > maxLine)
			maxLine = p->line;
		p = p->next;
	}
	return maxLine;
}

/*This function will write the errors to console with STDERR and print it into an error file, The reason for printing to a file as well
 * Is for a convinent error checking in case the proggramer intends to check the errors using a file*/
void printErrors(char * filename) {
	/*Error file*/
	FILE * tmp = NULL;
	error * ptr = head;
	int maxLine = getMaxLine();
	int index = 1;
	/*Create filename for this input file*/
	char error_filename[MAX_FILENAME];
	strcpy(error_filename, "errors-");
	strcat(error_filename, filename);
	/*Pointer to go through all the errors*/

	/*Open the file and check if succeeded*/
	if ((tmp = fopen(error_filename, "w"))) {
		if (!ptr) {
			fclose(tmp);
			return;
		}
		/*Write the errors*/
		while (index <= maxLine) {
			ptr = head;
			while (ptr) {
				if (ptr->line == index) {
					if (ptr->str) {
						/*Print error into a file , used for internal failure checks file comperison*/
						fprintf(tmp, "ERROR, LINE %d:\t %s \"%s\".\n",
						 ptr->line, ptr->str_error, ptr->str);
						/*Print error to console with stderr*/
						fprintf(stderr, " ERROR, LINE %d:\t %s \"%s\".\n",
								ptr->line, ptr->str_error, ptr->str);
					} else {
						/*Print error into a file , used for internal failure checks file comperison*/
						fprintf(tmp, "ERROR, LINE %d:\t %s.\n", ptr->line,
						 ptr->str_error);
						/*Print error to console with stderr*/
						fprintf(stderr, "ERROR, LINE %d:\t %s.\n", ptr->line,
								ptr->str_error);
					}
				}
				ptr = ptr->next;
			}
			index++;
		}
		ptr = head;
		while (ptr) {
			if (ptr->line < 0) {
				if (ptr->str) {
					/*Print error into a file , used for internal failure checks file comperison*/
					fprintf(stderr, "%s: \"%s\"\n.", ptr->str_error, ptr->str);
				} else {
					/*Print error to console with stderr*/
					fprintf(stderr, "%s.\n", ptr->str_error);
				}
			}
			ptr = ptr->next;
		}
		printf(
				"Operation failed. Errors were printed to console AND exported to %s.txt\n",
				error_filename);
	}
	fclose(tmp);

}

/*Clear all error objects and error->str strings*/
void clearErrors() {
	error * ptr = head;
	if (!ptr)
		return;

	while (ptr) {
		error * tmp = ptr;
		ptr = ptr->next;
		free(tmp->str);
		free(tmp);
	}
	head = NULL;
}

void checkAllocation(void * ptr) {
	if (!ptr) {
		printf("Memory allocation failed.");
		exit(0);
	}
}
