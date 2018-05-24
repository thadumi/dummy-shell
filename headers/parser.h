//
//  parser.h
//  
//
//  Created by Andrea Balasso on 22/05/18.
//

#ifndef parser_h
#define parser_h

int CODE=0;

FILE *outputfile;
FILE *errorfile;
FILE *templatefile;

char *defout="outputfile.txt";
char *deferr="errputfile.txt";
char *deftmp="templatefile.txt";
char *defaultmode="w";

int richiesta_int(int a);
char * richiesta_file(char *file, char *richiesta, char *corrente);
void getfile(int argc, char **argv);

#endif /* parser_h */
