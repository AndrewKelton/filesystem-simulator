#include "filesys.h"
#define MAX 99999999
#define INSTR_SZ 3 // instruction size

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
    
    if (line[i] == '\0' || line[i+1] == '\0') {
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

    char * line;

    char * instruction = NULL;
    char * file = NULL;
    unsigned op;

    // get input until 'q' inputted
    while (c->status) {
        line = malloc(sizeof(char) * MAX);
        if (line == NULL) {
            printf("memory allocation failed\n");
            exit(1);
        }
        line = input(line);
        linePartition(line, &instruction, &file);
        free(line);
        if (opfetch(instruction, &op)) {
            if (setc(op, c)) {
                printf("\n");
                if (op == 0x73 || op == 0x73 + 0x72 || !c->status) {
                    goto jump; // skip to jump
                } else if (!c->allow && !c->root) {
                    printf("you must login first to access commands\n");
                } else {
                    if (file == NULL) printf("command not directed anywhere\n");
                    else D = performOp(c, file, D);
                }
            } else {
                printf("command not found\n");
            }
        } else {
            printf("command not found\n");
        }
        jump: // jump point
        printf("\n");
        free(instruction);
        free(file);
        instruction = NULL;
        file = NULL;
    }
    free(c);
    while (D->parent != NULL) {
        D = D->parent;
    }
    freeD(D); // free all directory data
}