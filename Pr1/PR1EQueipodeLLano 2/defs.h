#ifndef DEFS_H
#define DEFS_H

#define MAX_CHARS_NIF  9
#define NR_FIELDS_STUDENT 4

/**
 * Structure that describes a student
 *
 */

typedef struct {
    int student_id; 
    char NIF[MAX_CHARS_NIF+1];  
    char* first_name;
    char* last_name;
} student_t;


/**
 * Index of the various fields in student_t
 * Helper data type to simplify the parser's implementation.
 */
typedef enum {
	STUDENT_ID_IDX=0,
	NIF_IDX,
	FIRST_NAME_IDX,
	SECOND_NAME_IDX,
} token_id_t;


/**
 * Structure to hold the "variables" associated with
 * command-line options
 */
struct options {
	FILE* file;
	char* filename;
	char* records[];
};

#endif