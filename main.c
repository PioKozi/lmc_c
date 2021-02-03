#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const int ramsize = 99;
struct entry {
    char *str;
    int   n;
};
const struct entry instructions[] = {
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
    int n_instructions = sizeof(instructions) / sizeof(struct entry);
    for (int i = 0; i < n_instructions; i++)
        if (strcmp(instr, instructions[i].str) == 0)
            return instructions[i].n;
    return -1;
}
int loc_for_pointer(char *varname, struct entry *pointers)
{
    for (int i = 0; i < ramsize; i++)
        if (strcmp(varname, pointers[i].str) == 0)
            return pointers[i].n;
    return -1;
}

void find_pointers(struct entry *pointers)
{
    FILE *  inst_fd = fopen("./instructions", "r");
    char *  inst    = NULL;
    size_t  n       = 0;
    ssize_t nread;
    int     i = 0;
    while ((nread = getline(&inst, &n, inst_fd)) != -1) {
        char *token = strtok(inst, " ");
        if (code_for_instr(token) == -1) {
            pointers[i].str = malloc(sizeof token);
            strcpy(pointers[i].str, token);
            pointers[i].n = i;
        }
        i++;
    }
    free(inst);
    fclose(inst_fd);
}

void load_instructions() {}

int main()
{
    int ram[ramsize];  // array acts as main memory
    // map pointer/var name to pointer location/line
    struct entry pointers[ramsize];
    find_pointers(pointers);
    printf("%s %d\n", pointers[0].str, pointers[0].n);
    printf("%s %d\n", pointers[1].str, pointers[1].n);
    return 0;
}
