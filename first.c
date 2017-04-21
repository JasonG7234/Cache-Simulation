#include "first.h"

int main(int argc, char ** argv) {
        //Make sure file input is formatted correctly
        checkInput(argv);
        int i = 0;
        
        setNum = cacheSize / (blockSize * assoc);
        
        //Define cache A and B
        Cache tableA[setNum];
        for (i = 0; i < setNum; i++) {
            tableA[i].q = (Queue*) calloc(1, sizeof(Queue));
        }
        Cache tableB[setNum];
        for (i = 0; i < setNum; i++) {
            tableB[i].q = (Queue*) calloc(1, sizeof(Queue));
        }
        //Used for converting hex addresses to binary
        binary = malloc(49);
        binary = "0000000100100011010001010110011110001001101010111100110111101111";
        digits = malloc(16);
        digits = "0123456789abcdef";
        
        
        fillCache(tableA, tableB);

        fclose(file);
        free(fileText);
        printResults();
        
return 0;
}

void printResults() {
    printf("cache A\n");
    printf("Memory reads: %d\n", memARead);
    printf("Memory writes: %d\n", memAWrite);
    printf("Cache hits: %d\n", cacheAHit);
    printf("Cache misses: %d\n", cacheAMiss);
    printf("cache B\n");
    printf("Memory reads: %d\n", memBRead);
    printf("Memory writes: %d\n", memBWrite);
    printf("Cache hits: %d\n", cacheBHit);
    printf("Cache misses: %d\n", cacheBMiss);
}

void checkInput(char ** input) {
        if (input[1] == NULL || input[2] == NULL || input[3] == NULL || input[4] == NULL) {
                printf("ERROR : improper input format\n");
                exit(1);
        }
        
        /* Get cache size */
        cacheSize = atoi(input[1]);
        if (cacheSize <= 0 || !PowerOfTwo(cacheSize)) {
                printf("ERROR : improper cache size\n");
                exit(1);
        }

	    /* Get block size */
        blockSize = atoi(input[3]);
        if (blockSize <= 0 || !PowerOfTwo(blockSize) || blockSize > cacheSize) {
                printf("ERROR : improper block size\n");
                exit(1);
        }

        /*Get associativity */
        if (strncmp(input[2], "direct", 6) == 0) {

                /* Direct-mapped cache */
                assoc = 1;

        } else if(strncmp(input[2], "assoc:", 6) == 0) {

                /* Cache with given associativity */
                int index = 6; 
                assoc = input[2][index] - '0'; 
                while (isdigit(input[2][index+1]) != 0) {
                    assoc = assoc * 10 + (input[2][index+1] - '0');
                    index++;
                }

        } else {
            
                /* Fully associative cache */
                assoc = cacheSize / blockSize;
        }
        
        if (assoc <= 0) {
                printf("ERROR : improper associativity\n");
                exit(1);
        }
        
        /*Get file text*/
        file = fopen(input[4], "r");
        if (file == NULL) {
            printf("ERROR : Incorrect file name\n");
            exit(1);
        }
        long sizeOfFile = 0;
        fseek(file, 0L , SEEK_END);
        sizeOfFile = ftell(file);
        fseek(file, 0L, SEEK_SET);
        fileText = malloc(sizeOfFile);
        if (fileText == NULL) {
            mallocError();
        }
        
        if(fread(fileText, sizeOfFile, 1, file) != 1){
            
            //If there's an error reading the contents of the file
            printf("ERROR : Unable to Read File");
            exit(1);
        }
        
}

bool PowerOfTwo(int x) {
    return ((x!=0) && (x & (x-1)) == 0);
}

void mallocError() {
    printf("ERROR : unable to malloc. Exiting program.\n");
    exit(1);
}

void fillCache(Cache * tableA, Cache * tableB) {
    bool r = false;
    bool w = false;
    fileText = strtok(fileText, " \n\t\r\f\v\0");
    
    while(fileText != NULL ) {
        if (r) {
            parseMemory(fileText, tableA, tableB);
        } if (w) {
            memAWrite++;
            memBWrite++;
            parseMemory(fileText, tableA, tableB);
        }
        
        r = false;
        w = false;
        
        if (strncmp(fileText, "R", 1) == 0) {
            r = true;
        } else if (strncmp(fileText, "W", 1) == 0) {
            w = true;
        }
        fileText = strtok(NULL, " ");
    }
    
}

void parseMemory(char * val, Cache * tableA, Cache * tableB) {
    char * number = malloc(49);
    if (number == NULL) {
        mallocError();
    }
    number = binaryConversion(val);
    search(number, tableA, tableB);
}

void updateCache(Cache * table, Line * temp, char * number) {
    
    int i = 0;
    Queue * trail = NULL;
    //First base case -> queue is empty
     if (table[temp->index].q == NULL || table[temp->index].q->line == NULL) { 
         Queue * a = malloc(sizeof(Queue));
        a->next = NULL;
        a->prev = NULL;
        a->line = temp;
        table[temp->index].q = a;
        return;
    } else {
    //Else -> queue has at least one input
        Queue * a = table[temp->index].q;
        for(i = 0; i<assoc; i++) {
            if (a == NULL) {
                a = calloc(1, sizeof(Queue));
                a->next = NULL;
                a->prev = trail;
                a->line = temp;
                trail->next = a;
                return;
            } else {
                if (strncmp(temp->tag, a->line->tag, max(strlen(temp->tag), strlen(a->line->tag))) == 0) {
                    a->line = temp;
                    return;
                }
            }
            trail = a;
            a = a->next;
        }
    }
    if (assoc == 1) {
    Queue * insert = malloc(sizeof(Queue));
    insert->next = NULL;
    insert->prev = NULL;
    insert->line = temp;
    table[temp->index].q = insert;
    } else {
    table[temp->index].q->next->prev = NULL;
    table[temp->index].q = table[temp->index].q->next; //dequeue
    Queue * insert = malloc(sizeof(Queue));
    insert->next = NULL;
    insert->prev = trail;
    insert->prev->next = insert;
    insert->line = temp;
    }
}

