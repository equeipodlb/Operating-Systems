#include <stdio.h>
#include <unistd.h> /* for getopt() */
#include <stdlib.h> /* for EXIT_SUCCESS, EXIT_FAILURE */
#include <string.h>
#include "defs.h"

student_t* read_student_file(FILE* file, int* nr_entries);
int findID(int id, student_t* entries, int nr_entries);

/**
 * @brief  Make a copy of existing string allocating memory accordingly
 *
 * @param original
 * @return new string that is a clone of original
 **/
static inline char* clone_string(char* original)
{
	char* copy;
	copy=malloc(strlen(original)+1);
	strcpy(copy,original);
	return copy;
}


/**
 * @brief Receive records and number of them and return the binary representation of them
 *
 * @param records[]
 * @param nr_entries
 * @return array of entries (student_t*) and entry count (nr_entries parameter)
 */
student_t* parse_records(char* records[], int* nr_entries)
{
	student_t* entries;
	char* token;
	token_id_t token_id;
	int entry_idx;
	char* cur_record;


	entries=malloc(sizeof(student_t)* *nr_entries);
	/* zero fill the array of structures */
	memset(entries,0,sizeof(student_t)* *nr_entries);

	/* Parse file */
	entry_idx=0;
	int i;
	for (i=0; i< *nr_entries; ++i){
		student_t* cur_entry = &entries[i];
		cur_record = records[i];
		token_id=0;
		while((token = strsep(&cur_record, ":"))!=NULL) {
			switch(token_id) {
			case STUDENT_ID_IDX:
				if (sscanf(token,"%d",&cur_entry->student_id)!=1) {
					fprintf(stderr, "Couldn't parse ID field. token is %s\n", token);
					return NULL;
				}
				break;
			case NIF_IDX:
				strcpy(cur_entry->NIF,token);
				break;
			case FIRST_NAME_IDX:
				cur_entry->first_name=clone_string(token);
				break;
			case SECOND_NAME_IDX:
				cur_entry->last_name=clone_string(token);
				break;
			default:
				break;
			}
			token_id++;
		}
		if (token_id!=NR_FIELDS_STUDENT) {
			fprintf(stderr, "Could not process all tokens from line %d of /etc/passwd\n",entry_idx+1);
			return NULL;
		}

		entry_idx++;
	}
	return entries;
}

int dump_entries(student_t* entries, int nr_entries, FILE* studentsFILE){
	int i;
	//Check for duplicates before dumping
	if (!entries)
		return EXIT_FAILURE;

	for (i=0; i<nr_entries; i++) {
		student_t* e=&entries[i]; /* Point to current entry */
		fwrite(&e->student_id, sizeof(int), 1, studentsFILE);
		fwrite(e->NIF,sizeof(char), MAX_CHARS_NIF + 1,studentsFILE);
		fwrite(e->first_name,sizeof(char), strlen(e->first_name) + 1,studentsFILE);
		fwrite(e->last_name,sizeof(char), strlen(e->last_name) + 1, studentsFILE);
		fprintf(stdout,"Dumped %s %s\n",e->first_name,e->last_name);
	}
	return EXIT_SUCCESS;
}

int checkDuplicates(student_t* nuevos,int cuantos,student_t* antiguos,int numAntiguos){
	int i;
	for (i = 0; i < cuantos; ++i){
		student_t* e = &nuevos[i]; // Check for duplicates in O(N^2)
		if (findID(e->student_id,antiguos,numAntiguos) >= 0){ // If the index returned is valid 
			fprintf(stdout,"Found duplicate with id %d\n", e->student_id);
			return EXIT_FAILURE;
		}
	}
	return 0;
}

// Funcion que lee un string de un fichero hasta el caracter \0
char* loadstr(FILE* file){
	int count;
	count = 0;
	char* aux;
	char* str;
	aux = malloc(sizeof(char));
	while (fread(aux,sizeof(char), 1,file) && *aux != '\0')
		count++;

	fseek(file,-count - 1,SEEK_CUR);

	str=malloc(sizeof(char)*(count+1));
	fread(str,sizeof(char),count + 1, file);

	return str;
}

