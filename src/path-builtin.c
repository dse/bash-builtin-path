#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "path-builtin.h"

int path_action = PATH_ACTION_NONE;
int path_force = 0;

/**
 * entry point.
 *
 * Returns EXECUTION_{SUCCESS,FAILURE}.
 */
int path_builtin(WORD_LIST* list) {
    int opt;
    reset_internal_getopt();
    while ((opt = internal_getopt(list, "acdf")) != -1) {
        switch (opt) {
        case 'a':
            path_action = PATH_ACTION_APPEND;
            break;
        case 'c':
            path_action = PATH_ACTION_CHECK;
            break;
        case 'd':
            path_action = PATH_ACTION_DELETE;
            break;
        case 'f':
            path_force = 1;
            break;
        CASE_HELPOPT;
        default:
            builtin_usage();
            return EX_USAGE;
        }
    }
    list = loptend;
    return path_builtin_stage1(list);
}

/**
 * after options are processed
 *
 * Returns EXECUTION_{SUCCESS,FAILURE}.
 */
int path_builtin_stage1(WORD_LIST* list) {
    if (list == NULL) {
        builtin_error("no variable name specified");
        return EXECUTION_FAILURE;
    }

    char* varname = list->word->word;
    if (varname == NULL || !*varname) {
        builtin_error("empty variable name specified");
        return EXECUTION_FAILURE;
    }

    list = list->next;
    if (list == NULL) {
        if (path_list_var(varname)) {
            return EXECUTION_SUCCESS;
        }
        return EXECUTION_FAILURE;
    }

    return path_builtin_stage2(varname, list);
}

/**
 * after exceptions are handled
 *
 * Returns EXECUTION_{SUCCESS,FAILURE}.
 */
int path_builtin_stage2(char* varname, WORD_LIST* list) {
    int retval = EXECUTION_SUCCESS;
    int result;
    for (; list != NULL; list = list->next) {
        switch (path_action) {
        case PATH_ACTION_PREPEND:
            result = path_prepend_var(varname, list->word->word);
            break;
        case PATH_ACTION_APPEND:
            result = path_append_var(varname, list->word->word);
            break;
        case PATH_ACTION_CHECK:
            result = path_check_var(varname, list->word->word);
            break;
        case PATH_ACTION_DELETE:
            result = path_delete_var(varname, list->word->word);
            break;
        }
        if (!result) {
            retval = EXECUTION_FAILURE;
        }
    }
    return retval;
}

/**
 * Always returns 1 or 0, to indicate success or failure.
 */
int path_list_var(char* varname) {
    char* value = find_regular_variable_value(varname);
    if (!value) {
        return 0;
    }
    path_list(value);
}

/**
 * Always returns 1 or 0, to indicate success or failure.
 */
int path_check_var(char* varname, char* dir) {
    char* value = find_regular_variable_value(varname);
    if (!value) {
        return 0;
    }
    char* occurrence = path_check(value, dir);
    return occurrence ? 1 : 0;
}

/**
 * Always returns 1 or 0, to indicate success or failure.
 */
int path_prepend_var(char* varname, char* dir) {
}

/**
 * Always returns 1 or 0, to indicate success or failure.
 */
int path_append_var(char* varname, char* dir) {
}

/**
 * Always returns 1 or 0, to indicate success or failure.
 */
int path_delete_var(char* varname, char* dir) {
}

void path_list(char* path) {
    char* colon_ptr;
    while (*path) {
        colon_ptr = strchr(path, ':');
        if (!colon_ptr) {
            puts(path);
            return;
        }
        *colon_ptr = '\0';
        puts(path);
        *colon_ptr = ':';
        path = colon_ptr + 1;
    }
}

/**
 * Always returns a newly allocated string.
 */
char* path_prepend(char* path, char* dir) {
    size_t dir_len = strlen(dir);
    if (path_force) {
        path = path_delete(path, dir); /* new allocation */
    } else {
        path = strdup(path);    /* new allocation */
        if (path_check(path, dir)) {
            return path;
        }
    }
    size_t path_len = strlen(path);
    path = realloc(path, path_len + dir_len + 2); /* dir + ':' + path + '\0' */
    memmove(path + dir_len + 1, path, path_len + 1);
    strcpy(path, dir);
    path[dir_len] = ':';
    return path;
}

