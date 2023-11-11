#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <time.h>
#define BUFFER_SIZE 256
#define READ_BUFFER_SIZE 2

int verificareArgument(int nrArgumente, char* director){
    struct stat data;
    DIR *dir;

    if((nrArgumente != 2)){
        perror("Usage ./program <director>\n");
        exit(-1);
    }
    if((dir = opendir(director)) == NULL){
        perror("Eroare la deschiderea directorului!\n");
        exit(-1);
    }
    if(lstat(director, &data) < 0){
        perror("Eroare la obtinerea informatiilor despre director!\n");
        exit(-1);
    }
    int m = data.st_mode;
    if(S_ISDIR(m)){
        printf("Director!\n");
        return 1;
    }
    else{
        return 0;
    }
}

int verificareBMP(char *denumireFisier){
    struct stat data;
    int fisier;
    if((fisier = open(denumireFisier, O_RDONLY)) < 0){
        perror("Eroare la deschiderea fisierului!\n");
        exit(-1);
    }
    if (lstat(denumireFisier, &data) < 0) {
        perror("Eroare la obtinerea informatiilor despre fisier!\n");
        close(fisier);
        exit(-1);
    }
    close(fisier);
    int m = data.st_mode;
    if(S_ISREG(m)){
        char *p;
        char numeFisier[BUFFER_SIZE];
        char extensieFisier[BUFFER_SIZE];
        p = strtok(denumireFisier, ".");
        strcpy(numeFisier, p);
        p = strtok(NULL, ".");
        if(p == NULL){
            return 0;
        }
        strcpy(extensieFisier, p);
        if(strcmp(extensieFisier, "bmp") != 0){
            return 0;
        }
    }
    else
    {
        perror("Nu este un fisier simplu!\n");
        return 0;
    }
    return 1;
}


int creareFisier(char *denumire){
    int fisier;
    if((fisier = open(denumire, O_WRONLY | O_TRUNC |O_CREAT, S_IRWXU)) < 0){
        perror("Eroare la crearea fisierului!\n");
        exit(-1);
    }
    else
    {
        printf("Fisier de iesire creat!\n");
    }
    return fisier;
}

void scriereDataBMP(int fisierIntrare, int fisierIesire){
    int count = 0;
    char buffer[BUFFER_SIZE];
    char *fileSize, *fileWidth, *fileHeight;
    fileSize = malloc(BUFFER_SIZE * sizeof(char));
    fileWidth = malloc(BUFFER_SIZE * sizeof(char));
    fileHeight = malloc(BUFFER_SIZE * sizeof(char));
    if(fileSize == NULL || fileWidth == NULL || fileHeight == NULL) {
        perror("Failed to allocate memory");
        exit(-1);
    }
    
    while(read(fisierIntrare, buffer, READ_BUFFER_SIZE)> 0)
    {
        count = count + 2;
        char asciiBuffer[READ_BUFFER_SIZE * 2 + 1];
        for (int i = 0; i < READ_BUFFER_SIZE; i++) {
            sprintf(asciiBuffer + i * 2, "%02X", buffer[i]);
        }
        if(count == 4)
        {
            sprintf(fileSize, "Dimensiune: %s\n", asciiBuffer);
        }
        if(count == 6){
            strcat(fileSize, buffer);
            if(write(fisierIesire, fileSize, strlen(fileSize)) < 0){
                perror("Eroare la scrierea 'fileSize'!\n");
                exit(-1);
            }
        }
        if(count == 20){
            sprintf(fileWidth, "Latime: %s\n", buffer);
        }
        if(count == 22){
            strcat(fileWidth, buffer);
            if(write(fisierIesire, fileWidth, strlen(fileWidth)) < 0){
                perror("Eroare la scrierea 'fileWidth'!\n");
                exit(-1);
            }
        }
        if(count == 24){
            sprintf(fileHeight, "Inaltime: %s\n", buffer);
        }
        if(count == 26){
            strcat(fileHeight, buffer);
            if(write(fisierIesire, fileHeight, strlen(fileHeight)) < 0){
                perror("Eroare la scrierea 'fileHeight'!\n");
                exit(-1);
            }
        }
        if(count > 30){
            break;
        }
    }

    printf("Scriere terminata\n");
    free(fileHeight);
    free(fileWidth);
    free(fileSize);
}

