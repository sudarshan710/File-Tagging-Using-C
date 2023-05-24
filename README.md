# File-Tagging-Using-C

Project to implement file tagging to files using library functions, headers and C language. Using this user can add specific tags to files, 
which will eventually be stored in a filename.ext.tags (if file is filename.ext) file. We create and compile a shared library to for the 
header file to run operations required for this process.

## Operations 
* Create, read, write files
* Add, search, remove tags

## How to run?


```sh
export LD_LIBRARY_PATH="path-to-your-functions-c-file:$LD_LIBRARY_PATH"
```

This will also be the path where the shared library will be compiled and can be referred during execution of main program.


```sh
gcc -shared -o libapi.so libapi.c
```

libapi.so is the shared that will be compiled after running above command. It will compile the functions/operations from libapi.c file as
a shared libary.

```sh
gcc -o program.exe program.c -L"path-to-your-library" -lapi
```

Above command generates an executable (program.exe) for program.c main program. The executable will run program.c which will prompt user with
choices to perform various operations.

```sh 
./program.exe
```

Above command will execute program.c file and will wait for user input.

## Note

You will need to re-compile your shared library every time you make changes to functions/operations in your libapi.c, which contains all the
logic to perform the operations.


