# Assignment 2 OSP: VSFS Program

VSFS (Very Simple File System) is a program that manages a custom filesystem file, and allows users to performs various tasks to manipulate the file and directories within that filesystem.

## Installation

At the main folder of this program, run the following command to build and compile all the source files into a single excutable file.

```bash
make
```

## Usage

There are multiple options for this program. You can supply a valid .notes file into the parameters, of if you don't have one, the program will automatically create a new, empty .notes file in the path you specify.

- To list all the files in the filesystem
```bash
./VSFS list [filesystem_name]
```

* To copy an external file into the filesystem as an internal file and have its content encoded with base64
```bash
./VSFS copyin [filesystem_name] [external_file_name] [internal_file_name]
```

* To copy an internal file within the filesystem to external file EF and have its content decoded with base64
```bash
./VSFS copyout [filesystem_name] [internal_file_name] [external_file_name]  
```

* To creates an empty internal directory in the filesystem
```bash
./VSFS mkdir [filesystem_name] [internal_dir_name]
```

* To remove an internal file from the filesystem
```bash
./VSFS rm [filesystem_name] [internal_file_name]
```

* To remove an internal directory from the filesystem
```bash
./VSFS rm [filesystem_name] [internal_dir_name]
```

* To defragment and sort out the filesystem
```bash
./VSFS defrag [filesystem_name]
```

* To create an index file of the filesystem and list all the indexes out
```bash
./VSFS index [filesystem_name]
```

## Clean up

If you wish to remove the program and all of its object files, run this command:

```bash
make clean
```

## Additional notes

By default, the copyin and copyout will encode and decode data of the external file, respectively, in order to make sure the program can store and preserve the content of all types of files, including text files or binary files, etc... The result is when you open the filesystem file with a note editor, the file content will be encoded using base64 algorithm. If you wish to just have the content of the external file in plain and orignal format, you can run these two commands, but be aware that the content of binary files may get corrupted and missing:

* To copy an external file into the filesystem as an internal file in plain format
```bash
./VSFS copyin_plain [filesystem_name] [external_file_name] [internal_file_name]
```

* To copy an internal file within the filesystem to external file EF in plain format
```bash
./VSFS copyout_plain [filesystem_name] [internal_file_name] [external_file_name]  
```











