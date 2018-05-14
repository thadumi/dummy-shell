#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//TRY VERSION OF THE PARSER, FOR THE SINGLE FUNCTION THE FILE IS "parser.c"

int MAXLEN=50;  //maxlen di default
int CODE=0;     //code di default, 0=true 1=false
int errorcount=0;

/*struct dir {
    struct *father;
    char name[100];
}*/

FILE *outfile;
FILE *errfile;
FILE *tempfile;

void getfile(int argc, char **argv);

int main(int argc, char **argv) {

    
    // ./shell --outfile="nomefile" --errfile="nomefile" ...
    // ./shell -o nomefile -e nomefile ...
    
    /*if(argc==1) {
        outfile=fopen("outputfile.txt","w");
        errfile=fopen("errorfile.txt","w");
        tempfile=fopen("templatefile.txt","r");
    }*/
    //else {
        getfile(argc,argv);
        if(errorcount!=0) {
            goto end;
        }
    //}
    
    FILE *fp;
    char *buffer;
    size_t bufsize = 32;
    size_t characters;
    int control=0;
    char pwd[256];;
    system("sh pwd.sh");
    fp=fopen("pwd.txt","r");
    fscanf(fp,"%s",pwd);
    fclose(fp);
    //printf("%s",pwd);
    buffer = (char *)malloc(bufsize * sizeof(char));
    if( buffer == NULL) {
        perror("Unable to allocate buffer");
        exit(1);
    }
    printf("---SHELL CUSTOM---\n");
    printf("scopri che comandi puoi usare usando il menu \"help\"\n\n");
    
    do{
        printf("> ");
        characters = getline(&buffer,&bufsize,stdin);
        if(strcmp(buffer,"exit\n")==0) {
            control=-1;
        }
        else if(strcmp(buffer,"help\n")==0) {
            printf("\n---COMANDI DISPONIBILI---\n");
            printf("ls\t\tpwd\n");
            printf("mkdir\t\tdate\n");
            printf("help\t\tmenu\n");
            printf("back\t\texit\n\n");
        }
        else if(strcmp(buffer,"menu\n")==0) {
            //funzione configuration file
        }/*
        else if(buffer[0]=='c' && buffer[1]=='d' buffer[2]==' ') {
            int p=3;
            
            while(buffer[p]!='\n') {
                
            }
        }*/
        else {
            //a ogni comando aggiungere percorso pwd
            char nuova[256];
            strcpy (nuova,"#!/bin/bash\n");
            strcat(nuova, buffer);
            //strcat(nuova, "\b /Users/andreabalasso/Desktop/");
            fp=fopen("command.sh","w");
            if(fp != NULL) {
                rewind(fp);
                fprintf(fp,"#!/bin/bash\n %s", nuova);
                fclose(fp);
            }
            else {
                printf("errore apertura file command.sh");
            }
            system("sh command.sh");
        }
    }while(control!=-1);
    printf("\n---grazie per aver usato la nostra shell custom---\n\n");

    fclose(outfile);
    fclose(errfile);
    fclose(tempfile);
    fclose(fp);
    end:
    return 0;
}


