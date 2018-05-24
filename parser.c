//
//  shell.c
//  
//
//  Created by Andrea Balasso on 08/05/18.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "headers/config.h"
#include "headers/parser.h"

int errorcount=0;

char *currentout;
char *currenterr;
char *currenttmp;
char *currentmode;

//CONTROLLA ARGOMENTI PASSATI DA RIGA DI COMANDO AL LANCIO DEL PROGRAMMA
void getfile(int argc, char **argv) {
    
    char *buffer;
    size_t bufsize = 32;
    size_t characters;
    
    int ncom=1;
    
    int out=0;  //verifico se ho gia' ricevuto un file di output
    int err=0;  //verifico se ho gia' ricevuto un file di errore
    int tmp=0;  //verifico se ho gia' ricevuto un file template
    int mln=0;  //verifico se ho gia' ricevuto maxlen
    int cod=0;  //verifico se ho gia' ricevuto code true/false
    int mod=0;  //verifico se ho gia' ricevuto modalita' di apertura
    
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
                            char *nuovo = malloc(strlen(currentout)*sizeof(char));
                            strcpy(nuovo,currentout);
                            nuovo = richiesta_file(argv[ncom+1],"File di Output",nuovo);
                            free(currentout);
                            currentout = malloc(strlen(nuovo)*sizeof(char));
                            strcpy(currentout,nuovo);
                        }
                        else {
                            free(currentout);
                            currentout = malloc(strlen(argv[ncom+1])*sizeof(char));
                            strcpy(currentout,argv[ncom+1]);
                            out=1;
                        }
                    }
                    
                    //comando -e
                    else if(strcmp(argv[ncom],"-e")==0) {
                        //controllo se file di errore e' gia' stato fornito
                        if(err==1) {
                            char *nuovo = malloc(strlen(currenterr)*sizeof(char));
                            strcpy(nuovo,currenterr);
                            nuovo = richiesta_file(argv[ncom+1],"File di Errore",nuovo);
                            free(currenterr);
                            currenterr = malloc(strlen(nuovo)*sizeof(char));
                            strcpy(currenterr,nuovo);
                        }
                        else {
                            //free(currenterr);
                            currenterr = malloc(strlen(argv[ncom+1])*sizeof(char));
                            strcpy(currenterr,argv[ncom+1]);
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
                            currenttmp = malloc(strlen(argv[ncom+1])*sizeof(char));
                            strcpy(currenttmp,argv[ncom+1]);
                            if((templatefile=fopen(currenttmp,"r"))!=NULL)
                                tmp=1;
                            else {
                                printf("\n---Errore, template file non esistente---\n");
                                printf("Se desideri usare il template file di default\tpremi [s]\n");
                                characters = getline(&buffer,&bufsize,stdin);
                                if(strcmp(buffer,"s\n")!=0 || strcmp(buffer,"S\n")!=0) {
                                    errorcount=1;
                                    printf("Il programma verra' terminato\n");
                                }
                            }
                        }
                    }

                    //comando -m
                    else if(strcmp(argv[ncom],"-m")==0) {
                        //controllo se maxlen e' gia' stato fornito
                        int a;
                        a = atoi(argv[ncom+1]);
                        if(mln==1) {
                            MAXLEN=richiesta_int(a);
                        }
                        else {
                            //controllo che il numero fornito dall'utente non sia assurdo
                            if(a<=0) {
                                printf("\n---Errore, numero non valido---\n");
                                printf("Per modificare il maxlen fornito\tpremere [1]\n");
                                printf("Altrimenti, per mantenere il maxlen attuale (%d)\te ignorare l'inserimento corrente (%d)\tpremere un carattere diverso da [1]\n",MAXLEN,a);
                                characters = getline(&buffer,&bufsize,stdin);
                                if(strcmp(buffer,"1\n")==0) {
                                    do {
                                        printf("inserire nuovo valore di MAXLEN: ");
                                        characters = getline(&buffer,&bufsize,stdin);
                                        a=atoi(buffer);
                                        if(a<=0)
                                            printf("---Errore, numero non valido, inserire numero maggiore di zero---\n");
                                    }while(a<=0);
                                    printf("OK, il nuovo MAXLEN e' %d\n",a);
                                    MAXLEN=a;
                                }
                                else {
                                    printf("OK, MAXLEN rimane %d\n",MAXLEN);
                                }
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
                            if(strcmp(argv[ncom+1],"true")==0 || strcmp(argv[ncom+1],"TRUE")==0 || strcmp(argv[ncom+1],"True")==0 || strcmp(argv[ncom+1],"0")==0) {
                                if(CODE==0) {
                                    printf("\nREMINDER: Code gia' fornito in precedenza e con valore uguale\n");
                                    printf("Il code rimarra' 'true'\n");
                                }
                                else{
                                    printf("\nATTENZIONE: Code gia' fornito in precedenza con valore diverso\n");
                                    printf("Per usare il nuovo code (%s) al posto del precedente 'false'\tpremi [1]\n",argv[ncom+1]);
                                    printf("Altrimenti se si desidera mantenere 'false' e ignorare (%s)\tpremi carattere diverso da [1]\n",argv[ncom+1]);
                                    characters = getline(&buffer,&bufsize,stdin);
                                    if(strcmp(buffer,"1\n")==0) {
                                        CODE=0;
                                        printf("OK, nuovo code settato a 'true'\n");
                                    }
                                    else {
                                        printf("OK, il code rimarra' settato a 'false'\n");
                                    }
                                }
                            }
                            else if(strcmp(argv[ncom+1],"false")==0 || strcmp(argv[ncom+1],"false")==0 || strcmp(argv[ncom+1],"False")==0 || strcmp(argv[ncom+1],"1")==0) {
                                if(CODE==1) {
                                    printf("\nREMINDER: Code gia' fornito in precedenza e con valore uguale\n");
                                    printf("Il code rimarra' 'false'\n");
                                }
                                else{
                                    printf("\nATTENZIONE: Code gia' fornito in precedenza con valore diverso\n");
                                    printf("Per usare il nuovo code (%s) al posto del precedente 'true'\tpremi [1]\n",argv[ncom+1]);
                                    printf("Altrimenti se si desidera mantenere 'true' e ignorare (%s)\tpremi carattere diverso da [1]\n",argv[ncom+1]);
                                    characters = getline(&buffer,&bufsize,stdin);
                                    if(strcmp(buffer,"1\n")==0) {
                                        CODE=1;
                                        printf("OK, nuovo code settato a 'false'\n");
                                    }
                                    else {
                                        printf("OK, il code rimarra' settato a 'true'\n");
                                    }
                                }
                            }
                            else {
                                printf("\n---Errore, codice di ritorno non valido---\n");
                                printf("Se si desidera settare il code a 'true'\tpremi [1]\n");
                                printf("Se si desidera settare il code a 'false\tpremi [2]\n");
                                printf("Altrimenti, se si desidera mantenere il code attuale\tpremi carattere diverso da [1] e [2]\n");
                                characters = getline(&buffer,&bufsize,stdin);
                                if(strcmp(buffer,"1\n")==0) {
                                    printf("OK, code settato a 'true'\n");
                                }
                                else if(strcmp(buffer,"2\n")==0) {
                                    printf("OK, code settato a 'false'\n");
                                }
                                else {
                                    if(CODE==0) {
                                        printf("OK, code rimarra' 'true'\n");
                                    }
                                    else {
                                        printf("OK, code rimarra' 'false'\n");
                                    }
                                }
                            }
                        }
                        else {
                            //code true
                            if(strcmp(argv[ncom+1],"true")==0 || strcmp(argv[ncom+1],"TRUE")==0 || strcmp(argv[ncom+1],"True")==0 || strcmp(argv[ncom+1],"0")==0) {
                                CODE=0;
                                cod=1;
                            }
                            //code false
                            else if(strcmp(argv[ncom+1],"false")==0 || strcmp(argv[ncom+1],"false")==0 || strcmp(argv[ncom+1],"False")==0 || strcmp(argv[ncom+1],"1")==0) {
                                CODE=1;
                                cod=1;
                            }
                            //code non riconosciuto
                            else {
                                printf("\n---Errore, codice di ritorno non valido---\n");
                                printf("Se si desidera settare il code a 'true'\tpremi [1]\n");
                                printf("Se si desidera settare il code a 'false\tpremi [2]\n");
                                printf("Altrimenti, se si desidera mantenere il code attuale\tpremi carattere diverso da [1] e [2]\n");
                                characters = getline(&buffer,&bufsize,stdin);
                                if(strcmp(buffer,"1\n")==0) {
                                    printf("OK, code settato a 'true'\n");
                                    CODE=0;
                                    cod=1;
                                }
                                else if(strcmp(buffer,"2\n")==0) {
                                    printf("OK, code settato a 'false'\n");
                                    CODE=1;
                                    cod=1;
                                }
                                else {
                                    if(CODE==0) {
                                        printf("OK, code rimarra' 'true'\n");
                                    }
                                    else {
                                        printf("OK, code rimarra' 'false'\n");
                                    }
                                }
                            }
                        }
                    }
                    
                    //comando -a
                    else if(strcmp(argv[ncom],"-a")==0) {
                        //controllo se modalita' e' gia' stata fornita
                        if(strcmp(argv[ncom+1],"w")!=0 && strcmp(argv[ncom+1],"a")!=0 && strcmp(argv[ncom+1],"w+")!=0 && strcmp(argv[ncom+1],"a+")!=0) {
                            printf("\nATTENZIONE: Modalita' non esistente, verra' mantenuta modalita' di default \"%s\"\n",defaultmode);
                        }
                        else {
                            if(mod==1) {
                                if(strcmp(argv[ncom+1],currentmode)==0) {
                                    printf("\nREMINDER: Modalita' uguale gia' fornita in precedenza\n");
                                    printf("Modalita' rimarra' %s\n",currentmode);
                                }
                                else {
                                    printf("\nATTENZIONE: Modalita' differente fornita in precedenza\n");
                                    printf("Per usare il nuovo mode \"%s\" al posto del precedente \"%s\"\tpremi [1]\n",argv[ncom+1],currentmode);
                                    printf("Altrimenti se si desidera mantenere \"%s\" e ignorare \"%s\"\tpremi carattere diverso da [1]\n",currentmode,argv[ncom+1]);
                                    characters = getline(&buffer,&bufsize,stdin);
                                    if(strcmp(buffer,"1\n")==0) {
                                        free(currentmode);
                                        currentmode = malloc(strlen(argv[ncom+1])*sizeof(char));
                                        strcpy(currentmode,argv[ncom+1]);
                                        printf("OK, modalita' settata a \"%s\"\n",currentmode);
                                    }
                                    else {
                                        printf("OK, modalita' rimarra' settata a \"%s\"\n",currentmode);
                                    }
                                }
                            }
                            else {
                                currentmode = malloc(strlen(argv[ncom+1])*sizeof(char));
                                strcpy(currentmode,argv[ncom+1]);
                                mod=1;
                            }
                        }
                    }
                    else {
                        //altrimenti ritorno errore
                        printf("\n---Errore, comando non valido---\n");
                        printf("---Il programma verra' terminato---\n");
                        errorcount=1;
                    }
                    free(fil);
                }
                ncom=ncom+2;
            }
            
            //comandi forma --
            else {
                int contachar = 0;
                while(contachar<9 && argv[ncom][contachar]!='='){
                    contachar++;
                };
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
                    while(i<contachar){
                        comand[i]=argv[ncom][i];
                        i++;
                    };
                    comand[contachar]='\0';
                    contachar+=1;
                    
                    for(int j=0; j<size; j++) {
                        fil[j]=argv[ncom][j+contachar];
                    }
                    fil[size]='\0';
                    //comando --outfile=
                    if(strcmp(comand,"--outfile")==0) {
                        //controllo se file di output e' gia' stato fornito
                        if(out==1) {
                            char *nuovo = malloc(strlen(currentout)*sizeof(char));
                            strcpy(nuovo,currentout);
                            nuovo = richiesta_file(fil,"File di Output",nuovo);
                            free(currentout);
                            currentout = malloc(strlen(nuovo)*sizeof(char));
                            strcpy(currentout,nuovo);
                        }
                        else {
                            //free(currentout);
                            currentout = malloc(size*sizeof(char));
                            strcpy(currentout,fil);
                            out=1;
                        }
                    }
                    
                    //comando --errfile=
                    else if(strcmp(comand,"--errfile")==0) {
                        if(err==1) {
                            char *nuovo = malloc(strlen(currenterr)*sizeof(char));
                            strcpy(nuovo,currenterr);
                            nuovo = richiesta_file(fil,"File di Errore",nuovo);
                            free(currenterr);
                            currenterr = malloc(strlen(nuovo)*sizeof(char));
                            strcpy(currenterr,nuovo);
                        }
                        else {
                            free(currenterr);
                            currenterr = malloc((size)*sizeof(char));
                            strcpy(currenterr,fil);
                            err=1;
                        }
                    }
                    
                    //comando --tmpfile=
                    else if(strcmp(comand,"--tmpfile")==0) {
                        if(tmp==1) {
                            printf("\n---Errore, template file gia' fornito---\n\n");
                            errorcount=1;
                        }
                        else {
                            currenttmp = malloc(strlen(fil)*sizeof(char));
                            strcpy(currenttmp,fil);
                            if((templatefile=fopen(currenttmp,"r"))!=NULL)
                                tmp=1;
                            else {
                                printf("\n---Errore, template file non esistente---\n");
                                printf("Se desideri usare il template file di default\tpremi [s]\n");
                                characters = getline(&buffer,&bufsize,stdin);
                                if(strcmp(buffer,"s\n")!=0 || strcmp(buffer,"S\n")!=0) {
                                    errorcount=1;
                                    printf("Il programma verra' terminato\n");
                                }
                            }
                        }
                    }
                    
                    //comando --maxlen=
                    else if(strcmp(comand,"--maxlen")==0) {
                        int a;
                        a = atoi(fil);
                        if(mln==1) {
                            MAXLEN=richiesta_int(a);
                        }
                        else {
                            //controllo che il numero fornito dall'utente non sia assurdo
                            if(a<=0) {
                                printf("\n---Errore, numero non valido---\n");
                                printf("Per modificare il maxlen fornito\tpremere [1]\n");
                                printf("Altrimenti, per mantenere il maxlen attuale (%d)\te ignorare l'inserimento corrente (%d)\tpremere un carattere diverso da [1]\n",MAXLEN,a);
                                characters = getline(&buffer,&bufsize,stdin);
                                if(strcmp(buffer,"1\n")==0) {
                                    do {
                                        printf("inserire nuovo valore di MAXLEN: ");
                                        characters = getline(&buffer,&bufsize,stdin);
                                        a=atoi(buffer);
                                        if(a<=0)
                                            printf("---Errore, numero non valido, inserire numero maggiore di zero---\n");
                                    }while(a<=0);
                                    printf("OK, il nuovo MAXLEN e' %d\n",a);
                                    MAXLEN=a;
                                }
                                else {
                                    printf("OK, MAXLEN rimane %d\n",MAXLEN);
                                }
                            }
                            else
                                MAXLEN = a;
                            mln=1;
                        }
                    }
                    
                    //comando --code=
                    else if(strcmp(comand,"--code")==0) {
                        if(cod==1) {
                            if(strcmp(fil,"true")==0 || strcmp(fil,"TRUE")==0 || strcmp(fil,"True")==0 || strcmp(fil,"0")==0) {
                                if(CODE==0) {
                                    printf("\nREMINDER: Code gia' fornito in precedenza e con valore uguale\n");
                                    printf("Il code rimarra' 'true'\n");
                                }
                                else{
                                    printf("\nATTENZIONE: Code gia' fornito in precedenza con valore diverso\n");
                                    printf("Per usare il nuovo code (%s) al posto del precedente 'false'\tpremi [1]\n",argv[ncom+1]);
                                    printf("Altrimenti se si desidera mantenere 'false' e ignorare (%s)\tpremi carattere diverso da [1]\n",argv[ncom+1]);
                                    characters = getline(&buffer,&bufsize,stdin);
                                    if(strcmp(buffer,"1\n")==0) {
                                        CODE=0;
                                        printf("OK, nuovo code settato a 'true'\n");
                                    }
                                    else {
                                        printf("OK, il code rimarra' settato a 'false'\n");
                                    }
                                }
                            }
                            else if(strcmp(fil,"false")==0 || strcmp(fil,"false")==0 || strcmp(fil,"False")==0 || strcmp(fil,"1")==0) {
                                if(CODE==1) {
                                    printf("\nREMINDER: Code gia' fornito in precedenza e con valore uguale\n");
                                    printf("Il code rimarra' 'false'\n");
                                }
                                else{
                                    printf("\nATTENZIONE: Code gia' fornito in precedenza con valore diverso\n");
                                    printf("Per usare il nuovo code (%s) al posto del precedente 'true'\tpremi [1]\n",argv[ncom+1]);
                                    printf("Altrimenti se si desidera mantenere 'true' e ignorare (%s)\tpremi carattere diverso da [1]\n",argv[ncom+1]);
                                    characters = getline(&buffer,&bufsize,stdin);
                                    if(strcmp(buffer,"1\n")==0) {
                                        CODE=1;
                                        printf("OK, nuovo code settato a 'false'\n");
                                    }
                                    else {
                                        printf("OK, il code rimarra' settato a 'true'\n");
                                    }
                                }
                            }
                            else {
                                printf("\n---Errore, codice di ritorno non valido---\n");
                                printf("Se si desidera settare il code a 'true'\tpremi [1]\n");
                                printf("Se si desidera settare il code a 'false\tpremi [2]\n");
                                printf("Altrimenti, se si desidera mantenere il code attuale\tpremi carattere diverso da [1] e [2]\n");
                                characters = getline(&buffer,&bufsize,stdin);
                                if(strcmp(buffer,"1\n")==0) {
                                    printf("OK, code settato a 'true'\n");
                                }
                                else if(strcmp(buffer,"2\n")==0) {
                                    printf("OK, code settato a 'false'\n");
                                }
                                else {
                                    if(CODE==0) {
                                        printf("OK, code rimarra' 'true'\n");
                                    }
                                    else {
                                        printf("OK, code rimarra' 'false'\n");
                                    }
                                }
                            }
                        }
                        else {
                            //code true
                            if(strcmp(fil,"true")==0 || strcmp(fil,"TRUE")==0 || strcmp(fil,"True")==0 || strcmp(fil,"0")==0) {
                                CODE=0;
                                cod=1;
                            }
                            //code false
                            else if(strcmp(fil,"false")==0 || strcmp(fil,"false")==0 || strcmp(fil,"False")==0 || strcmp(fil,"1")==0) {
                                CODE=1;
                                cod=1;
                            }
                            //code non riconosciuto
                            else {
                                printf("\n---Errore, codice di ritorno non valido---\n");
                                printf("Se si desidera settare il code a 'true'\tpremi [1]\n");
                                printf("Se si desidera settare il code a 'false\tpremi [2]\n");
                                printf("Altrimenti, se si desidera mantenere il code attuale\tpremi carattere diverso da [1] e [2]\n");
                                characters = getline(&buffer,&bufsize,stdin);
                                if(strcmp(buffer,"1\n")==0) {
                                    printf("OK, code settato a 'true'\n");
                                    CODE=0;
                                    cod=1;
                                }
                                else if(strcmp(buffer,"2\n")==0) {
                                    printf("OK, code settato a 'false'\n");
                                    CODE=1;
                                    cod=1;
                                }
                                else {
                                    if(CODE==0) {
                                        printf("OK, code rimarra' 'true'\n");
                                    }
                                    else {
                                        printf("OK, code rimarra' 'false'\n");
                                    }
                                }
                            }
                        }
                    }
                    
                    //comando --aprmode
                    else if(strcmp(comand,"--aprmode")==0) {
                        if(strcmp(fil,"w")!=0 && strcmp(fil,"a")!=0 && strcmp(fil,"w+")!=0 && strcmp(fil,"a+")!=0) {
                            printf("\nATTENZIONE: Modalita' non esistente, verra' mantenuta modalita' di default \"%s\"\n",defaultmode);
                        }
                        else {
                            if(mod==1) {
                                if(strcmp(fil,currentmode)==0) {
                                    printf("\nREMINDER: Modalita' uguale gia' fornita in precedenza\n");
                                    printf("Modalita' rimarra' %s\n",currentmode);
                                }
                                else {
                                    printf("\nATTENZIONE: Modalita' differente fornita in precedenza\n");
                                    printf("Per usare il nuovo mode \"%s\" al posto del precedente \"%s\"\tpremi [1]\n",fil,currentmode);
                                    printf("Altrimenti se si desidera mantenere \"%s\" e ignorare \"%s\"\tpremi carattere diverso da [1]\n",currentmode,fil);
                                    characters = getline(&buffer,&bufsize,stdin);
                                    if(strcmp(buffer,"1\n")==0) {
                                        free(currentmode);
                                        currentmode = malloc(strlen(fil)*sizeof(char));
                                        strcpy(currentmode,fil);
                                        printf("OK, modalita' settata a \"%s\"\n",currentmode);
                                    }
                                    else {
                                        printf("OK, modalita' rimarra' settata a \"%s\"\n",currentmode);
                                    }
                                }
                            }
                            else {
                                currentmode = malloc(strlen(fil)*sizeof(char));
                                strcpy(currentmode,fil);
                                mod=1;
                            }
                        }
                    }
                    //comando non riconosciuto
                    else {
                        printf("\n---Errore, comando non valido: %s ---\n\n",argv[ncom]);
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
        printf("\tmodalita' apertura output:\t -a CHAR\t| --aprmode=CHAR\n");
        printf("\tfile template:   -t FILENAME\t| --tmpfile=FILENAME\n");
        printf("\tmaxlen:\t\t -m POS_INT\t| --maxlen=POS_INT\n");
        printf("\tcode:\t\t -c TRUE/FALSE\t| --code=TRUE/FALSE\n");
        printf("\n---Esempio di esecuzione---\n");
        printf("./shell --maxlen=100 -o \"output.txt\" --errfile=\"pippo.txt\" -t \"newtemp.txt\"\n\n");
        exit(1);
    }
    else {
        
        //apro file output
        if(out==0) {
            if(mod==0) {
                //outputfile=fopen(config->shellConf.outfile,defaultmode);
                outputfile=fopen(defout,defaultmode);
            }
            else {
                //outputfile=fopen(config->shellConf.outfile,currentmode);
                outputfile=fopen(defout,currentmode);
            }
        }
        else {
            if(mod==0) {
                outputfile=fopen(currentout,defaultmode);
            }
            else {
                outputfile=fopen(currentout,currentmode);
            }
        }
        
        //apro file error
        if(err==0) {
            if(mod==0) {
                //errorfile=fopen(config->shellConf.errfile,defaultmode);
                errorfile=fopen(deferr,defaultmode);
            }
            else {
                //errorfile=fopen(config->shellConf.errfile,currentmode);
                errorfile=fopen(deferr,currentmode);
            }
        }
        else {
            if(mod==0) {
                errorfile=fopen(currenterr,defaultmode);
            }
            else {
                errorfile=fopen(currenterr,currentmode);
            }
        }
        
        //apro file template
        if(tmp==0) {
            templatefile=fopen(deftmp,"r");
        }
    }
    return;
}

