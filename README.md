# Assignment 2 OSP: VSFS Program

VSFS (Very Simple File System) is a program that manages a custom filesystem, and allows users to performs various tasks to manipulate the file and directories within that filesystem.

## Installation

At the main folder of this program, run the following command to build and compile all the source files into a single excutable file.

```bash
make
```

## Usage

There are multiple options for this program. You can supply a valid .notes file into the parameters, of if you don't have one, the program will automatically create a new, empty .notes file in the path you specify

- To list all the files in the filesystem
```bash
./VSFS list [filesystem_name]
```

* To copy an external file into the filesystem as an internal file
```bash
./VSFS list [filesystem_name] [external_file_name] [internal_file_name]
```



