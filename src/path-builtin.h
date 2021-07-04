#ifndef PATH_BUILTIN_H
#define PATH_BUILTIN_H

#include <stdio.h>

#include "config.h"
#include "builtins.h"
#include "shell.h"
#include "common.h"
#include "bashgetopt.h"

#define PATH_ACTION_NONE    0
#define PATH_ACTION_PREPEND 1
#define PATH_ACTION_APPEND  2
#define PATH_ACTION_CHECK   3
#define PATH_ACTION_DELETE  4

extern int path_action;
extern int path_force;

int path_builtin(WORD_LIST*);
int path_builtin_stage1(WORD_LIST*);
int path_builtin_stage2(char*, WORD_LIST*);

int path_list_var(char*);
int path_prepend_var(char*, char*);
int path_append_var(char*, char*);
int path_check_var(char*, char*);
int path_delete_var(char*, char*);

char* path_prepend(char*, char*);
char* path_append(char*, char*);
char* path_check(char*, char*);
char* path_delete(char*, char*);
void path_list(char*);

SHELL_VAR* find_regular_variable(char*);
SHELL_VAR* find_regular_writable_variable(char*);
char* find_regular_variable_value(char*);


#endif
