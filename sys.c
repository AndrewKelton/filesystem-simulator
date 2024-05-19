#include "filesys.h"
#define MAX 99999999
#define MAXLINE 80
#define MAXFLEN 1000
#define DNUM 97

int count = 0;

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

// add new user to users
void addU(usern * users, char * name) {
    for (int i = 0; i < strlen(name); i++) {
        if (users->let[name[i] - 'a'] == NULL) users->let[name[i]- 'a'] = malloc(sizeof(usern));
        users = users->let[name[i] - 'a'];
        users->flag = false;
    }
    users->flag = true;
}

// user fetch/login
bool ufetch(usern * names, char * name) {
    usern * tmp = names;
    for (int i = 0; i < strlen(name); i++) {
        if (tmp == NULL) return false;
        tmp = tmp->let[name[i] - 'a'];
    }
    return tmp->flag;
}

// check root login
bool rootcheck(char * name) {
    if (strcmp("root", name) == 0) return true;
    return false;
}

// hash value 
int hashD(int a) {
    return a % DNUM;
}

// quadratic probing
int qProbe(int a, int i) {
    return a + (i * i);
}

// get child hash value
int checkChild(directory * d, int a, int i) {
    if (d->full == true) return -1;
    int b = hashD(a);
    if (d->child[b] == NULL) {
       return b;
    }
    a = qProbe(a, i);
    return checkChild(d, a, i+1);
}

// locate user in user list return user directory
directory * userd(user * u, char * n) {
    if (u == NULL) return NULL;
    if (strcmp(u->name, n) == 0) return u->d;
    return userd(u->next, n);
}

// initialize users directory in root
directory * userRootDirectoryInit(directory * d) {
    d->child[0] = malloc(sizeof(directory));
    d->child[0]->full = false;
    d->child[0]->parent = d;
    d->child[0]->len = 0;
    d->child[0]->name = strdup("users");
    d->child[0]->child = malloc(sizeof(directory *) * DNUM);
    d->child[0]->fs = NULL;
    return d;
}

// initialize root directory to access users and their files
directory * rootDinit(user * u) {
    directory * rootd = u->d;
    userRootDirectoryInit(rootd);
    user * tmpu = u->next;
    int i = 0;
    while (tmpu != NULL) {
        rootd->child[0]->child[i] = tmpu->d;
        rootd->child[0]->child[i++]->parent = rootd->child[0];
        tmpu = tmpu->next;
    }
    return rootd;
}

// check if user directory still connects to root directory take away access
directory * disableR(user * u) {
    directory * ud = u->d;
    if (ud->parent != NULL) {
        ud->parent = NULL;
    }
    return ud;
}

// initialize directory
directory * initD(directory * pD, char * n) {
    directory * D = malloc(sizeof(directory));
    D->fs = NULL;
    D->full = false;
    D->name =  strdup(n);
    D->len = 0;
    D->parent = pD;
    D->child = malloc(sizeof(directory *) * DNUM);
    return D;
}

// add new directory
directory * addtoD(directory * pD, char * name) {
    int h = checkChild(pD, atoi(name), 1);
    pD->child[h] = initD(pD, name);
    return pD;
}

// add user to linked list
void addULL(user * u, char * n) {
    user * tmp = malloc(sizeof(user));
    tmp->name = strdup(n);
    tmp->next = u->next;
    u->next = tmp;
    tmp->d = initD(NULL, n);
}

// add user directories to root directory
void addUserstoRootD(user * u) {
    directory * rootd = u->d;
    int i = 0;
    while(u->next != NULL) {
        u = u->next;
        rootd->child[0]->child[i] = u->d;
    }
}

// get number of users
int numUsers(user * u) {
    if (u == NULL) return 0;
    return 1 + numUsers(u->next);
}

// check if directory is full
bool isFull(directory * d) {
    for (int i = 0; i < DNUM; i++) {
        if (d->child[i] == NULL) return false;
    }
    return true;
}