/**
 * Always returns a newly allocated string.
 */
char* path_append(char* path, char* dir) {
    if (path_force) {
        path = path_delete(path, dir); /* new allocation */
    } else {
        path = strdup(path);    /* new allocation */
        if (path_check(path, dir)) {
            return path;
        }
    }
    size_t path_len = strlen(path);
    path = realloc(path, path_len + dir_len + 2); /* path + ':' + dir + '\0' */
    path[path_len] = ':';
    strcpy(path + path_len + 1, dir);
    return path;
}

/**
 * Always returns the first occurrence, or NULL.
 */
char* path_check(char* path, char* dir) {
    char* occurrence;
    char* ptr = path;
    size_t len = strlen(dir);
    while (1) {
        occurrence = strstr(ptr, dir);
        if (!occurrence) {
            return NULL;
        }
        if (occurrence != path && *(occurrence - 1) != ':') {
            /* neither anchored to beginning of string nor preceded by ':' */
            ptr = strchr(occurrence + len, ':'); /* find ':' before next path element */
            if (!ptr) {
                return NULL;    /* there isn't a next one */
            }
            ptr += 1; /* skip ':' to go to start of next path element */
            if (!*ptr) {
                return NULL;    /* at end of string */
            }
            continue;           /* there's a next path element; continue search from it */
        }
        ptr = occurrence + len; /* end of occurrence */
        if (*ptr && *ptr != ':') {
            /* neither anchored to end of string nor followedy by ':' */
            ptr = strchr(ptr, ':'); /* find ':' before next path element */
            if (!ptr) {
                return NULL;    /* there isn't a next one */
            }
            ptr += 1; /* skip ':' to go to start of next path element */
            if (!*ptr) {
                return NULL;    /* at end of string */
            }
            continue;           /* there's a next path element; continue search from it */
        }
        return occurrence;      /* found a match */
    }
}

/**
 * Always returns a newly allocated string.
 */
char* path_delete(char* path, char* dir) {
    char* occurrence;
    size_t len = strlen(dir);
    path = strdup(path);
    while ((occurrence = path_check(path, dir)) != NULL) {
        char* end = occurrence + len;
        if (occurrence == path) {
            if (*end == ':') {
                memmove(occurrence, end + 1, strlen(end + 1) + 1); /* 'A:b:c' => 'b:c' */
            } else {
                *occurrence = '\0'; /* 'A' => '' */
            }
        } else {
            if (*end == ':') {
                memmove(occurrence, end + 1, strlen(end + 1) + 1); /* 'x:y:z:A:b:c' => 'x:y:z:b:c' */
            } else {
                *(occurrence - 1) = '\0'; /* 'x:y:z:A' => 'x:y:z' */
            }
        }
    }
    return path;
}

SHELL_VAR* find_regular_variable(char* varname) {
    SHELL_VAR* var = find_variable(varname);
    if (!var) {
        return NULL;
    }
    if (array_p(var)) {
        builtin_error("%s: is an array", varname);
        return NULL;
    }
    if (function_p(var)) {
        builtin_error("%s: is a function", varname);
        return NULL;
    }
    if (integer_p(var)) {
        builtin_error("%s: is an integer", varname);
        return NULL;
    }
    if (assoc_p(var)) {
        builtin_error("%s: is an associative array", varname);
        return NULL;
    }
    return var;
}

SHELL_VAR* find_regular_writable_variable(char* varname) {
    SHELL_VAR* var = find_regular_variable(varname);
    if (!var) {
        return NULL;
    }
    if (readonly_p(var)) {
        return NULL;
    }
    return var;
}

char* find_regular_variable_value(char* varname) {
    SHELL_VAR* var = find_regular_variable(varname);
    if (!var) {
        return NULL;
    }
    char* value = get_variable_value(var);
    if (!value) {
        builtin_error("%s: variable has no value", varname);
        return NULL;
    }
    return value;
}

char* path_doc[] = {
    "Manipulate PATH and similar environment variables.",
    (char*)NULL
};

struct builtin path_struct = {
    "path",                     /* name */
    path_builtin,               /* function */
    BUILTIN_ENABLED,            /* flags */
    path_doc,                   /* long_doc */
    "path [-a] [-c] [-d] [-f] [VARNAME [DIR ...]]", /* short_doc */
    0                                               /* handle, for future use */
};