student_t* read_student_file(FILE* file, int* nr_entries){
	// Primero leemos el numero de entries
	fread(nr_entries,sizeof(int),1,file);
	student_t* record;
	int i;
	record = malloc(sizeof(student_t)* *nr_entries);
	for(i=0; i < *nr_entries; ++i){ // leemos cada uno de los campos de student_t
		student_t* cur = &record[i];
		fread(&record[i].student_id,sizeof(int),1,file);
		fread(&record[i].NIF,MAX_CHARS_NIF + 1, 1, file); 
		record[i].first_name = loadstr(file);
		record[i].last_name = loadstr(file);
	}

	return record;
}


/**
 * @brief Free up the array of entries, including the 
 * memory of dynamically allocated strings
 * 
 * @param entries 
 * @param nr_entries 
 */
static void free_entries(student_t* entries, int nr_entries)
{
	int i=0;
	student_t* entry;

	for (i=0; i<nr_entries; i++) {
		entry=&entries[i]; /* Point to current entry */
		free(entry->first_name);
		free(entry->last_name);
	}

	free(entries);
}

/**
 * @brief This function takes care of invoking the parser, 
 * and then displays the contents of /etc/passwd based on the 
 * selected output mode
 * 
 * @param options 
 * @return int 
 */
static int show_student(struct options* options)
{
	int nr_entries;
	int i;
	student_t* entries=read_student_file(options->file,&nr_entries);

	if (!entries)
		return EXIT_FAILURE;

	for (i=0; i<nr_entries; i++) {
		student_t* e=&entries[i]; /* Point to current entry */
		fprintf(stdout,"[Entry #%d]\n",i);
		fprintf(stdout,"\tid=%d\n\tNIF=%s\n\t"
				"first_name=%s\n\tlast_name=%s\n",
				e->student_id, e->NIF,
				e->first_name, e->last_name);
	}
	free_entries(entries,nr_entries);
	return EXIT_SUCCESS;
}

// returns index of found id or -1 if not found
int findID(int id, student_t* entries, int nr_entries){
	int i;
	for (i=0; i<nr_entries; i++) {
		student_t* e=&entries[i]; /* Point to current entry */
		if (e->student_id == id)
			return i;
	}
	return -1;
}

// returns index of found NIF or -1 if not found
int findNIF(char* NIF, student_t* entries, int nr_entries){
	int i;
	for (i=0; i<nr_entries; i++) {
		student_t* e=&entries[i]; /* Point to current entry */
		if (strcmp(e->NIF, NIF) == 0)
			return i;
	}
	return -1;
}

//Leer el fichero, buscar al estudiante por ID y si está, imprimirlo
static int printStudentWithID(int id, struct options* options)
{
	int nr_entries;
	int i;
	student_t* entries=read_student_file(options->file,&nr_entries);
	if (!entries)
		return EXIT_FAILURE;

	i = findID(id,entries,nr_entries);
	if (i != -1){
		student_t* e = &entries[i];
		fprintf(stdout,"[Entry #%d]\n",i);
		fprintf(stdout,"\tid=%d\n\tNIF=%s\n\t"
				"first_name=%s\n\tlast_name=%s\n",
				e->student_id, e->NIF,
				e->first_name, e->last_name);
	}
	else
		printf("No entry was found\n");
		
	free_entries(entries,nr_entries);
	return EXIT_SUCCESS;
}
//Leer el fichero, buscar al estudiante por NIF y si está, imprimirlo
static int printStudentWithNIF(char* NIF, struct options* options)
{
	int nr_entries;
	int i;
	student_t* entries=read_student_file(options->file,&nr_entries);
	if (!entries)
		return EXIT_FAILURE;

	i = findNIF(NIF,entries,nr_entries);
	if (i != -1){
		student_t* e = &entries[i];
		fprintf(stdout,"[Entry #%d]\n",i);
		fprintf(stdout,"\tid=%d\n\tNIF=%s\n\t"
				"first_name=%s\n\tlast_name=%s\n",
				e->student_id, e->NIF,
				e->first_name, e->last_name);
		
	}
	else
		printf("No entry was found\n");
	

	free_entries(entries,nr_entries);
	return EXIT_SUCCESS;
}

