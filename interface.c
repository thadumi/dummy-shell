//
//  interface.c
//  
//
//  Created by Andrea Balasso on 25/05/18.
//

#include <stdio.h>
#include <string.h>
#include "headers/structure.h"
#include "headers/interface.h"

void visible() {
    printf("Al momento i dati visibili sono:\n");
    if(templateProfile._comando==0)
        printf("\t_comando\n");
    if(templateProfile._timeStamp==0)
        printf("\t_timeStamp\n");
    if(templateProfile._output==0)
        printf("\t_output\n");
    if(templateProfile._returnCode==0)
        printf("\t_returnCode\n");
    if(templateProfile._idProcesso==0)
        printf("\t_idProcesso\n");
    if(templateProfile._idProcessoPadre==0)
        printf("\t_idProcessoPadre\n");
    if(templateProfile._runtimeSeconds==0)
        printf("\t_runtimeSeconds\n");
    if(templateProfile._memoryUsed==0)
        printf("\t_memoryUsed\n");
    if(templateProfile._user==0)
        printf("\t_user\n");
}

void personalizzazionefilelog () {
    char *buffer;
    size_t bufsize = 32;
    size_t characters;
    
    visible();
    
    printf("Per apportare modifiche, premi [s]\n");
    characters = getline(&buffer,&bufsize,stdin);
    if(strcmp(buffer,"s\n")==0 || strcmp(buffer,"S\n")==0) {
        printf("Digita il nome di un dato per modificarne la visibilita'\n");
        printf("Per visualizzare i dati modificabili, digita \"list\"\n");
        printf("Per visualizzare i dati attualmente visibili, digita \"visible\"\n");
        printf("Per terminare, digita \"end\"\n");
        do {
            characters = getline(&buffer,&bufsize,stdin);
            if(strcmp(buffer,"_comando\n")==0 || strcmp(buffer,"comando\n")==0 || strcmp(buffer,"Comando\n")==0) {
                if(templateProfile._comando==0)
                    templateProfile._comando=1;
                else
                    templateProfile._comando=0;
            }
            if(strcmp(buffer,"_timeStamp\n")==0 || strcmp(buffer,"timeStamp\n")==0  || strcmp(buffer,"timestamp\n")==0 || strcmp(buffer,"Timestamp\n")==0) {
                if(templateProfile._timeStamp==0)
                    templateProfile._timeStamp=1;
                else
                    templateProfile._timeStamp=0;
            }
            if(strcmp(buffer,"_output\n")==0 || strcmp(buffer,"output\n")==0 || strcmp(buffer,"Output\n")==0) {
                if(templateProfile._output==0)
                    templateProfile._output=1;
                else
                    templateProfile._output=0;
            }
            if(strcmp(buffer,"_returnCode\n")==0 || strcmp(buffer,"returnCode\n")==0 || strcmp(buffer,"returncode\n")==0 || strcmp(buffer,"Returncode\n")==0) {
                if(templateProfile._returnCode==0)
                    templateProfile._returnCode=1;
                else
                    templateProfile._returnCode=0;
            }
            if(strcmp(buffer,"_idProcesso\n")==0 || strcmp(buffer,"idProcesso\n")==0 || strcmp(buffer,"idprocesso\n")==0) {
                if(templateProfile._idProcesso==0)
                    templateProfile._idProcesso=1;
                else
                    templateProfile._idProcesso=0;
            }
            if(strcmp(buffer,"_idProcessoPadre\n")==0 || strcmp(buffer,"idProcessoPadre\n")==0 || strcmp(buffer,"idprocessopadre\n")==0) {
                if(templateProfile._idProcessoPadre==0)
                    templateProfile._idProcessoPadre=1;
                else
                    templateProfile._idProcessoPadre=0;
            }
            if(strcmp(buffer,"_runtimeSeconds\n")==0 || strcmp(buffer,"runtimeSeconds\n")==0 || strcmp(buffer,"runtimeseconds\n")==0) {
                if(templateProfile._runtimeSeconds==0)
                    templateProfile._runtimeSeconds=1;
                else
                    templateProfile._runtimeSeconds=0;
            }
            if(strcmp(buffer,"_memoryUsed\n")==0 || strcmp(buffer,"memoryUsed\n")==0 || strcmp(buffer,"memoryused\n")==0) {
                if(templateProfile._memoryUsed==0)
                    templateProfile._memoryUsed=1;
                else
                    templateProfile._memoryUsed=0;
            }
            if(strcmp(buffer,"_user\n")==0 || strcmp(buffer,"user\n")==0) {
                if(templateProfile._user==0)
                    templateProfile._user=1;
                else
                    templateProfile._user=0;
            }
            if(strcmp(buffer,"list\n")==0) {
                printf("_comando\t_timeStamp\t_output\n");
                printf("_returnCode\t_idProcesso\t_idProcessoPadre\n");
                printf("_runtimeSeconds\t_memoryUsed\t_user\n");
            }
            if(strcmp(buffer,"visible\n")==0) {
                visible();
            }
        }while(strcmp(buffer,"end\n")!=0 && strcmp(buffer,"END\n")!=0);
    }
    return;
}
