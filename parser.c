//
//  shell.c
//  
//
//  Created by Andrea Balasso on 08/05/18.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int MAXLEN=50;  //maxlen di default
int CODE=0;     //code di default, 0=true 1=false
int errorcount=0;

FILE *outfile;
FILE *errfile;
FILE *tempfile;

//CONTROLLA ARGOMENTI PASSATI DA RIGA DI COMANDO AL LANCIO DEL PROGRAMMA
void getfile(int argc, char **argv) {
    int ncom=1;
    
    int out=0;  //verifico se ho gia' ricevuto un file di output
    int err=0;  //verifico se ho gia' ricevuto un file di errore
    int tmp=0;  //verifico se ho gia' ricevuto un file template
    int mln=0;  //verifico se ho gia' ricevuto maxlen
    int cod=0;  //verifico se ho gia' ricevuto code true/false
    
    //ciclo finche' argomenti non sono finiti o viene rilevato errore
    while(ncom<argc && errorcount==0) {
        
        //controllo che il comando sia nella forma - o --
        if(argv[ncom][0]!='-') {
            printf("\n---Errore, comando non valido---\n\n");
            errorcount=1;
        }
        else {
            //comandi forma -
            if(argv[ncom][1]!='-') {
                //controllo che dopo il comando ci sia un altro argomento
                if(argc<ncom+2) {
                    printf("\n---Errore, numero argomenti non valido---\n\n");
                    errorcount=1;
                }
                else {
                    char *fil = malloc(strlen(argv[ncom+1])*sizeof(char));
                    //comando -o
                    if(strcmp(argv[ncom],"-o")==0) {
                        //controllo se file di output e' gia' stato fornito
                        if(out==1) {
                            printf("\n---Errore, output file gia' fornito---\n\n");
                            errorcount=1;
                        }
                        else {
                                strcpy(fil,argv[ncom+1]);
                                outfile=fopen(fil,"w");
                                out=1;
                        }
                    }
                    //comando -e
                    else if(strcmp(argv[ncom],"-e")==0) {
                        //controllo se file di errore e' gia' stato fornito
                        if(err==1) {
                            printf("\n---Errore, error file gia' fornito---\n\n");
                            errorcount=1;
                        }
                        else {
                                strcpy(fil,argv[ncom+1]);
                                errfile=fopen(fil,"w");
                                err=1;
                        }
                    }
                    //comando -t
                    else if(strcmp(argv[ncom],"-t")==0) {
                        //controllo se file template e' gia' stato fornito
                        if(tmp==1) {
                            printf("\n---Errore, template file gia' fornito---\n\n");
                            errorcount=1;
                        }
                        else {
                                strcpy(fil,argv[ncom+1]);
                            if((tempfile=fopen(fil,"r"))!=NULL)
                                tmp=1;
                            else {
                                printf("\n---Errore, template file non esistente---\n\n");
                                errorcount=1;
                            }
                        }
                    }
                    //comando -m
                    else if(strcmp(argv[ncom],"-m")==0) {
                        //controllo se maxlen e' gia' stato fornito
                        if(mln==1) {
                            printf("\n---Errore, maxlen gia' fornito---\n\n");
                            errorcount=1;
                        }
                        else {
                                int a;
                                a = atoi(argv[ncom+1]);
                                //controllo che il numero fornito dall'utente non sia assurdo
                                if(a<=0) {
                                    printf("\n---Errore, numero non valido---\n\n");
                                    errorcount=1;
                                }
                                else
                                    MAXLEN = a;
                                mln=1;
                        }
                    }
                     //comando -c
                    else if(strcmp(argv[ncom],"-c")==0) {
                        //controllo se code e' gia' stato fornito
                        if(cod==1) {
                            printf("\n---Errore, code gia' fornito---\n\n");
                            errorcount=1;
                        }
                        else {
                                //code true
                                if(strcmp(argv[ncom+1],"true")==0 || strcmp(argv[ncom+1],"TRUE")==0 || strcmp(argv[ncom+1],"True")==0) {
                                    CODE=0;
                                    cod=1;
                                }
                                //code false
                                else if(strcmp(argv[ncom+1],"false")==0 || strcmp(argv[ncom+1],"false")==0 || strcmp(argv[ncom+1],"False")==0) {
                                    CODE=1;
                                    cod=1;
                                }
                                //code non riconosciuto
                                else {
                                    printf("\n---Errore, codice di ritorno non valido---\n\n");
                                    errorcount=1;
                                }
                        }
                    }
                    else {
                        //altrimenti ritorno errore
                        printf("\n---Errore, comando non valido---\n\n");
                        errorcount=1;
                    }
                    free(fil);
                }
                ncom=ncom+2;
            }
            //comandi forma --
            else {
                int contachar = 0;
                do{
                    contachar++;
                }while(contachar<9 && argv[ncom][contachar]!='=');
                
                //se non vi e' assegnazione con =, comando non valido
                if(argv[ncom][contachar]!='=') {
                    printf("\n---Errore, comando non valido---\n\n");
                    errorcount=1;
                }
                else {
                    int size=strlen(argv[ncom])-contachar-1;
                    int i=0;
                    char *comand = malloc(contachar*sizeof(char));
                    char *fil = malloc(size*sizeof(char));
                    do {
                        comand[i]=argv[ncom][i];
                        i++;
                    }while(i<contachar);
                    contachar+=1;
                    
                    for(int j=0; j<size; j++) {
                        fil[j]=argv[ncom][j+contachar];
                    }

                    //comando --outfile=
                    if(strcmp(comand,"--outfile")==0) {
                        //controllo se file di output e' gia' stato fornito
                        if(out==1) {
                            printf("\n---Errore, output file gia' fornito---\n\n");
                            errorcount=1;
                        }
                        else {
                            outfile=fopen(fil,"w");
                            out=1;
                        }
                    }
                    //comando --errfile=
                    else if(strcmp(comand,"--errfile")==0) {
                        if(err==1) {
                            printf("\n---Errore, error file gia' fornito---\n\n");
                            errorcount=1;
                        }
                        else {
                            errfile=fopen(fil,"w");
                            err=1;
                        }
                    }
                    else if(strcmp(comand,"--tmpfile")==0) {
                        if(tmp==1) {
                            printf("\n---Errore, template file gia' fornito---\n\n");
                            errorcount=1;
                        }
                        else {
                            tempfile=fopen(fil,"w");
                            tmp=1;
                        }
                    }
                    else if(strcmp(comand,"--maxlen")==0) {
                        if(mln==1) {
                            printf("\n---Errore, maxlen gia' fornito---\n\n");
                            errorcount=1;
                        }
                        else {
                            int a;
                            a = atoi(fil);
                            //controllo che il numero fornito dall'utente non sia assurdo
                            if(a<=0) {
                                printf("\n---Errore, numero non valido---\n\n");
                                errorcount=1;
                            }
                            else
                                MAXLEN = a;
                            mln=1;
                        }
                    }
                    else if(strcmp(comand,"--code")==0) {
                        if(cod==1) {
                            printf("\n---Errore, code gia' fornito---\n\n");
                            errorcount=1;
                        }
                        else {
                            if(strcmp(fil,"true")==0 || strcmp(fil,"TRUE")==0 || strcmp(fil,"True")==0) {
                                CODE=0;
                                cod=1;
                            }
                            else if(strcmp(fil,"false")==0 || strcmp(fil,"false")==0 || strcmp(fil,"False")==0) {
                                CODE=1;
                                cod=1;
                            }
                            else {
                                printf("\n---Errore, codice di ritorno non valido---\n\n");
                                errorcount=1;
                            }
                        }
                    }
                    //comando non riconosciuto
                    else {
                        printf("\n---Errore, comando non valido---\n\n");
                    }
                    free(fil);
                    free(comand);
                }
                ncom=ncom+1;
            }
        }
    }
    
    if(errorcount!=0) {
        printf("Assicurarsi che la sintassi sia nella forma:\n");
        printf("\tfile output:\t -o FILENAME\t| --outfile=FILENAME\n");
        printf("\tfile error:\t -e FILENAME\t| --errfile=FILENAME\n");
        printf("\tfile template:   -t FILENAME\t| --tmpfile=FILENAME\n");
        printf("\tmaxlen:\t\t -m POS_INT\t| --maxlen=POS_INT\n");
        printf("\tcode:\t\t -c TRUE/FALSE\t| --code=TRUE/FALSE\n");
        printf("\n---Esempio di esecuzione---\n");
        printf("./shell --maxlen=100 -o \"output.txt\" --errfile=\"pippo.txt\" -t \"newtemp.txt\"\n\n");
        printf("Non immettere la stessa richiesta piu' di una volta\n\n");
    }
    else {
        if(out==0) {
            //default se non specificato
            outfile=fopen("outputfile.txt","w");
        }
        if(err==0) {
            //default se non specificato
            errfile=fopen("errorfile.txt","w");
        }
        if(tmp==0) {
            //default se non specificato
            tempfile=fopen("templatefile.txt","r");
        }
    }
    return;
}