int main(int argc, char *argv[])
{
	int retCode =EXIT_SUCCESS, opt;
	char* aux;
	int fflag = 0,qflag = 0;
	struct options options;

	/* Initialize default values for options */
	options.file = NULL;
	

	/* Parse command-line options */
	// Se usa f: si -f debe estar seguido de arg obligatorio
	while((opt = getopt(argc, argv, "f:hlc:a:qi:n:")) != -1) {
		switch(opt) {
		case 'h':
			fprintf(stderr,"Usage: %s -f file [ -h | -l | -c | -a | -q [ -i|-n ID]] [list of records]]\n",argv[0]);
			exit(0);
		case 'l':
			if (fflag == 1){
				if ((options.file=fopen(options.filename,"r+"))==NULL) {
					fprintf(stderr, "The input file %s could not be opened: ",
							options.filename);
					perror(NULL);
					exit(EXIT_FAILURE);
				}
				retCode=show_student(&options);

			}
			break;
		case 'c':
			if (fflag == 1){
				if ((options.file=fopen(options.filename,"w+"))==NULL) {
					fprintf(stderr, "The input file %s could not be opened: ",
							options.filename);
					perror(NULL);
					exit(EXIT_FAILURE);
				}
				else{
					// dump entries de los ya parseados. Tengo que saber cuantos son
					int cuantos = argc - optind + 1;
					fwrite(&cuantos,sizeof(int),1,options.file); // Escribir cuántos
					dump_entries(parse_records(&argv[optind-1], &cuantos),cuantos,options.file);
					fprintf(stdout,"%d records written succesfully\n",cuantos);
				}
			}
			break;
		case 'a':
			// Cargar el fichero, añadirlos al final, parsear de nuevo y dump
			if (fflag  == 1){
				if ((options.file=fopen(options.filename,"r+"))==NULL) {
					fprintf(stderr, "The input file %s could not be opened: ",
							options.filename);
					perror(NULL);
					exit(EXIT_FAILURE);
				}
				else{
					// dump entries de los ya parseados. Tengo que saber cuantos son
					int cuantos = argc - optind + 1;
					int numAntiguos, index;
					student_t* antiguos = read_student_file(options.file,&numAntiguos);
					student_t* nuevos = parse_records(&argv[optind-1], &cuantos);
					if (index = checkDuplicates(nuevos,cuantos,antiguos,numAntiguos) != 0)
						printf("Found duplicates\n");
					else {
						int total = cuantos + numAntiguos;
						fseek(options.file,0,SEEK_SET);
						fwrite(&total,sizeof(int),1,options.file);
						fseek(options.file,0,SEEK_END);
						dump_entries(nuevos,cuantos,options.file);
						fprintf(stdout,"%d extra records written succesfully\n",cuantos);
					}
				}
			}
			break;
		case 'f':
			options.filename = clone_string(optarg);
			fflag = 1; //Dejamos flags para saber que se ha introducido -f
			break;
		case 'q':
			qflag = 1; // Dejamos flag para saber que se ha introducido -q
			break;
		case 'i':
			if (qflag == 1 && fflag == 1){
				if ((options.file=fopen(options.filename,"r"))==NULL) {
					fprintf(stderr, "The input file %s could not be opened: ",
							options.filename);
					perror(NULL);
					exit(EXIT_FAILURE);
				}
				else {
					int id = atoi(optarg);
					printStudentWithID(id,&options);
				}
			}
			else{
				fprintf(stderr, "Bad usage of commands");
			}
			break;
		case 'n':
			if (qflag == 1 && fflag == 1){
				if ((options.file=fopen(options.filename,"r"))==NULL) {
					fprintf(stderr, "The input file %s could not be opened: ",
							options.filename);
					perror(NULL);
					exit(EXIT_FAILURE);
				}
				else {
					char* NIF = clone_string(optarg);
					printStudentWithNIF(NIF,&options);
				}
			}
			else{
				fprintf(stderr, "Bad usage of commands");
			}
			break;
		default:
			exit(EXIT_FAILURE);
		}
	}
	
	
	exit(retCode);
}
