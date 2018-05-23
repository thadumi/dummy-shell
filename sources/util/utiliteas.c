//
// Created by thadumi on 5/23/18.
//

#include "../../headers/util/utiliteas.h"
#include <stdlib.h>

int strcr(char *str, char orig, char rep) {
    char *ix = str;
    int n = 0;
    while((ix = strchr(ix, orig)) != NULL) {
        *ix++ = rep;
        n++;
    }
    return n;
}



char *strsr(char *str, char *orig, char *rep) {
    char *result; // the return string
    char *ins;    // the next insert point
    char *tmp;    // varies
    int len_orig;  // length of orig (the string to remove)
    int len_rep; // length of rep (the string to replace orig rep)
    int len_front; // distance between orig and end of last orig
    int count;    // number of origlacements

    // sanity checks and initialization
    if (!str || !orig)
        return NULL;
    len_orig = strlen(orig);
    if (len_orig == 0)
        return NULL; // empty orig causes infinite loop during count
    if (!rep)
        rep = "";
    len_rep = strlen(rep);

    // count the number of replacements needed
    ins = str;
    for (count = 0; tmp = strstr(ins, orig); ++count) {
        ins = tmp + len_orig;
    }

    tmp = result = malloc(strlen(str) + (len_rep - len_orig) * count + 1);

    if (!result)
        return NULL;

    // first time through the loop, all the variable are set correctly
    // from here on,
    //    tmp points to the end of the result string
    //    ins points to the next occurrence of orig in str
    //    str points to the remainder of str after "end of orig"
    while (count--) {
        ins = strstr(str, orig);
        len_front = ins - str;
        tmp = strncpy(tmp, str, len_front) + len_front;
        tmp = strcpy(tmp, rep) + len_rep;
        str += len_front + len_orig; // move to next "end of orig"
    }
    strcpy(tmp, str);
    return result;
}