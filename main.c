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
    
    if ((line[i] == '\0' || line[i+1] == '\0') || line[i] == '\n' || line[i+1] == '\n') {
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
    controls * c = malloc(sizeof(controls)); // control structure
    if (c == NULL) exit(1);
    initc(c);

    char root[] = "root";
    directory * D = initD(NULL, root); // initial root directory

    usern * userns = malloc(sizeof(usern)); // usernames trie

    user * users = malloc(sizeof(user)); // user linked list to store user's directories 
    users->name = strdup("root");
    users->d = D;
    users->next = NULL;

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
        if (opfetch(instruction, &op)) {
            if (setc(op, c, users, userns, file)) {
                printf("\n");
                if (!c->status) { // exiting program
                    printf("g\t\t\t\t\t\te \n\to\t\t\t\ty \n\t\to\t\tb \n\t\t\td \n\t\to\t\tb \n\to\t\t\t\ty \ng\t\t\t\t\t\te\n");
                    sleep(1);
                    goto jump;
                } else if (op == 0x6c + 0x6f) goto jump; // logout 
                if (!c->allow && !c->root) {  // no access
                    printf("you must login first to access commands\n");
                } else {
                    if (c->back || c->printd) D = performOp(op, c, file, D, users);
                    else if (file == NULL) printf("command not directed anywhere\n");
                    else D = performOp(op, c, file, D, users);
                }
            } else if (op == 0x6e + 0x75) { 
                if (file == NULL) {
                    printf("\nno user name inputted\n\n");
                    continue;
                }
                addU(userns, file); // add new user
                addULL(users, file); // add user to linked list
                printf("\n%s added... login as %s to access their files\n", file, file);
            } else if (op != 0x73) {
                printf("command not found\n");
            }
        } else {
            printf("command not found\n");
        }
        jump: // jump point
        free(line);
        printf("\n");
        free(instruction);
        free(file);
        instruction = NULL;
        file = NULL;
    }
    // reset users parent D if logged into root at end of program
    if (users->d->left->left != NULL) {
        users->d->left->left->parent = NULL;
        users->d->left->left = NULL;
    }
    if (users->d->left->right != NULL) {
        users->d->left->right->parent = NULL;
        users->d->left->right = NULL;
    }
    free(c); // free control structure
    freeU(users); // free data of users
    freeNames(userns); // free user's names in trie
}