#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>

#ifndef FILESYS
#define FILESYS

// user privelages
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
}controls;

// sorted as binary tree
typedef struct files {
    char * f;
    char * name;
    struct files * left;
    struct files * right;
}files;

// sorted in Tree
typedef struct directory {
    char * name;
    int len;
    struct files * fs;
    struct directory * parent;
    struct directory * left;
    struct directory * right;
} directory;

void initc(controls * c); // set all to false
void resetc(controls * c); // reset controls after each command
directory * initD(directory * pD, char * n); // initialize new directory
files * createF(char * fname, char * f); // create file struct
bool setc(unsigned op, controls * c); // set controls from operation
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
directory * performOp(controls * c, char * fname, directory * D); // perform operations
void freeD(directory * D);
void freeF(files * f);



#endif