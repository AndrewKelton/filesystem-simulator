#include "filesys.h"
#define MAX 99999999
#define MAXLINE 80
#define MAXFLEN 100000

// set all controls false
void initc(controls * c) {
    c->root = false;
    c->allow = false;
    c->read = false;
    c->write = false;
    c->printd = false;
    c->cd = false;
    c->newd = false;
    c->save = false;
    c->status = true;
}

// reset changed controls
void resetc(controls * c) {
    c->read = false;
    c->write = false;
    c->printd = false;
    c->cd = false;
    c->newd = false;
    c->save = false;
}

// initialize directory
directory * initD(directory * pD, char * n) {
    directory * D = malloc(sizeof(directory));
    D->fs = NULL;
    D->left = NULL;
    D->right = NULL;
    D->name = n;
    D->parent = pD;
    D->len = 0;
    return D;
}

// create new file
files * createF(char * fname, char * f) {
    files * newF = malloc(sizeof(files));
    if (newF == NULL) {
        printf("memory allocation failed\n");
        exit(1);
    }
    newF->name = strdup(fname);
    newF->f = strdup(f);
    newF->left = NULL;
    newF->right = NULL;
    return newF;
}

// set controls for operation
bool setc(unsigned op, controls * c) {
    switch(op) {
        case 0x70: // print directory contents
            c->printd = true;
            break;
        case 0x71: // quit terminal simulation
            initc(c);
            c->status = false;
            break;
        case 0x72: // read file
            c->read = c->allow || c->root;
            break;
        case 0x73 + 0x61: // save file to computer
            c->save = true;
            break;
        case 0x77: // write & create file
            c->write = c->allow || c->root;
            break;
        case 0x72 + 0x77: // read & write file
            c->read = c->allow || c->root;
            c->write = c->allow || c->root;
            break;
        case 0x73: // sign in to user
            c->allow = true;
            break;
        case 0x73 + 0x72: // sign in to root
            c->root = true;
            c->allow = true;
            break;
        default: // invalid command
            return false;
    }
    return true;
}

/* check if instruction within length boundaries, 
will check if instruction is valid operation in setc */
bool opfetch(char * instr, unsigned * op) {
    int len = strlen(instr);

    if (len == 0x1) *op = (int)instr[0];
    else if (len == 0x2) {
        *op = (int)instr[0];
        *op += (int)instr[1];
    } else {
        return false;
    }
    return true;
}

// compare entire strings in case any similar characters
bool compstrngs(char * n1, char * n2) {
    int len = (strlen(n1) >= strlen(n2)) ? strlen(n2) : strlen(n1);
    for (int i = 0; i < len; i++) {
        if (n1[i] -'a' > n2[i] - 'a') return true;
        else if (n1[i] -'a' < n2[i] - 'a') return false;
    }
    return (strlen(n1) >= strlen(n2));
}

// search for file in BST
files * searchf(files * fs, char * fname) {
    if (fs == NULL) return NULL;
    if (strcmp(fs->name, fname) == 0) return fs;
    if (compstrngs(fs->name, fname)) searchf(fs->left, fname);
    return searchf(fs->right, fname);
}

// add file to D->fs Tree
files * addFT(files * fs, char * fname, char * file) {
    if (fs == NULL) {
        fs = malloc(sizeof(files));
        fs->name = malloc(sizeof(char) * strlen(fname));
        strcpy(fs->name, fname);
        fs->f = malloc(sizeof(char) * strlen(file));
        strcpy(fs->f, file);
        free(file);
        return fs;
    }
    if (compstrngs(fs->name, fname)) fs->left = addFT(fs->left, fname, file);
    else fs->right = addFT(fs->right, fname, file);
    return fs;
}

// create and write to file 
void wfile(char * fname, directory * D) {
    char * f = malloc(sizeof(char) * MAXFLEN);
    printf("HELLO\n");
    printf("to finish writing press press return\n");

    if (fgets(f, MAXFLEN-1, stdin) == NULL) f[strlen(f+1)] = '\0';

    // add to minheap 
    D->fs = addFT(D->fs, fname, f);
    D->len++;

    printf("\n");
}

// read file (print to terminal)
void rfile(files * f) {
    int len = strlen(f->f);
    f->f[len] = '\0';
    for (int i = 0; i < len; i++) {
        if (i % 80 == 0) printf("\n");
        printf("%c", f->f[i]);
    }
    printf("\n");
}

// print diretory
void printD(directory * D) {
    printf("%s:\n", D->name);
    printFT(D->fs);
}

// print files in tree
void printFT(files * f) {
    if (f == NULL) return;
    printFT(f->left);
    printf("%s\n", f->name);
    printFT(f->right);
}

// read and append to file
void rwfile(files * f) {
    int len = strlen(f->f);
    for (int i = 0; i < len; i++) {
        if (i % 80 == 0) printf("\n");
        printf("%c", f->f[i]);
    }

    printf("\n");

    char * buffer = malloc(sizeof(char) * MAXFLEN-len);
    printf("to finish writing press press return\n");
    if (fgets(buffer, MAXFLEN-1, stdin) == NULL) buffer[strlen(buffer + 1)] = '\0';
    strcat(f->f, buffer);

    printf("\n");
}

void saveF(files * tmp) {
    char * tmpname = malloc(sizeof(char) * (strlen(tmp->f) + 4));
    if (tmpname == NULL) {
        printf("memory allocation failed\n");
        exit(1);
    }
    strcpy(tmpname, tmp->name);
    strcat(tmpname, ".txt");
    FILE * fsave = fopen(tmpname, "w");

    fprintf(fsave, "%s", tmp->f);
    fclose(fsave);
    printf("file saved as %s\n", tmpname);
    free(tmpname);
}

// do the operation
void performOp(controls * c, char * fname, directory * D) {
    files * tmp;
    if (c->write && !c->read) {
        tmp = searchf(D->fs, fname);
        if (tmp == NULL) wfile(fname, D);
        else printf("file already exists\n");
    } else if (!c->write && c->read) {
        tmp = searchf(D->fs, fname);
        if (tmp != NULL) rfile(tmp);
        else printf("file does not exist\n");
    } else if (c->write && c->read) {
        tmp = searchf(D->fs, fname);
        if (tmp != NULL) rwfile(tmp);
        else  printf("file does not exist\n");
    } else if (c->printd) {
        printD(D);
    } else if (c->cd) {
        if (strcmp(D->left->name, fname) == 0) {
            D = D->left;
        } else if (strcmp(D->right->name, fname) == 0) {
            D = D->right;
        } else {
            printf("directory does not exist\n");
        }
    } else if (c->newd) {
        if (strcmp(D->left->name, fname) != 0 && strcmp(D->right->name, fname) != 0) {
            D = initD(D, fname);
        } else if (D->left != NULL || D->right != NULL) {
            printf("directory is already a child directory\n");
        } else {
            printf("no open directory path... cd and start again\n");
        }
    } else if (c->save) {
        tmp = searchf(D->fs, fname);
        if (tmp == NULL) printf("file does not exist\n");
        else {
            saveF(tmp);
        }
    }
    resetc(c);
}