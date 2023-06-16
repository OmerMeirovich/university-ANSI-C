#ifndef DEFINES_H_
#define DEFINES_H_

/* Boolean */
enum boolean {
	FALSE, TRUE
};

/*Directive enum -> first pass verification for checking directive type.
 NONE means no directive given and ERROR means that the string was recognized as a directive but the syntax was wrong*/
enum directive {
	DIRECTIVE_NONE,
	DIRECTIVE_DATA,
	DIRECTIVE_STRING,
	DIRECTIVE_ENTRY,
	DIRECTIVE_EXTERN,
	DIRECTIVE_ERROR
};

/*Addressing method enum - NONE means first word*/
enum addresing_method {
	NONE = -1, IMMEDIATE = 0, DIRECT = 1, JUMP = 2, REGISTER = 3
};

/*Directive string constants, to be compared with input*/
#define STR_DIRECTIVE_DATA ".data"
#define STR_DIRECTIVE_STRING ".string"
#define STR_DIRECTIVE_ENTRY ".entry"
#define STR_DIRECTIVE_EXTERN ".extern"

/*Extrnsions for file i/o*/
#define AS_EXTENSION ".as"
#define TXT_EXTENSION ".txt"
#define OB_EXTENSION ".ob"
#define ENT_EXTENSION ".ent"
#define EXT_EXTENSION ".ext"

/*Maximum line buffer size*/
#define MAX_LINE 80

/*Trim whitespace */
#define TRIM_WHITESPACE " "

/*Maximum filename size*/
#define MAX_FILENAME 50

/*Number of registers*/
#define NUM_OF_REGISTERS 8

/*Opcode amount*/
#define NUM_OF_OPCODES 16

/*Maximum errors in file*/
#define MAX_ERRORS 1000

/* Maximum letters allowed in a label*/
#define LABEL_LEN 31

/*Minimum  value that a data node can have*/
#define MIN_NUMBER_DATA 8191
/*Maximum  value that a data node can have*/
#define MAX_NUMBER_DATA -8192

/*Maximum  value of an immediate number*/
#define MAX_NUM_IMMEDIATE 4069
/*Minimum  value of an immediate number*/
#define MIN_NUMBER_IMMEDIATE -4069

/* Opcodes*/
#define OPCODE_MOV 0
#define OPCODE_CMP 1
#define OPCODE_ADD 2
#define OPCODE_SUB 3
#define OPCODE_NOT 4
#define OPCODE_CLR 5
#define OPCODE_LEA 6
#define OPCODE_INC 7
#define OPCODE_DEC 8
#define OPCODE_JMP 9
#define OPCODE_BNE 10
#define OPCODE_RED 11
#define OPCODE_PRN 12
#define OPCODE_JSR 13
#define OPCODE_RTS 14
#define OPCODE_STOP 15

/*Error defines*/
#define ILLEGAL_LABEL "^"
#define RESERVED_LABEL "@"

/*STRTOK delimeters to skip*/
#define STRTOK_DELIMS " ,\t\n()"

#endif /* DEFINES_H_ */
