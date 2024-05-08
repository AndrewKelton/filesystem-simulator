Filesystem Simulator in C
-
- This code simulates a file system in a computer through the command line.
- Implemented in this code is a library called filesys.h created by me.

Installation 
-
1. Download C files and header file
2. Compile in terminal using ```gcc -o m main.c sys.c```
3. Run with ```./m```
4. Happy filing :)

ABOUT filesys.h
-
- filesys.h is the header for accessing, creating, writing, reading, and saving files.
- Data structures used in filesys.h:
  - Dynamically Allocated Structs (all structs including controls struct)
  - Linked Lists (user's data/directories)
  - Binary Search Trees (files in each directory)
  - Binary Trees (directories)
  - Tries (user's names)
  
ABOUT filesys.c
-
- filesys.c is the actual c code that implements the functions from filsys.h library.

ABOUT main.c
-
- main.c implements the interaction between the user and the filesystem. main.c partitions the lines of the user's to grab the command and the file/directory it is directed to
- main.c will check if the second field in your command is NULL based on the appropriate command

Supported Commands
-
| Command | Action |
| ------- | ----------- |
| s-       | login       | 
| nu-      | create new user |
| lo       | logout user |
| w-       | write/create file            |
| r-        |  read file           |
| rw-        | read/write to file            |
| rm-        | remove/delete file            |
| sa-        |  save file to computer           |
| p        | print all files in directory            |
| p- path  | print path to current directory |
| pa       | print child directories under directory |
| cd-        | change directory            |
| nd-        | create new directory            |
| rd-        | remove/delete directory and all contents
| bd        | change to parent directory            |
| q        |  quit           |

- Why do you need '- ' when accessing files and using any input other than the operation command?
  - '-' will tell the program that is the end of the command. If the command doesn't need a '-' the program knows this because it has already reached the end of the line in the partition. Commands are partitioned by the first two indexes of line inputted by the user. Everything after the first two indexes is assumed '- ', '\0', or '\n'. The ' ' after the '-' is from the program moving onto the next 2 indexes beyond the command. It also looks better when using the filesystem.
- Example
```
cmd: s- root


cmd: w- file

HELLO
to finish writing press return
hello


cmd: r- file


hello


cmd: q
```

To Be Added
-
- Add passwords to root/user
- Specifc interface for the filesystem beyond the terminal
