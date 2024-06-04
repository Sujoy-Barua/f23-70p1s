/*
 * Project 1
 * EECS 370 LC-2K Instruction-level simulator
 *
 * Make sure to NOT modify printState or any of the associated functions
 */

#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

//DO NOT CHANGE THE FOLLOWING DEFINITIONS 

// Machine Definitions
#define MEMORYSIZE 65536 /* maximum number of words in memory (maximum number of lines in a given file)*/
#define NUMREGS 8 /*total number of machine registers [0,7]*/

// File Definitions
#define MAXLINELENGTH 1000 /* MAXLINELENGTH is the max number of characters we read */

typedef struct 
stateStruct {
    int pc;
    int mem[MEMORYSIZE];
    int reg[NUMREGS];
    int numMemory;
} stateType;

void printState(stateType *);

static inline int convertNum(int32_t);

int 
main(int argc, char **argv)
{
    char line[MAXLINELENGTH];
    stateType state;
    FILE *filePtr;

    if (argc != 2) {
        printf("error: usage: %s <machine-code file>\n", argv[0]);
        exit(1);
    }

    filePtr = fopen(argv[1], "r");
    if (filePtr == NULL) {
        printf("error: can't open file %s , please ensure you are providing the correct path", argv[1]);
        perror("fopen");
        exit(1);
    }

    /* read the entire machine-code file into memory */
    for (state.numMemory = 0; fgets(line, MAXLINELENGTH, filePtr) != NULL; ++state.numMemory) {
        if (sscanf(line, "%d", state.mem+state.numMemory) != 1) {
            printf("error in reading address  %d\n. Please ensure you are providing a machine code file.", state.numMemory);
            perror("sscanf");
            exit(1);
        }
        printf("memory[%d]=%d\n", state.numMemory, state.mem[state.numMemory]);
    }

    //Your code starts here!
    int instructions = 0;
    state.pc = 0;
    for (int i = 0; i < 8; i++) {
        state.reg[i] = 0;
    }
    printState(&state);


    while (((state.mem[state.pc] & (0b111 << 22)) >> 22) != 0b110) {

        if (((state.mem[state.pc] & (0b111 << 22)) >> 22) == 0b000) {
            //add
            state.reg[state.mem[state.pc] & (0b111)] = 
            (state.reg[(state.mem[state.pc] & (0b111 << 19)) >> 19]) + 
            (state.reg[(state.mem[state.pc] & (0b111 << 16)) >> 16]);
            state.pc++;
            instructions++;
        }
        else if (((state.mem[state.pc] & (0b111 << 22)) >> 22) == 0b001) {
            //nor
            state.reg[state.mem[state.pc] & (0b111)] = 
            ~((state.reg[(state.mem[state.pc] & (0b111 << 19)) >> 19]) | 
            (state.reg[(state.mem[state.pc] & (0b111 << 16)) >> 16]));
            state.pc++;
            instructions++;
        }
        else if (((state.mem[state.pc] & (0b111 << 22)) >> 22) == 0b010) {
            //lw
            int baseReg = (state.mem[state.pc] & (0b111 << 19)) >> 19;
            int offset = convertNum(state.mem[state.pc] & (0xFFFF));
            state.reg[(state.mem[state.pc] & (0b111 << 16)) >> 16] = state.mem[state.reg[baseReg] + offset];
            state.pc++;
            instructions++;
        }
        else if (((state.mem[state.pc] & (0b111 << 22)) >> 22) == 0b011) {
            //sw
            int baseReg = (state.mem[state.pc] & (0b111 << 19)) >> 19;
            int offset = convertNum(state.mem[state.pc] & (0xFFFF));
            state.mem[state.reg[baseReg] + offset]
            = state.reg[(state.mem[state.pc] & (0b111 << 16)) >> 16];
            state.pc++;
            instructions++;
        }
        else if (((state.mem[state.pc] & (0b111 << 22)) >> 22) == 0b100) {
            //beq
            if (state.reg[(state.mem[state.pc] & (0b111 << 19)) >> 19] == 
            state.reg[(state.mem[state.pc] & (0b111 << 16)) >> 16] ) {
                state.pc = (state.pc + 1) + convertNum(state.mem[state.pc] & (0xFFFF));
            }
            else {
                state.pc++;
            }
            instructions++;
        }
        else if (((state.mem[state.pc] & (0b111 << 22)) >> 22) == 0b101) {
            //jalr
            state.reg[(state.mem[state.pc] & (0b111 << 16)) >> 16] = (state.pc)+1;
            state.pc = state.reg[(state.mem[state.pc] & (0b111 << 19)) >> 19];
            instructions++;
        }
        else if (((state.mem[state.pc] & (0b111 << 22)) >> 22) == 0b111) {
            //noop
            state.pc++;
            instructions++;
        }
        printState(&state);
    }
    state.pc++;
    instructions++;
    printf("%s\n", "machine halted");
    printf("%s%d%s\n", "total of ", instructions, " instructions executed");
    printf("%s\n", "final state of machine:");
    printState(&state);
    //Your code ends here! 

    return(0);
}

/*
* DO NOT MODIFY ANY OF THE CODE BELOW. 
*/

void 
printState(stateType *statePtr)
{
    int i;
    printf("\n@@@\nstate:\n");
    printf("\tpc %d\n", statePtr->pc);
    printf("\tmemory:\n");
    for (i=0; i<statePtr->numMemory; i++) 
              printf("\t\tmem[ %d ] %d\n", i, statePtr->mem[i]);
    printf("\tregisters:\n");
    for (i=0; i<NUMREGS; i++) 
              printf("\t\treg[ %d ] %d\n", i, statePtr->reg[i]);
    printf("end state\n");
}

// convert a 16-bit number into a 32-bit Linux integer
static inline int convertNum(int num) 
{
    return num - ( (num & (1<<15)) ? 1<<16 : 0 );
}

/*
* Write any helper functions that you wish down here. 
*/
