#include "filesys.h"
#define MAX 99999999
#define MAXLINE 80
#define MAXFLEN 1000

// set all controls false
void initc(controls * c) {
    c->root = false;
    c->allow = false;
    c->read = false;
    c->write = false;
    c->printd = false;
    c->cd = false;
    c->back = false;
    c->newd = false;
    c->save = false;
    c->rm = false;
    c->status = true;
}

// reset controls
void resetc(controls * c) {
    c->read = false;
    c->write = false;
    c->printd = false;
    c->cd = false;
    c->back = false;
    c->newd = false;
    c->save = false;
    c->rm = false;
}

// initialize directory
directory * initD(directory * pD, char * n) {
    directory * D = malloc(sizeof(directory));
    D->fs = NULL;
    D->left = NULL;
    D->right = NULL;
    D->name =  strdup(n);
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
        case 0x62 + 0x64: // to parent directory
            c->cd = true;
            c->back = true;
            break;
        case 0x63 + 0x64: // change directory
            c->cd = true;
            break;
        case 0x6e + 0x64: // create new directory 
            c->newd = true;
            break; 
        case 0x70: // print directory contents
            c->printd = true;
            break;
        case 0x72 + 0x64: // remove directory and its contents
            c->rm = true;
            c->cd = true;
            break;
        case 0x71: // quit terminal simulation
            initc(c);
            c->status = false;
            break;
        case 0x72: // read file
            c->read = true;
            break;
        case 0x72 + 0x6D: // remove file
            c->rm = true;
            break;
        case 0x73 + 0x61: // save file to computer
            c->save = true;
            break;
        case 0x77: // write & create file
            c->write = true;
            break;
        case 0x72 + 0x77: // read & write file
            c->read = true;
            c->write = true;
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
    if (instr[0] != '\0') *op = (int)instr[0];
    else return false;
    if (instr[1] != '\0') *op += (int)instr[1];
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
    if (compstrngs(fs->name, fname)) fs = searchf(fs->left, fname);
    else fs = searchf(fs->right, fname);
    return fs;
}

