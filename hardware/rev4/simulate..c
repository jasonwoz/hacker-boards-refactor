/* EECS 370 LC-2K Instruction-level simulator */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define NUMMEMORY 65536 /* maximum number of words in memory */
#define NUMREGS 8 /* number of machine registers */
#define MAXLINELENGTH 1000

typedef struct stateStruct {
	int pc;
	int mem[NUMMEMORY];
	int reg[NUMREGS];
	int numMemory;
} stateType;

void printState(stateType *);
int getOpcode(int machine);
void modifyState(int machine, stateType *state);
int aReg(int machine);
int bReg(int machine);
int dReg(int machine);
int offset(int machine);
int convertNum(int num);

int main(int argc, char *argv[])
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
		printf("error: can't open file %s", argv[1]);
		perror("fopen");
		exit(1);
	}

	/* read in the entire machine-code file into memory */
	for (state.numMemory = 0; fgets(line, MAXLINELENGTH, filePtr) != NULL;
		state.numMemory++) {

		if (sscanf(line, "%d", state.mem + state.numMemory) != 1) {
			printf("error in reading address %d\n", state.numMemory);
			exit(1);
		}
		printf("memory[%d]=%d\n", state.numMemory, state.mem[state.numMemory]);
	}
	for (int x = 0; x < NUMREGS; x++){
		state.reg[x] = 0;
	}
	state.pc = 0;
	int num = 0;
	for(state.pc; state.pc < state.numMemory; state.pc++)
	{
		printState(&state);
		num++;
		int machine = state.mem[state.pc];
		if (getOpcode(machine) == 6)
		{
			printf("machine halted\ntotal of %i directions executed\nfinal state of machine:\n", num);
			state.pc++;
			printState(&state);
			break;
		}
		else
		{
			modifyState(machine, &state);
		}
	}

	fclose(filePtr);



	return(0);
}

void
printState(stateType *statePtr)
{
	int i;
	printf("\n@@@\nstate:\n");
	printf("\tpc %d\n", statePtr->pc);
	printf("\tmemory:\n");
	for (i = 0; i<statePtr->numMemory; i++) {
		printf("\t\tmem[ %d ] %d\n", i, statePtr->mem[i]);
	}
	printf("\tregisters:\n");
	for (i = 0; i<NUMREGS; i++) {
		printf("\t\treg[ %d ] %d\n", i, statePtr->reg[i]);
	}
	printf("end state\n");
}

int getOpcode(int machine)
{
	int op = 0;
	op = machine >> 22;
	return op;
}

int aReg(int machine)
{
	//shift machine left 10 then right 29
	int left10 = machine & 3670016;
	int right29 = left10 >> 19;

	return right29;

		//
}

int bReg(int machine)
{
	//shift machine left 13 then right 29
	int left13 = machine & 458752;
	int right29 = left13 >> 16;

	return right29;

	//(state.instrReg & 458752) >> 16

}

int dReg(int machine)
{
	//shift machine left 29 then right 29
	return machine & 7;

	//state.instrReg & 7
}

int offset(int machine)
{
	//shift machine left 16 then right 16
	int left16 = machine << 16;
	int right16 = left16 >> 16;

	if(right16 < 0)
	{
		convertNum(right16);
	}

	return right16;
}

void modifyState(int machine, stateType *state) //pc int is in state
{
		int opcode = machine >> 22;
		int regA = aReg(machine);
	int regB = bReg(machine);


	if (opcode == 0 || opcode == 1) //add or nand
	{
		int destReg = dReg(machine);

		//add the contents of regA and regB and put it in destReg
		if(opcode == 0)
			{
				state->reg[destReg] = state->reg[regA] + state->reg[regB];
			}
		else{
				state->reg[destReg] = ~(state->reg[regA] & state->reg[regB]);
			}
		//nand the contents of regA and regB and put it in destReg

	}
		else if (opcode == 2 || opcode == 3 || opcode == 4) //lw, sw, beq
	{
		int offsetField = offset(machine);

		//lw: add offset field to the contents of regA to load regB
		if(opcode == 2)
			{
				state->reg[regB] = state->mem[state->reg[regA] + offsetField];
			}

		else if(opcode == 3){
				state->mem[state->reg[regA] + offsetField] = state->reg[regB];
			}
		//sw: memory address of regB is stored adding offsetField to the contents of regA
		//beq: if the contents of regA and regB are the same, branch to PC+1+OffsetField
		else{
				offsetField = state->pc + offsetField;
				if(state->reg[regA] == state->reg[regB])
					{
						state->pc = offsetField;
					}

			}


	}
	else if (opcode == 5) //jalr
	{
		//store pc+1 in regB, branch to address in regA
		state->reg[regB] = state->pc + 1;
		state->pc = state->reg[regA] - 1;
	}
	else{ //halt or noop

		//halt: pc needs to increment then halt

		//noop: do nothing

	}

}

int convertNum(int num)
{
	if(num & (1<<15))
	{
		num -= (1<<16);
	}
	return(num);
}
