//
// Created by thadumi on 5/6/18.
//

#ifndef SHELL_UTILITEAS_H
#define SHELL_UTILITEAS_H

#include <string.h>

#define FALSE 0
#define TRUE !FALSE

typedef char bool;

#define LAMBDA(c_) ({ c_ _;})


int strcr(char *str, char orig, char rep);
char *strsr(char *str, char *orig, char *rep);
char *trimwhitespace(char *str);

#endif //SHELL_UTILITEAS_H