char *richiesta_file(char *file, char *richiesta, char *corrente) {
    char *buffer;
    size_t bufsize = 32;
    size_t characters;
    
    buffer = (char *)malloc(bufsize * sizeof(char));
    if( buffer == NULL) {
        perror("Unable to allocate buffer");
        exit(1);
    }
    
    printf("\n---Errore, %s gia' fornito---\n\n",richiesta);
    //chiedo quale file si desidera tenere
    printf("Al momento hai gia' specificato il file %s come %s desiderato\n",corrente,richiesta);
    printf("Se desideri usare il file %s come nuovo %s premi [s]: ",file,richiesta);
    characters = getline(&buffer,&bufsize,stdin);
    if(strcmp(buffer,"s\n")==0 || strcmp(buffer,"S\n")==0) {
        free(corrente);
        corrente = malloc(strlen(file)*sizeof(char));
        strcpy(corrente,file);
        printf("OK, nuovo %s e' %s\n",richiesta,corrente);
    }
    else {
        printf("OK, %s rimane %s\n",richiesta,corrente);
    }
    return corrente;
}



int richiesta_int(int a) {
    char *buffer;
    size_t bufsize = 32;
    size_t characters;
    
    buffer = (char *)malloc(bufsize * sizeof(char));
    if( buffer == NULL) {
        perror("Unable to allocate buffer");
        exit(1);
    }
    
    if(a<=0) {
        printf("\n---Errore, numero non valido---\n\n");
        printf("Se si desidera modificare e usare l'ultimo inserimento (%d)\ne scartare il maxlen precedentemente fornito (%d)\tpremere [1]\n",a,MAXLEN);
        printf("Altrimenti, se si desidera usare il maxlen attuale (%d)\te ignorare l'ultimo inserimento (%d)\tpremere un carattere diverso da [1]\n",MAXLEN,a);
        characters = getline(&buffer,&bufsize,stdin);
        if(strcmp(buffer,"1\n")==0) {
            do {
                printf("inserire nuovo valore di MAXLEN: ");
                characters = getline(&buffer,&bufsize,stdin);
                a=atoi(buffer);
                if(a<=0)
                    printf("---Errore, numero non valido, inserire numero maggiore di zero---\n");
            }while(a<=0);
            printf("OK, il nuovo maxlen e' %d\n",a);
            return a;
        }
        else {
            printf("OK, maxlen rimane %d\n",MAXLEN);
            return MAXLEN;
        }
    }
    else {
        printf("Se si desidera modificare e usare l'ultimo inserimento (%d)\ne scartare il maxlen precedentemente fornito (%d)\tpremere [1]\n",a,MAXLEN);
        printf("Altrimenti, se si desidera usare il maxlen attuale (%d)\te ignorare l'ultimo inserimento (%d)\tpremere un carattere diverso da [1]\n",MAXLEN,a);
        characters = getline(&buffer,&bufsize,stdin);
        if(strcmp(buffer,"1\n")==0) {
            printf("OK, il nuovo maxlen e' %d\n",a);
            return a;
        }
        else {
            printf("OK, maxlen rimane %d\n",MAXLEN);
            return MAXLEN;
        }
    }
}
