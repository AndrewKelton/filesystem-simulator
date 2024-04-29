Filesystem Simulator in C
-
- This code simulates a file system in a computer through the command line.
- Implemented in this code is a library called filesys.h created by me.

ABOUT filesys.h
-
- filesys.h is the library for accessing, creating, writing, reading, and saving files.
- Data structures used in filesys.h:
  - Structs used for controls, directories, and files in directories. Controls are used to controls the flow of data control which functions are used for operations. Directories are implemented with a binary tree. Files are implemented using a binary search tree, comparing their values of characters with previously inputted files.
  
ABOUT filesys.c
-
- filesys.c is the actual c code that implements the functions from filsys.h library.

ABOUT main.c
-
- main.c implements the interaction between the user and the filesystem. main.c partitions the lines of the user's to grab the command and the file/directory it is directed to

HOW TO USE
-
1. Download files
2. gcc -o m main.c sys.c  # to compile
3. ./m # to run
- Supported Commands
  | Command | Description |
  | ------- | ----------- |
  | s       | login       |
  | sr      | root login  |
  | w       | write/create file            |
  | r        |  read file           |
  | rw        | read/write to file            |
  | sa        |  save file to computer           |
  | p        | print all files in directory            |
  | cd        | change directory            |
  | nd        | create new directory            |
  | bd        | change to parent directory            |
  | q        |  quit           |
  
- If trying to use any command other than s, sr, bd, or q, you must use '- ' to access the file or directory you want. Without using '- ' you will either encounter 'command not found' or you will not be able to access the file/directory or create a file/directory missing the first letter.
- Why do you need '- ' only when accessing files?
  - '-' will tell the program that is the end of the command. If the command doesn't need a '-' the program knows this because it has already reached the end of the line in the partition. The ' ' after the '-' is from the program moving onto the next 2 indexes beyond the command. It also looks better when using the filesystem.
- Example
```
cmd: s


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
- Deleting files and directories without quitting the program
- Adding specific root/user privileges
- Add passwords to root/user
- Change between accounts and privilege accesses
- Interact with actual computer filesystem
- Specifc interface for the filesystem beyond the terminal
