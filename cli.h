#pragma once

#include "filesystem.h"
#include "inode.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
touch [name] [size]   DONE
mkdir [name]          DONE
pwd                   DONE
ls                    DONE
cd [name]             BUGGY
rm [name]             LIMITED
rmdir [name]          LIMITED
exit                  DONE
write [file] [data]
read [file]
chmod [file] [flags]
*/

void run_cli_sim(FileSystem* fs);
void redirect_func_call(FileSystem* fs, char* line, int size, int type);
void touch(FileSystem* fs, char* name, int size);
void mkdir(FileSystem* fs, char* name);
void pwd(FileSystem* fs);
void ls(FileSystem* fs);
void cd(FileSystem* fs, char* name);
void rm(FileSystem* fs, char* name);
void rmdir(FileSystem* fs, char* name);