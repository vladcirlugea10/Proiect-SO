#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#define BUFFER_SIZE 256

void verificareArgumente(int nrArgumente, int fisier, char *denumireFisier){
    struct stat data;
    if((nrArgumente != 2)){
        perror("Usage ./program <fisier_intrare>\n");
        exit(-1);
    }
    if((fisier = open(denumireFisier, O_RDONLY)) < 0){
        perror("Eroare la deschiderea fisierului!\n");
        exit(-1);
    }
    if (lstat(denumireFisier, &data) < 0) {
        perror("Eroare la obtinerea informatiilor despre fisier!\n");
        exit(-1);
    } 
    int m = data.st_mode;
    if(S_ISREG(m)){
        char *p;
        char numeFisier[BUFFER_SIZE];
        char extensieFisier[BUFFER_SIZE];
        p = strtok(denumireFisier, ".");
        strcpy(numeFisier, p);
        p = strtok(NULL, ".");
        strcpy(extensieFisier, p);
        if(strcmp(extensieFisier, "bmp") != 0){
            perror("Fisierul dat ca argument nu este de tip bmp!>\n");
            exit(-1);
        }
        printf("Ok!\n");
    }
    else
    {
        perror("Usage ./program <fisier_intrare>\n");
        exit(-1);
    }
}

void creareFisier(char *denumire, int fisier){
    if((fisier = open(denumire, O_WRONLY | O_TRUNC |O_CREAT, S_IRWXU)) < 0){
        perror("Eroare la crearea fisierului!\n");
        exit(-1);
    }
}

int main(int argc, char *argv[]){
    int fIn, fOut, rd;

    verificareArgumente(argc, fIn, argv[1]);
    /*creareFisier("statistica.txt", fOut);*/

    /*char buffer[2];
    while((rd = read()))*/

    return 0;
}