// reset root user directories
void resetRootD(user * u) {
    int len = numUsers(u);
    for (int i = 0; i < len; i++) {
        u->d->child[0]->child[i] = NULL;
    }
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
bool setc(unsigned op, controls * c, user * u, usern * names, char * name /*file or name of user*/) {
    switch(op) {
        case 0x62 + 0x64: // to parent directory
            c->cd = true;
            c->back = true;
            break;
        case 0x63 + 0x64: // change directory
            c->cd = true;
            break;
        case 0x68: // print all commands
            break;
        case 0x6c + 0x6f: // logout user
            c->allow = false;
            c->root = false;
            break;
        case 0x6e + 0x64: // create new directory 
            c->newd = true;
            break; 
        case 0x70: // print directory contents
            c->printd = true;
            break;
        case 0x70 + 0x61: // print all child directories in directory
            c->printd = true;
            c->read = true;
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
        /* need to do this here otherwise unauthorized access 
             can be gained or controls will never be true */
        case 0x73: // sign in
            if (c->allow || c->root) { // already signed in
                printf("\nalready signed in... sign out to change user");
                break;
            }
            if (name == NULL) break;
            if (ufetch(names, name)) { // check for user
                c->allow = true;
            } else if (rootcheck(name)) {  // check for root
                c->allow = true;
                c->root = true;
            } else {
                printf("\nuser not found\n"); // not a valid user
                return false;
                break;
            }
            printf("\nlogged in as %s", name);
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

// print directory
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

// print child directories under directory
void printDd(directory * d) {
    if (d->child == NULL) return;
    for (int i = 0; i < DNUM; i++) {
        if (d->child[i] != NULL) {
            printf("%s\t", d->child[i]->name);
        }
    }
    printf("\n");
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
    if (fs == NULL) return NULL; // file doesn't exist random case never possible to be true
    if (strcmp(fs->name, fname) == 0) { // file found
        if (fs->left == NULL) { // one child right
            files * tmp = fs->right;
            free(fs->f);
            free(fs->name);
            free(fs);
            return tmp;
        } else if (fs->right == NULL) { // one child left
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
    if (D->child != NULL) {
        for (int i = 0; i < DNUM; i++) {
            freeD(D->child[i]);
        }
    }
    freeF(D->fs);
    free(D->name);
    free(D->child);
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

// free user and user data from user linked list
void freeU(user * u) {
    if (u == NULL) return;
    user * tmp = u->next;
    freeD(u->d);
    free(u->name);
    free(u);
    freeU(tmp);
}

// free user's names from trie
void freeNames(usern * unames) {
    if (unames == NULL) return;
    for (int i = 0; i < 26; i++) {
        freeNames(unames->let[i]);
    }
    free(unames);
}

// search for directory index
int searchd(directory * D, char * dname) {
    if (D == NULL) return -1;
    int a = atoi(dname);
    a = hashD(a);
    if (D->child[a] == NULL) return -1;
    if (strcmp(D->child[a]->name, dname) == 0) return a;
    int i = 1;
    int h = qProbe(a, i);
    i++;
    while (count != DNUM) {
        h = hashD(h);
        count++;
        if (D->child[h] != NULL) {
            if (strcmp(D->child[h]->name, dname) == 0) {
                count = 0;
                return h;
            }
        }
        h = qProbe(h, i);
        i++;
    }
    count = 0;
    return -1;
}

// help function to print all commands
void helpP() {
    printf("******************************************************\n\n");
    printf("...all commands containing '-' require a ' ' before inputting destination/user...\n");
    printf("...all commands require lowercase input, files may be anything...\n\n");
    printf("-----------------------commands-----------------------\n");
    printf("s- \t|\t login\n");
    printf("nu- \t|\t create new user\n");
    printf("lo \t|\t logout\n");
    printf("w- \t|\t write/create file\n");
    printf("r- \t|\t read file\n");
    printf("rw- \t|\t read file and write to it (starting from same line)\n");
    printf("rm- \t|\t remove/delete file\n");
    printf("sa- \t|\t save file to computer memory\n");
    printf("p \t|\t print all files in current directory\n");
    printf("p- path |\t print path to current directory\n");
    printf("pa \t|\t print all child directories under current directory\n");
    printf("cd- \t|\t change directory to destination\n");
    printf("nd- \t|\t create new directory\n");
    printf("rd- \t|\t remove/delete directory and its contents\n");
    printf("bd \t|\t change to parent directory\n");
    printf("q \t|\t quit program and free all memory\n");
    printf("h \t|\t print all commands... what you just did\n\n");
    printf("******************************************************\n");
}

// do the operation
directory * performOp(unsigned op, controls * c, char * fname/* file name or name of user */, directory * D, user * u) {
    files * tmp;
    
    D->full = isFull(D);

    if (op == 0x73) {
        if (fname == NULL) {
            printf("no user inputted\n");
            return NULL;
        }
        D = userd(u, fname);
        if (!c->root) {
            if (D->parent != NULL) {
                D->parent = NULL;
            }
        } else {
            D = rootDinit(u);
        }
        return D;
    }

    if (c->read && c->printd) {
        printDd(D);

    } else if (c->write && !c->read) {
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
            int idx = searchd(D, fname);
            if (idx == -1) printf("directory does not exist\n");
            else {
                freeD(D->child[idx]);
                D->child[idx] = NULL;
                D->full = false;
            }
        }

    } else if (c->cd && !c->back) {
        if (fname == NULL) {
            printf("no directory named... aborting command\n");
            goto jump;
        }
        int idx = searchd(D, fname);
        if (idx == -1) {
            printf("directory does not exist\n");
        } else {
            D = D->child[idx];
        }
        
    } else if (c->newd) {
        if (searchd(D, fname) != -1) {
            printf("directory is already a child directory\n");
            goto jump;
        } 
        int h = checkChild(D, atoi(fname), 1);
        if (D->full) {
            printf("no open directory path... cd and start again\n");
        } else {
            D->child[h] = initD(D, fname);
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