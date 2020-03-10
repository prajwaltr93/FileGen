//filename : filegen.h
//author : PrajwalTR
//date last modified : Tue Mar 10 22:27:03 2020
//comments : contains function prototypes and constants

//constants
#define MAXEXTSIZE 7 //exetension for file max length
#define STRUCT_ELE sizeof(comments)/sizeof(comments[0]) //
#define PROP_SIZE  sizeof(prop)/sizeof(prop[0])
//maximum line length to write to file
#define MAXLINELEN 400
//function prototypes for filegen.c
//get extension from file name return NULL on invalid filetype
void *getextension(char *filename);
//get corresponding comment to use for got extension
void *getcomment(char *extension);
//subroutine to write to file
void writeline(int fd_open,char *comment,char *prop,char *value,char *filename);