//CONTROLLA ARGOMENTI PASSATI DA RIGA DI COMANDO AL LANCIO DEL PROGRAMMA
void getfile(int argc, char **argv) {
    int ncom=1;
    
    int out=0;  //verifico se ho gia' ricevuto un file di output
    int err=0;  //verifico se ho gia' ricevuto un file di errore
    int tmp=0;  //verifico se ho gia' ricevuto un file template
    int mln=0;  //verifico se ho gia' ricevuto maxlen
    int cod=0;  //verifico se ho gia' ricevuto code true/false
    
    while(ncom<argc && errorcount==0) {
        //printf("ncom=%d\n",ncom);
        char *fil = malloc(MAXLEN*sizeof(char));
        if( fil == NULL) {
            perror("Unable to allocate fil String");
            exit(1);
        }
        //controllo che il comando sia nella forma - o --
        if(argv[ncom][0]!='-') {
            printf("\nErrore, comando non valido\n\n");
            errorcount=1;
        }
        else {
            //casi in cui il comando e' nella forma -
            if(argv[ncom][0]=='-' && argv[ncom][1]!='-') {
                //switch dei casi possibili
                switch(argv[ncom][1]) {
                    case 'o':
                        //caso definizione output file nella forma -o OUTFILE
                        //controllo che non sia gia' stato fornito file output precedentemente
                        if(out==1) {
                            printf("\nErrore, output file gia' fornito\n\n");
                            errorcount=1;
                        }
                        //se output file non e' stato fornito precedentemente, lo leggo
                        else {
                            if(argc>=ncom+2) {
                                strcpy(fil,argv[ncom+1]);
                                outfile=fopen(fil,"w");
                                //printf("OUTFILE fornito e'%s\n",fil);
                                out=1;
                            }
                            else {
                                printf("\nErrore, numero argomenti non valido\n\n");
                                errorcount=1;
                            }
                        }
                        break;
                    case 'e':
                        //caso definizione error file nella forma -e ERRFILE
                        //controllo che non sia gia' stato fornito file error precedentemente
                        if(err==1) {
                            printf("\nErrore, error file gia' fornito\n\n");
                            errorcount=1;
                        }
                        //se error file non e' stato fornito precedentemente, lo leggo
                        else {
                            if(argc>=ncom+2) {
                                strcpy(fil,argv[ncom+1]);
                                errfile=fopen(fil,"w");
                                //printf("ERRORFILE fornito e'%s\n",fil);
                                err=1;
                            }
                            else {
                                printf("\nErrore, numero argomenti non valido\n\n");
                                errorcount=1;
                            }
                        }
                        break;
                    case 't':
                        //caso definizione template file nella forma -t TMPFILE
                        //controllo che non sia gia' stato fornito file template precedentemente
                        if(tmp==1) {
                            printf("\nErrore, template file gia' fornito\n\n");
                            errorcount=1;
                        }
                        //se template file non e' stato fornito precedentemente, lo leggo
                        else {
                            if(argc>=ncom+2) {
                                strcpy(fil,argv[ncom+1]);
                                tempfile=fopen(fil,"r");
                                //printf("tempfile fornito e'%s\n",fil);
                                tmp=1;
                            }
                            else {
                                printf("\nErrore, numero argomenti non valido\n\n");
                                errorcount=1;
                            }
                        }
                        break;
                    case 'm':
                        //caso definizione maxlen nella forma -m MAXLEN (dove MAXLEN e' un int)
                        //controllo che non sia gia' stato fornito maxlen precedentemente
                        if(mln==1) {
                            printf("\nErrore, maxlen gia' fornito\n\n");
                            errorcount=1;
                        }
                        else {
                            if(argc>=ncom+2) {
                                //printf("MAXLEN fornito\n");
                                int a;
                                a = atoi(argv[ncom+1]);
                                //controllo che il numero fornito dall'utente non sia assurdo
                                if(a<=0) {
                                    printf("\nErrore, numero non valido\n\n");
                                    errorcount=1;
                                }
                                else
                                    MAXLEN = a;
                                mln=1;
                            }
                            else {
                                printf("\nErrore, numero argomenti non valido\n\n");
                                errorcount=1;
                            }
                        }
                        break;
                    case 'c':
                        //caso definizione code nella forma -c CODE (dove verifico perche' sia valido che sia true o false
                        //controllo che non sia gia' stato fornito code precedentemente
                        if(cod==1) {
                            printf("\nErrore, code gia' fornito\n\n");
                            errorcount=1;
                        }
                        else {
                            if(argc>=ncom+2) {
                                if(strcmp(argv[ncom+1],"true")==0 || strcmp(argv[ncom+1],"TRUE")==0 || strcmp(argv[ncom+1],"True")==0) {
                                    CODE=0;
                                    cod=1;
                                }
                                else if(strcmp(argv[ncom+1],"false")==0 || strcmp(argv[ncom+1],"false")==0 || strcmp(argv[ncom+1],"False")==0) {
                                    CODE=1;
                                    cod=1;
                                }
                                else {
                                    printf("\nErrore, codice di ritorno non valido\n\n");
                                    errorcount=1;
                                }
                                //printf("CODE fornito\n");
                            }
                            else {
                                printf("\nErrore, numero argomenti non valido\n\n");
                                errorcount=1;
                            }
                        }
                        break;
                    default:
                        //altrimenti ritorno errore
                        printf("\nErrore, comando non valido\n\n");
                        errorcount=1;
                        break;
                }
                ncom=ncom+2;
            }
            
            //casi in cui il comando e' nella forma --
            else {
                int len=strlen(argv[ncom]);
                int i=10;   //contatore per i FILE
                int j=9;    //contatore per il MAXLEN
                int k=7;
                int conta=0;    //contatore default per il vettore di appoggio
                //char file[256]; //vettore d'appoggio
                switch(argv[ncom][2]) {
                    case 'o':
                        if(out==1) {
                            printf("\nErrore, output file gia' fornito\n\n");
                            errorcount=1;
                        }
                        else {
                            //se sintassi corrisponde interamente, allora leggo file chiesto dall'utente
                            if(argv[ncom][3]=='u' && argv[ncom][4]=='t' && argv[ncom][5]=='f' && argv[ncom][6]=='i' && argv[ncom][7]=='l' && argv[ncom][8]=='e' && argv[ncom][9]=='=') {
                                while(i<=len){
                                    fil[conta]=argv[ncom][i];
                                    i++;
                                    conta++;
                                }
                                outfile=fopen(fil,"w");
                                out=1;
                                printf("PROVA FILE OUTPUT --outfile%s\n",fil);
                            }
                            //altrimenti
                            else {
                                printf("\nErrore, comando non valido\n\n");
                                errorcount=1;
                            }
                        }
                        break;
                    case 'e':
                        if(err==1) {
                            printf("\nErrore, error file gia' fornito\n\n");
                            errorcount=1;
                        }
                        else {
                            //se sintassi corrisponde interamente, allora leggo file chiesto dall'utente
                            if(argv[ncom][3]=='r' && argv[ncom][4]=='r' && argv[ncom][5]=='f' && argv[ncom][6]=='i' && argv[ncom][7]=='l' && argv[ncom][8]=='e' && argv[ncom][9]=='=') {
                                while(i<=len){
                                    fil[conta]=argv[ncom][i];
                                    i++;
                                    conta++;
                                }
                                errfile=fopen(fil,"w");
                                err=1;
                                //printf("PROVA FILE ERRORE --errfile%s\n",fil);
                            }
                            //altrimenti
                            else {
                                printf("\nErrore, comando non valido\n\n");
                                errorcount=1;
                            }
                        }
                        break;
                    case 't':
                        if(tmp==1) {
                            printf("\nErrore, error file gia' fornito\n\n");
                            errorcount=1;
                        }
                        else {
                            if(argv[ncom][3]=='m' && argv[ncom][4]=='p' && argv[ncom][5]=='f' && argv[ncom][6]=='i' && argv[ncom][7]=='l' && argv[ncom][8]=='e' && argv[ncom][9]=='=') {
                                while(i<=len){
                                    fil[conta]=argv[ncom][i];
                                    i++;
                                    conta++;
                                }
                                tempfile=fopen(fil,"r");
                                tmp=1;
                            }
                            //altrimenti
                            else {
                                printf("\nErrore, comando non valido\n\n");
                                errorcount=1;
                            }
                        }
                        break;
                    case 'm':
                        if(mln==1) {
                            printf("\nErrore, error file gia' fornito\n\n");
                            errorcount=1;
                        }
                        else {
                            //se sintassi corrisponde interamente, allora leggo numero chiesto dall'utente
                            if(argv[ncom][3]=='a' && argv[ncom][4]=='x' && argv[ncom][5]=='l' && argv[ncom][6]=='e' && argv[ncom][7]=='n' && argv[ncom][8]=='=') {
                                while(j<=len){
                                    fil[conta]=argv[ncom][j];
                                    j++;
                                    conta++;
                                }
                                int a;
                                a = atoi(fil);
                                //controllo che il numero fornito dall'utente non sia assurdo
                                if(a<=0) {
                                    printf("\nErrore, numero non valido\n\n");
                                    errorcount=1;
                                }
                                else
                                    MAXLEN = a;
                                mln=1;
                            }
                            //altrimenti
                            else {
                                printf("\nErrore, comando non valido\n\n");
                                errorcount=1;
                            }
                        }
                        break;
                    case 'c':
                        if(cod==1) {
                            printf("\nErrore, code gia' fornito\n\n");
                            errorcount=1;
                        }
                        else {
                            //se sintassi corrisponde interamente, allora leggo codice chiesto dall'utente
                            if(argv[ncom][3]=='o' && argv[ncom][4]=='d' && argv[ncom][5]=='e' && argv[ncom][6]=='=') {
                                while(k<=len){
                                    fil[conta]=argv[ncom][k];
                                    k++;
                                    conta++;
                                }
                                if(strcmp(fil,"true")==0 || strcmp(fil,"TRUE")==0 || strcmp(fil,"True")==0) {
                                    CODE=0;
                                    cod=1;
                                }
                                else if(strcmp(fil,"false")==0 || strcmp(fil,"false")==0 || strcmp(fil,"False")==0) {
                                    CODE=1;
                                    cod=1;
                                }
                                else {
                                    printf("\nErrore, codice di ritorno non valido\n\n");
                                    errorcount=1;
                                }
                            }
                            //altrimenti
                            else {
                                printf("\nErrore, comando non valido\n\n");
                                errorcount=1;
                            }
                        }
                        break;
                    default:
                        printf("\nErrore, comando non valido\n\n");
                        break;
                }
                ncom=ncom+1;
            }
        }
        free(fil);
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
