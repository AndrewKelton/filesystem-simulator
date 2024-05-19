#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>
#include<unistd.h>

#ifndef FILESYS
#define FILESYS

// user actions
typedef struct {
    bool root;
    bool allow;
    bool read;
    bool write;
    bool printd;
    bool cd;
    bool back;
    bool newd;
    bool save;
    bool rm;
    bool status; // status of terminal
} controls;

// users names as Trie
typedef struct usern {
    struct usern * let[26];
    bool flag;
} usern;

// sorted as binary tree
typedef struct files {
    char * f;
    char * name;
    struct files * left;
    struct files * right;
} files;

// directory general tree structure
typedef struct directory {
    char * name;
    int len;
    struct files * fs;
    struct directory * parent;
    struct directory ** child;
    bool full;
} directory;

// user login control
typedef struct user {
    directory * d;
    char * name;
    struct user * next;
} user;

/* functions to control filesystem */

void initc(controls * c); // set all controls to false
void resetc(controls * c); // reset controls after each command
void addU(usern * users, char * name); // add user to users
bool ufetch(usern * users, char * name); // user fetch
bool rootcheck(char * name); // check if user is root
int hashD(int a); // create hash value for directory 
int qProbe(int a, int i); // quadratic probe to get new hash idx
int checkChild(directory * d, int a, int i); // recursively get hash idx
directory * userd(user * u, char * n); // grab user directory
directory * userRootDirectoryInit(directory * d); // initialize root's user directory
directory * rootDinit(user * u); // enable root access to user directories
directory * disableR(user * u); // disable root accesses in directory
directory * initD(directory * pD, char * n); // initialize new directory
void addULL(user * u, char * n); // add new user to user linked list from head->next (root always in front)
void addUserstoRootD(user * u); // not used
int numUsers(user * u); // return number of users
bool isFull(directory * d); // check if directory is full
void resetRootD(user * u); // user directories in root users get parent status to NULL
files * createF(char * fname, char * f); // create file struct
bool setc(unsigned op, controls * c, user * u, usern * names, char * name /* file or name of user */); // set controls from operation
bool opfetch(char * instr, unsigned * op); // get operation in hex
bool compstrngs(char * n1, char * n2); // compare strings
files * searchf(files * fs, char * fname); // search for file in directory
files * addFT(files * fs, char * fname, char * f); // add file to directory
void wfile(char * fname, directory * D); //write and create file
void rfile(files * f); // read file print to terminal
void rwfile(files * f); // read and append to file
void printFT(files * f); // print files in directory
void saveF(files * tmp); // save file to computer
void printP(directory * D); // print directory path
files * removef(files * fs, char * fname); // delete file & its contents
void helpP(); // print all commands
directory * performOp(unsigned op, controls * c, char * fname /* file name or name of user */, directory * D, user * u); // perform operations
void freeU(user * u); // free all contents of user
void freeD(directory * D); // free all directory contents and directory
void freeF(files * f); // free all file contents and file
void freeNames(usern * unames); // free names in trie



#endif