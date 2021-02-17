#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RAMSIZE 99

struct entry {
    char* str;
    int   n;
};

const struct entry instructions[] = {
    {"ADD", 1},    // add
    {"SUB", 2},    // subtract
    {"STA", 3},    // store
    {"LDA", 5},    // load
    {"BRA", 6},    // always branch
    {"BRZ", 7},    // branch if zero
    {"BRP", 8},    // branch if positive
    {"INP", 901},  // input
    {"OUT", 902},  // output
    {"END", 0},    // halt
    {"DAT", 0},    // data/variable
};

// return opcode if found, -1 if not found
int code_for_instr(char* instr)
{
    static const int n_instructions = 11;  // will not change
    for (int i = 0; i < n_instructions; i++)
        if (strcmp(instr, instructions[i].str) == 0) return instructions[i].n;
    return -1;
}

// return line/location for data/instruction pointed to by name, -1 if not found
int loc_for_pointer(char* name, struct entry* pointers)
{
    for (int i = 0; i < RAMSIZE; i++)
        if (strcmp(name, pointers[i].str) == 0) return pointers[i].n;
    return -1;
}

// return 1 if string is numeric, 0 if it is not.
// Only considers positive integers.
int is_numeric(char* str)
{
    for (int i = 0; i < strlen(str); i++)
        if (!isdigit(str[i])) return 0;
    return 1;
}

// load pointers into pointers structure using file inst_fd
void find_pointers(struct entry* pointers, char* filename)
{
    FILE*   inst_fd = fopen(filename, "r");
    char*   inst    = NULL;
    size_t  n       = 0;
    ssize_t nread;
    int     i = 0;
    while ((nread = getline(&inst, &n, inst_fd)) != -1) {
        char* token = strtok(inst, " ");
        if (code_for_instr(token) == -1) {
            pointers[i].str = malloc(sizeof token);
            strcpy(pointers[i].str, token);
            pointers[i].n = i;
        }
        i++;
    }
    fclose(inst_fd);
    free(inst);
}

// return 0 on success, return -1 on fail
int load_instructions(int* ram, char* filename, struct entry* pointers)
{
    FILE*   inst_fd = fopen(filename, "r");
    char*   inst    = NULL;
    char*   token   = NULL;
    size_t  n       = 0;
    ssize_t nread;
    int     location = 0;
    while ((nread = getline(&inst, &n, inst_fd)) != -1) {
        int opval                   = 0;
        token                       = strtok(inst, " ");
        token[strcspn(token, "\n")] = 0;  // remove newline
        if (code_for_instr(token) == -1)  // check named/pointed-to line
            token = strtok(NULL, " ");    // next token, instr
        if (token == NULL) {  // there is only a pointer and no instruction
            printf("line %d: no/bad instruction\n", location);
            return -1;
        }
        int opcode = code_for_instr(token);
        if (opcode == -1) {  // invalid instruction
            printf("line %d: bad instruction, %s\n", location, token);
            return -1;
        }
        if (opcode == 901 || opcode == 902)
            opval += opcode;
        else
            opval += 100 * opcode;
        token = strtok(NULL, " ");  // next token, "mailbox"
        if (token != NULL) {
            token[strcspn(token, "\n")] = 0;  // remove newline
            if (is_numeric(token) == 1) {
                int tmp;
                sscanf(token, "%d", &tmp);
                opval += tmp;
            } else {
                int pointer_loc = loc_for_pointer(token, pointers);
                if (pointer_loc == -1) {
                    printf("line %d: bad pointer, %s\n", location, token);
                    return -1;
                } else
                    opval += pointer_loc;
            }
        }
        ram[location] = opval;
        location++;
    }
    fclose(inst_fd);
    free(inst);
    return 0;
}

void add(int* acc, int* cir, int* mar, int* mdr, int* ram)
{
    *mar = *cir % 100;
    *mdr = ram[*mar];
    *acc += *mdr;
}
void sub(int* acc, int* cir, int* mar, int* mdr, int* ram)
{
    *mar = *cir % 100;
    *mdr = ram[*mar];
    *acc -= *mdr;
}
void store(int* acc, int* cir, int* mar, int* mdr, int* ram)
{
    *mar      = *cir % 100;
    *mdr      = *acc;
    ram[*mar] = *mdr;
}
void load(int* acc, int* cir, int* mar, int* mdr, int* ram)
{
    *mar = *cir % 100;
    *mdr = ram[*mar];
    *acc = *mdr;
}
void branch(int* pc, int* cir) { *pc = *cir % 100; }
void branch_if_zero(int* pc, int* cir, int* acc)
{
    if (*acc == 0) *pc = *cir % 100;
}
void branch_if_pos(int* pc, int* cir, int* acc)
{
    if (*acc >= 0) *pc = *cir % 100;
}
void io(int* acc, int* cir)
{
    switch (*cir % 100) {
    case 1: scanf("%d", acc); break;
    case 2: printf("%d\n", *acc); break;
    default: printf("bad instruction %d\n", *cir);
    }
}

void cycle(int* ram)
{
    // registers involved
    // included just because of what the program *should* be
    int pc  = 0;
    int mar = 0;
    int mdr = 0;
    int cir = 0;
    int acc = 0;
    while (1) {  // break must be explicit (reason same as registers)
        mar = pc;
        pc++;
        mdr = ram[mar];
        cir = mdr;
        // clang-format off
        switch (cir / 100) {
        case 0: if (cir == 0) return;  // if HLT, end cycle, else (DAT) ignore
        case 1: add(&acc, &cir, &mar, &mdr, ram); break;
        case 2: sub(&acc, &cir, &mar, &mdr, ram); break;
        case 3: store(&acc, &cir, &mar, &mdr, ram); break;
        case 5: load(&acc, &cir, &mar, &mdr, ram); break;
        case 6: branch(&pc, &cir); break;
        case 7: branch_if_zero(&pc, &cir, &acc); break;
        case 8: branch_if_pos(&pc, &cir, &acc); break;
        case 9: io(&acc, &cir); break;
        }
        // clang-format on
    }
}

int main(int argc, char* argv[])
{
    char* filename = NULL;
    if (argc > 1) {
        filename = malloc(sizeof argv[1]);
        strcpy(filename, argv[1]);
    } else {
        printf("file to read: ");
        size_t len = 0;
        getline(&filename, &len, stdin);
        sscanf(filename, "%s", filename);
    }
    int ram[RAMSIZE] = {0};  // array acts as main memory
    // map pointer/var name to pointer location/line
    struct entry pointers[RAMSIZE];
    find_pointers(pointers, filename);
    int ret = load_instructions(ram, filename, pointers);
    if (ret == 0) cycle(ram);  // if loading was successful
    return 0;
}