void scriereDateFisier(int fisierIntrare, int fisierIesire){
    char userID[BUFFER_SIZE];
    char lastModified[BUFFER_SIZE];
    char nrOfLinks[BUFFER_SIZE];
    char userAccessRight[BUFFER_SIZE] = "Drepturi de acces user: ";
    char groupAccessRight[BUFFER_SIZE] = "Drepturi de acces grup: ";
    char otherAccessRight[BUFFER_SIZE] = "Drepturi de acces altii: ";
    char fileSize[BUFFER_SIZE];
    struct stat data;
    if (fstat(fisierIntrare, &data) < 0) {
        perror("Eroare la obtinerea informatiilor despre fisier!\n");
        exit(-1);
    }
    sprintf(fileSize, "Dimensiune: %d octeti\n", data.st_size);
    sprintf(userID, "Identificatorul Utilizatorului: %d\n", data.st_uid);
    sprintf(lastModified, "Ultima modificare: %s", ctime(&data.st_atime));
    sprintf(nrOfLinks, "Contorul de legaturi: %d\n", data.st_nlink);
    if(write(fisierIesire, fileSize, strlen(fileSize)) < 0){
        perror("Eroare la scrierea dimensiunii!\n");
        exit(-1);
    }
    if(write(fisierIesire, userID, strlen(userID)) < 0){
        perror("Eroare la scrierea numelui!\n");
        exit(-1);
    }
    if(write(fisierIesire, lastModified, strlen(lastModified)) < 0){
        perror("Eroare la scrierea ultimei modificari!\n");
        exit(-1);
    }
    if(write(fisierIesire, nrOfLinks, strlen(nrOfLinks)) < 0){
        perror("Eroare la scrierea legaturilor!\n");
        exit(-1);
    }
    if(data.st_mode & S_IRUSR){
        strcat(userAccessRight, "R");
    }
    else{
        strcat(userAccessRight, "-");
    }
    if(data.st_mode & S_IWUSR){
        strcat(userAccessRight, "W");
    }
    else{
        strcat(userAccessRight, "-");
    }
    if(data.st_mode & S_IXUSR){
        strcat(userAccessRight, "X\n");
    }
    else{
        strcat(userAccessRight, "-\n");
    }
    if(write(fisierIesire, userAccessRight, strlen(userAccessRight)) < 0){
        perror("Eroare la scrierea drepturilor de acces user!\n");
        exit(-1);
    }
    if(data.st_mode & S_IRGRP){
        strcat(groupAccessRight, "R");
    }
    else{
        strcat(groupAccessRight, "-");
    }
    if(data.st_mode & S_IWGRP){
        strcat(groupAccessRight, "W");
    }
    else{
        strcat(groupAccessRight, "-");
    }   
    if(data.st_mode & S_IXGRP){
        strcat(groupAccessRight, "X\n");
    }
    else{
        strcat(groupAccessRight, "-\n");
    }
    if(write(fisierIesire, groupAccessRight, strlen(groupAccessRight)) < 0){
        perror("Eroare la scrierea drepturilor de acces grup!\n");
        exit(-1);
    }
    if(data.st_mode & S_IROTH){
        strcat(otherAccessRight, "R");
    }
    else{
        strcat(otherAccessRight, "-");
    }
    if(data.st_mode & S_IWOTH){
        strcat(otherAccessRight, "W");
    }
    else{
        strcat(otherAccessRight, "-");
    }
    if(data.st_mode & S_IXOTH){
        strcat(otherAccessRight, "X\n");
    }
    else{
        strcat(otherAccessRight, "-\n\n");
    }
    if(write(fisierIesire, otherAccessRight, strlen(otherAccessRight)) < 0){
        perror("Eroare la scrierea drepturilor de acces altii!\n");
        exit(-1);
    }
}

void scrieNumeFisier(int fisierIesire, char *denumireFisier){
    char fileName[BUFFER_SIZE];
    sprintf(fileName, "Nume Fisier: %s\n", denumireFisier);
    if(write(fisierIesire, fileName, strlen(fileName)) < 0){
        perror("Eroare la scrierea numelui fisierului!\n");
        exit(-1);
    }
}

void parcurgeDirector(char * director, int *fisierIesire){
    DIR *dir;
    struct dirent *intrare;

    if((dir = opendir(director)) == NULL){
        perror("Eroare la deschiderea directorului!\n");
        exit(-1);
    }

    printf("Deschidere director cu succes: %s\n", director);
    *fisierIesire = creareFisier("statistica.txt");

    while ((intrare = readdir(dir)) != NULL) {
        char buffer[BUFFER_SIZE];
        sprintf(buffer, "%s/%s", director, intrare->d_name);
        printf("Element gasit: %s\n", buffer);

        struct stat fileStat;
        if (stat(buffer, &fileStat) < 0) {
            perror("Eroare la preluarea informatiilor despre element!\n");
            exit(-1);
        }

        if (S_ISREG(fileStat.st_mode)){
            if(verificareBMP(buffer)){
                char caleCompleta[BUFFER_SIZE];
                sprintf(caleCompleta, "%s/%s", director, intrare->d_name);
                int fisierBMP = open(caleCompleta, O_RDONLY);
                if (fisierBMP < 0) {
                    perror("Eroare la deschiderea fisierului BMP!\n");
                    exit(-1);
                }
                scrieNumeFisier(*fisierIesire, intrare->d_name);
                scriereDataBMP(fisierBMP, *fisierIesire);
                scriereDateFisier(fisierBMP, *fisierIesire);
                close(fisierBMP);
            }
            else
            {
                char caleCompleta[BUFFER_SIZE];
                sprintf(caleCompleta, "%s/%s", director, intrare->d_name);
                int fisierSimplu = open(caleCompleta, O_RDONLY);
                if(fisierSimplu < 0){
                    perror("Eroare la deschiderea fisierului simplu!\n");
                    exit(-1);
                }
                scrieNumeFisier(*fisierIesire, intrare->d_name);
                scriereDateFisier(fisierSimplu, *fisierIesire);
                close(fisierSimplu);
            }
        }
    }
    close(*fisierIesire);
    closedir(dir);
}

int main(int argc, char *argv[]){
    int fIn = 0, fOut = 0, rd, wr;
    char *fileName = malloc(BUFFER_SIZE * sizeof(char));

    if(verificareArgument(argc, argv[1]) == 1){
        parcurgeDirector(argv[1], &fOut);
    }

    close(fIn);
    close(fOut);
    free(fileName);

    return 0;
}