char * binaryConversion (char * hexVal) {
    
    /* All of the conversion is done through two string arrays that convert the given char to its bit */
    
    char * conversion = (char*) calloc(1, 49);
    int i;
    char * temp = malloc(5); 
    
    char current = *(hexVal+2);
    while (current != '\n') {
    for (i = 0; i < 16; i++) {
        if (current == digits[i]) {
            if (strncmp(conversion, "\0", 1) == 0) {
                strncpy(temp, (binary + (i*4)), 4);
                strncpy(conversion, temp, 4);
                
            } else {
                strncpy(temp, (binary + (i*4)), 4);
                strcat(conversion, temp);
            }
            break;
        }
    }
    current = *(++hexVal + 2);
    }
    conversion = extendAddress(conversion);
    
    //free(temp);
    
    return conversion;
    
}

char * extendAddress(char * address) {
    
    int i;
    if (strlen(address) < 48) {
        char * extend = calloc(1, 49);
        for (i = 0; (48-strlen(address)) > i; i++) {
            strncat(extend, "0", 1);
        }
        strcat(extend, address);
        strncpy(address, extend, 49);
        //free(extend);
        return address;
    }
    
    return address;
}

Line * getBitInfo(char * address, bool ifAthen0) {
    Line * temp = malloc(sizeof(Line));
    if (temp == NULL) {
        mallocError();
    }
    
    char * offset = malloc((int) (log2((double)blockSize)));
    if (offset == NULL) {
        mallocError();
    }
    offset = address + strlen(address) - ((int) (log2((double)blockSize)));
    temp->offset = offset;
    //printf("offset is -> %s\n", offset);
    
    char * index1 = malloc((int) (log2((double)setNum)) + 1);
    if (index1 == NULL) {
        mallocError();
    }
    if (ifAthen0) {
    strncpy(index1, address, (int) (log2((double)setNum)));
    } else {
    strncpy(index1, (address + strlen(address) - strlen(offset) - (int) (log2((double)setNum))), (int) (log2((double)setNum)));
    }
    int res = 0;
    int len = strlen(index1) - 1;
    int i = 0;
    while (len!=-1) {
        if (strncmp(index1+i, "1", 1) == 0) {
            res += pow(2, len);
        }
        len--;
        i++;
    }
    temp->index = res;
    
    tag = malloc(49 - (int)strlen(offset)- ((int) (log2((double)setNum))));
    if (tag == NULL) {
        mallocError();
    }
    if (ifAthen0) {
    strncpy(tag, address+strlen(index1), 48-(int)strlen(offset) - ((int)(log2((double)setNum))));
    } else {
        strncpy(tag, address, 48-(int)strlen(offset) - ((int)(log2((double)setNum))));
    }
    temp->tag = tag; 
    
    return temp;
}

void search(char * data, Cache * tableA, Cache * tableB) {
    int i = 0;
    bool breakA = false;
    Line * keyA = getBitInfo(data, 0);
    Line * keyB = getBitInfo(data, 1);
    
    Queue * quA = tableA[keyA->index].q;
    Queue * quB = tableB[keyB->index].q;
    
    //Cache A
    for(i = 0; i < assoc; i++) {
        if (quA == NULL || quA->line == NULL) {
            //Empty spots in sets
            cacheAMiss++;
            memARead++;
            updateCache(tableA, keyA, data);
            breakA = true;
            break;
        } else {
            if (strncmp(data, quA->line->tag, strlen(quA->line->tag)) == 0) {
                cacheAHit++;
                breakA = true;
                break;
            }
        }
        quA = quA->next;
    }
    if (!breakA) {
    cacheAMiss++;
    memARead++;
    updateCache(tableA, keyA, data);
    }
    //Cache B
    for(i = 0; i < assoc; i++) {
        if (quB == NULL || quB->line == NULL) {
            //Empty spots in sets
            cacheBMiss++;
            memBRead++;
            updateCache(tableB, keyB, data);
            return;
        } else {
            if (strncmp(data+(int)(log2((double)setNum)), quB->line->tag, strlen(quB->line->tag)) == 0) {
                cacheBHit++;
                return;
            }
        }
        quB = quB->next;
    }
    cacheBMiss++;
    memBRead++;
    updateCache(tableB, keyB, data);
}

int btoi(char * address) {
    char * index = malloc((int) (log(setNum)/log(2)));
    if (index == NULL) {
        mallocError();
    }
    index = address + strlen(address) - ((int) (log(setNum)/log(2)));
    int res = 0;
    int len = strlen(index) - 1;
    int i = 0;
    while (len!=-1) {
        if (strncmp(index+i, "1", 1) == 0) {
            res += pow(2, len);
        }
        len--;
        i++;
    }
    //free(index);
    return res;
}

int max(int a, int b) {
    if (a > b) return a;
    return b;
}
