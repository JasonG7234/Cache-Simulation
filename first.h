#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <stdbool.h>

//Global variables
int cacheSize = 0; //Represents size of cache
int assoc = 0; //Represents associativiy
int blockSize = 0; //Represents block size
char * binary; //All binary representation for 4 digits, up to 16
char * digits; //Used to convert from hex to binary
int setNum = 0; //Represents number of sets
char * tag; //Keeps tag bits for conversion to int in btoi method
char * fileText; //Represents text in input file
FILE * file; //File pointer
int cacheAHit = 0; //Counts number of hits in cache A
int cacheAMiss = 0; //Counts number of misses in cache A
int memARead = 0; //Counts number of reads in cache A
int memAWrite = 0; //Counts number of writes in cache A
int cacheBHit = 0; //Counts number of hits in cache B
int cacheBMiss = 0; //Counts number of misses in cache B
int memBRead = 0; //Counts number of reads in cache B
int memBWrite = 0; //Counts number of writes in cache B

//Line documentation
typedef struct Line {
    char* tag;
    int index;
    char* offset;
} Line;

//Queue documentation
typedef struct Queue {
    struct Queue * next;
    struct Queue * prev;
    Line * line;
} Queue;

//Cache documentation
typedef struct Cache {
    Queue * q;
} Cache;

//Method Declarations
void printResults(); //Prints final tally of all global variables
void checkInput(char ** input); //Makes sure input format is correct
void mallocError(); //Exits program if malloc returns NULL
bool PowerOfTwo(int x); //Checks if input is proper power of two
void fillCache(Cache * tableA, Cache * tableB); //Interprets input and sends it to the read or write method
void parseMemory(char * val, Cache * tableA, Cache * tableB); //Helper method that passes address into binaryConversion and search
void updateCache(Cache * table, Line * temp, char * number); //Update given queue in given cache
char * binaryConversion(char * hexVal); //Converts hexadecimal location to binary
char * extendAddress(char * binary); //Extends the binary conversion to 48 bits
Line * getBitInfo(char * binary, bool ifAthen0); //Returns tag, index, and offset bits for each cache in line struct
void search(char * data, Cache * tableA, Cache * tableB); //Searches through the cache for given address
int btoi(char * address); //In search, translates index into an integer
int max(int a, int b); //Returns max for strncmp uses