// add file to D->fs Tree
files * addFT(files * fs, char * fname, char * file) {
    if (fs == NULL) {
        fs = malloc(sizeof(files));
        fs->name = malloc(sizeof(char) * strlen(fname));
        strcpy(fs->name, fname);
        fs->f = malloc(sizeof(char) * strlen(file));
        strcpy(fs->f, file);
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
    printf("to finish writing press return\n");

    if (fgets(f, MAXFLEN-1, stdin) == NULL) f[strlen(f+1)] = '\0';

    // add to tree 
    D->fs = addFT(D->fs, fname, f);
    D->len++;
    free(f);

    printf("\n");
}

// read file (print to terminal)
void rfile(files * f) {
    int len = strlen(f->f);
    f->f[len] = '\0';
    for (int i = 0; i < len; i++) {
        if (i % MAXLINE == 0) printf("\n");
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
    f->f[strlen(f->f)-1] = '\0';
    strcat(f->f, buffer);
    free(buffer);

    printf("\n");
}

// save file to computer memory
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

// print directory path
void printP(directory * D) {
    if (D == NULL) return;
    printP(D->parent);
    printf("%s/", D->name);
}

// remove file in tree and update tree
files * removef(files * fs, char * fname) {
    if (fs == NULL) return NULL; // file doesn't exist random case never possible
    if (strcmp(fs->name, fname) == 0) { // file found
        if (fs->left == NULL) { // one child
            files * tmp = fs->right;
            free(fs->f);
            free(fs->name);
            free(fs);
            return tmp;
        } else if (fs->right == NULL) { // one child 
            files * tmp = fs->left;
            free(fs->f);
            free(fs->name);
            free(fs);
            return tmp;
        } else if (fs->left && fs->left){ // two children 
            files * successor = fs->right;
            while (successor->left) successor = successor->left;

            fs->f = strdup(successor->f);
            fs->name = strdup(successor->name);
            free(successor->f);
            free(successor->name);
            free(successor);
            fs->right = removef(fs->right, successor->f);
        }
    } else if (!(compstrngs(fname, fs->name))) fs->left = removef(fs->left, fname);
    else if (compstrngs(fname, fs->name)) fs->right = removef(fs->right, fname);
    return fs;
}

// free directories
void freeD(directory * D) {
    if (D == NULL) return;
    freeD(D->left);
    freeD(D->right);
    freeF(D->fs);
    free(D->name);
    free(D);
}

// free files 
void freeF(files * f) {
    if (f == NULL) return;
    freeF(f->left);
    freeF(f->right);
    free(f->name);
    free(f->f);
    free(f);
}

// search for directory
directory * searchd(directory * D, char * dname) {
    if (D == NULL) return NULL;
    if (strcmp(D->name, dname) == 0) return D;
    directory * l = searchd(D->left, dname);
    if (l != NULL) {
        if (strcmp(l->name, dname) == 0) return l;
    }
    directory * r = searchd(D->right, dname);
    return r;
}

// do the operation
directory * performOp(controls * c, char * fname, directory * D) {
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
        if (fname == NULL) printD(D);
        else if (strcmp(fname, "path") == 0){
            printP(D);
            printf("\n");
        }
    } else if (c->cd && c->rm) {
        if (fname == NULL) printf("no directory named... aborting command\n");
        if (strcmp(D->name, fname) == 0) printf("cannot delete current directory\n");
        else if (strcmp("root", fname) == 0) printf("cannot delete root\n");
        else {
            directory * tmp = searchd(D, fname);
            if (tmp == NULL) printf("directory does not exist\n");
            else {
                if (tmp->parent->left != NULL) {
                    if (strcmp(tmp->parent->left->name, tmp->name) == 0) tmp->parent->left = NULL;
                    else tmp->parent->right = NULL;
                } else tmp->parent->right = NULL;
                freeD(tmp);
            }
        }
    } else if (c->cd && !c->back) {
        if (fname == NULL) {
            printf("no directory named... aborting command\n");
            goto jump;
        }
        if (D->left == NULL && D->right == NULL) {
            printf("directory does not exist\n");
            goto jump;
        }
        if (D->left != NULL) 
            if (strcmp(D->left->name, fname) == 0) {
                D = D->left;
                goto jump;
            }
        if (D->right != NULL) 
            if (strcmp(D->right->name, fname) == 0) {
                D = D->right;
                goto jump;
            }
        printf("directory does not exist\n");
    } else if (c->newd) {
        if (D->left == NULL && D->right == NULL) {
            D->left = initD(D, fname);
            goto jump;
        }
        if (D->left == NULL && D->right != NULL) {
            if (strcmp(D->right->name, fname) == 0) {
                printf("directory is already a child directory\n");
                goto jump;
            } else {
                D->left = initD(D, fname);
                goto jump;
            }
        }
        if (D->left != NULL && D->right == NULL) {
            if (strcmp(D->left->name, fname) == 0) {
                printf("directory is already a child directory\n");
                goto jump;
            } else {
                D->right = initD(D, fname);
                goto jump;
            }
        }
        if  (D->left != NULL && D->right != NULL) {
            printf("no open directory path... cd and start again\n");
            goto jump;
        }
    } else if (c->cd && c->back) {
        if (D->parent == NULL) printf("no parent directory...\n");
        else D = D->parent;
    } else if (c->save) {
        tmp = searchf(D->fs, fname);
        if (tmp == NULL) printf("file does not exist\n");
        else saveF(tmp);
    } else if (c->rm) {
        tmp = searchf(D->fs, fname);
        if (tmp == NULL) printf("file does not exist\n");
        else D->fs = removef(D->fs, fname);
    }

    jump: // jump point

    resetc(c);
    return D;
}