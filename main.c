#include "filesys.h"
#define USERS
#define MAX 99999999
#define INSTR_SZ 3

// get input
char * input(char * line) {
    printf("cmd: ");
    fgets(line, MAX, stdin);
    return line;
}

/* split line to get command always... get other inputs if possible */
void linePartition(char * line, char ** cmd, char ** file) {
    int i = 0;
    *cmd = malloc(sizeof(char) * INSTR_SZ);
    if (*cmd == NULL) {
        printf("memory allocation failed\n");
        exit(1);
    }

    // need '- ' after command if using other inputs
    while (i < INSTR_SZ && line[i] != '-' && line[i] != '\0' && line[i] != '\n') {
        (*cmd)[i] = line[i];
        i++;
    }
    (*cmd)[i] = '\0';
    
    if (line[i] == '\0') {
        *file = NULL;
        return;
    }
    i+=2;

    *file = malloc(sizeof(char) * (strlen(line + i)));
    if (*file == NULL) {
        printf("memory allocation failed\n");
        exit(1);
    }
    strcpy(*file, line + i);
    char * nL = strchr(*file, '\n');
    if (nL != NULL) {
        *nL = '\0';
    }
} 

int main(void) {

    controls * c = malloc(sizeof(controls));
    if (c == NULL) exit(1);
    initc(c);

    char root[] = "root";
    directory * D = initD(NULL, root);

    char * line = malloc(sizeof(char) * MAX);
    if (line == NULL) exit(1);

    char * instruction = NULL;
    char * file = NULL;
    unsigned op;

    // get input until 'q' inputted
    while (c->status) {
        line = input(line);
        linePartition(line, &instruction, &file);
        if (opfetch(instruction, &op)) {
            if (setc(op, c)) {
                printf("\n");
                if (op == 0x73 || op == 0x73 + 0x72 || !c->status) {
                    continue;
                } else if (!c->allow && !c->root) {
                    printf("you must login first to access commands\n");
                } else {
                    performOp(c, file, D);
                }
            } else {
                printf("command not found\n");
            }
        } else {
            printf("command not found\n");
        }
        printf("\n");
        free(instruction);
        free(file);
    }
    free(line);
    free(c);
}