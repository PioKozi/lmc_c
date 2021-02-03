#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const int ramsize = 99;
struct entry {
    char *str;
    int   n;
};
// map pointer/var name to pointer location/line
struct entry       pointers[ramsize] = {};
const struct entry instructions[]    = {
    {"LDA", 5},    // load
    {"STA", 3},    // store
    {"ADD", 1},    // add
    {"SUB", 2},    // subtract
    {"INP", 901},  // input
    {"OUT", 902},  // output
    {"END", 0},    // halt
    {"BRZ", 7},    // branch if zero
    {"BRP", 8},    // branch if positive
    {"BRA", 6},    // always branch
    {"DAT", 0},    // data/variable
};
// return int if found, -1 if not found
int code_for_instr(char *instr)
{
    int n_instructions = sizeof(instructions) / sizeof(instructions[0]);
    for (int i = 0; i < n_instructions; i++)
        if (strcmp(instr, instructions[i].str) == 0)
            return instructions[i].n;
    return -1;
}
int loc_for_pointer(char *varname)
{
    int n_vars = sizeof(pointers) / sizeof(pointers[0]);
    for (int i = 0; i < n_vars; i++)
        if (strcmp(varname, pointers[i].str) == 0)
            return pointers[i].n;
    return -1;
}

void load_data(int *ram)
{
    return;
}

void load_instructions() {}

int main()
{
    /* CPU REGISTERS */
    int ram[ramsize];  // array acts as main memory
    /* int   acc = 0;       // acculumator */
    /* int   pc  = 0;       // program counter */
    /* int   mar = 0;       // memory address register */
    /* int   mdr = 0;       // memory data register */
    /* char *cir;           // current instruction register */
    load_data(ram);
    return 0;
}
