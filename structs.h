#ifndef STRUCTS_H_
#define STRUCTS_H_

#endif /* STRUCTS_H_ */

/*Struct of error type. used to build an error list*/
typedef struct error {
	/*Error string*/
	char * str_error;
	/*Line of error*/
	int line;
	/*String of where error was found*/
	char * str;
	/*Pointer to next*/
	struct error * next;
} error;

/*Struct of opcode type
 used internally in operations.c to loop through all commands*/
typedef struct opcode {
	/*Operation String*/
	char * str;
	/*Opcode number*/
	int code;
	/*Group of legal operands*/
	int group;
} opcode;

/*Struct of data type
 contains an address and a value (which stores the number/char)*/
typedef struct data {
	/*address of data word*/
	int address;
	/*Value*/
	short int value;
	/*pointer to next*/
	struct data * next;
} data;

/*Struct of symbol type
 contains a string (label),
 address, and external/operation flags*/
typedef struct symbol {
	/*Label of the symbol*/
	char * label;
	/*Address of the symbol*/
	int address;
	/*Is external*/
	int external;
	/*Is operation*/
	int operation;
	/*Group of operands*/
	int group;
	/*pointer to next*/
	struct symbol * next;
} symbol;

/*Struct of entry type
 Contains a label, an address, an address flag, a line value,
 and a pointer to the next node*/
typedef struct entry {
	/*Label of entry*/
	char * label;
	/*Address*/
	short int address;
	/*Does it have an address? (for internal use) TODO: might delete property*/
	int hasAddress;
	/*Line of entry*/
	int line;
	/*pointer to next*/
	struct entry * next;
} entry;

/*Struct of first word type*/
typedef struct firstWord {

	/*ERA bits*/
	unsigned int era :2;
	/*Destination operand bits*/
	unsigned int destination :2;
	/*Source operand bits*/
	unsigned int source :2;
	/*Opcode bits*/
	unsigned int opcode :4;
	/*Parameter2 bits always 0 unless jump word*/
	unsigned int parameterTwo :2;
	/*Parameter1 bits always 0 unless jump word*/
	unsigned int parameterOne :2;

} firstWord;

/*Struct of immediate word type:
 immediate type of word type, contains 12 bit value of a number*/
typedef struct immediateWord {
	/*ERA bits*/
	unsigned int era :2;
	/*bits used to hold the value/address*/
	int number :12;
} immediateWord;

/*Struct of direct word type:
 * direct type will hold a symbol in memory, represented in 12 bit memory */
typedef struct directWord {
	/*ERA bits*/
	unsigned era :2;
	/*memory bits*/
	unsigned int :12;
} directWord;

/*Struct of register word type*/
typedef struct registerWord {
	/*ERA bits*/
	unsigned int era :2;
	/*Destination register bits*/
	unsigned int destination :6;
	/*source register bits*/
	unsigned int source :6;
} registerWord;

/*Word holder union, used to hold all of the words.
 a union is used so that every type of word can be in a linked list together
 Note - JUMP command is made of a label and params no need for JUMP struct since immediate/reg/direct will be used*/
typedef union wordHolder {
	/*firsWord type*/
	firstWord first;
	/*Immediate word type*/
	immediateWord immediate;
	/*Register word type*/
	registerWord reg;
	/*Direct word type*/
	directWord direct;
} wordHolder;

/*wordNode is the type used in the word list. it contains a holder union type,
 an address which is used when exporting files,
 the method identifier which tells which type of word it holds,
 the line is is at,
 missing label string (if it refers to a label),
 external label string (if it refers to an external variable),
 an external flag, and a pointer to the next node */
typedef struct wordNode {
	/*The word itself*/
	wordHolder holder;
	/*The address assigned to it*/
	int address;
	/*Addressing method - which word is it?*/
	int method;
	/*Which line in input?*/
	int line;
	/*Missing label*/
	char * missingLabel;
	/*Label of external*/
	char * externalLabel;
	/*Word group type*/
	unsigned short int group;
	/*Is it external?*/
	int isExternal;
	/*Pointer to next*/
	struct wordNode * next;
} wordNode;

