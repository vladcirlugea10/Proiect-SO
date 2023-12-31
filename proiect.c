#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <time.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <ctype.h>
#define BUFFER_SIZE 512
#define READ_BUFFER_SIZE 2

int verificareArgument(int nrArgumente, char* directorIntrare, char* directorIesire, char c){
    struct stat data, data1;
    DIR *dirInt, *dirIes;

    if((nrArgumente != 4)){
        perror("Usage ./program <directorIntrare> <directorIesire> <caracterAlfanumeric>\n");
        exit(-1);
    }
    if(isalnum(c)){
        perror("Caracterul nu este alfanumeric!\n");
        exit(-1);
    }
    if((dirInt = opendir(directorIntrare)) == NULL){
        perror("Eroare la deschiderea directorului intrare!\n");
        exit(-1);
    }
    if(lstat(directorIntrare, &data) < 0){
        perror("Eroare la obtinerea informatiilor despre directorul intrare!\n");
        exit(-1);
    }
    if((dirIes = opendir(directorIesire)) == NULL){
        perror("Eroare la deschiderea directorului iesire!\n");
        exit(-1);
    }
    if(lstat(directorIesire, &data1) < 0){
        perror("Eroare la obtinerea informatiilor despre directorul iesire!\n");
        exit(-1);
    }
    int m1 = data1.st_mode;
    int m = data.st_mode;
    if(S_ISDIR(m) && S_ISDIR(m1)){
        printf("Argumente Ok!\n");
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


int creareFisier(char* director, char *denumire){
    int fisier;
    char caleFisier[BUFFER_SIZE];
    sprintf(caleFisier, "%s/%s", director, denumire);
    if((fisier = open(caleFisier, O_WRONLY | O_TRUNC |O_CREAT, S_IRWXU)) < 0){
        perror("Eroare la crearea fisierului!\n");
        exit(-1);
    }
    else
    {
        printf("Fisier creat!\n");
    }
    return fisier;
}

void scriereDataBMP(int fisierIntrare, int fisierIesire) {
    char buffer[BUFFER_SIZE];

    if (read(fisierIntrare, buffer, 54) != 54) {
        perror("Eroare la citirea header-ului BMP!\n");
        exit(-1);
    }
    int width = *(int*)&buffer[18];
    int height = *(int*)&buffer[22];
    int size = *(int*)&buffer[2];

    char widthString[BUFFER_SIZE];
    char w[BUFFER_SIZE] = "Latime: ";
    sprintf(widthString, "%d\n", width);
    strcat(w, widthString);
    char heightString[BUFFER_SIZE];
    char h[BUFFER_SIZE] = "Inaltime: ";
    sprintf(heightString, "%d\n", height);
    strcat(h, heightString);
    char sizeString[BUFFER_SIZE];
    char s[BUFFER_SIZE] = "Dimensiune: ";
    sprintf(sizeString, "%d octeti\n", size);
    strcat(s, sizeString);

    write(fisierIesire, s, strlen(s));
    write(fisierIesire, h, strlen(h));
    write(fisierIesire, w, strlen(w));

    printf("Scriere terminata\n");
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
    sprintf(fileSize, "Dimensiune: %ld octeti\n", data.st_size);
    sprintf(userID, "Identificatorul Utilizatorului: %d\n", data.st_uid);
    sprintf(lastModified, "Ultima modificare: %s", ctime(&data.st_atime));
    sprintf(nrOfLinks, "Contorul de legaturi: %ld\n", data.st_nlink);
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
        strcat(otherAccessRight, "X\n\n");
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

void scrieLegaturaSimbolica(int fisierIesire, char *legatura, char *fisierTarget, char *director) {
    char linkName[BUFFER_SIZE];
    char linkSize[BUFFER_SIZE];
    char targetSize[BUFFER_SIZE];
    int fTarget = 0;

    sprintf(linkName, "Nume legatura: %s\n", legatura);
    char caleCompleta[BUFFER_SIZE];
    sprintf(caleCompleta, "%s/%s", director, fisierTarget);

    if((fTarget = open(caleCompleta, O_RDONLY)) < 0){
        perror("Eroare la deschiderea fisierului target!\n");
        exit(-1);
    }

    struct stat linkStat, targetStat;
    if (lstat(legatura, &linkStat) < 0) {
        perror("Eroare la preluarea informatiilor despre legatura simbolica!\n");
        exit(-1);
    }
    if (fstat(fTarget, &targetStat) < 0) {
        perror("Eroare la preluarea informatiilor despre fisierul target!\n");
        exit(-1);
    }

    sprintf(linkSize, "Dimensiune legatura: %ld octeti\n", linkStat.st_size);
    sprintf(targetSize, "Dimensiune fisier target: %ld octeti\n", targetStat.st_size);

    if (write(fisierIesire, linkName, strlen(linkName)) < 0) {
        perror("Eroare la scrierea numelui legaturii!\n");
        exit(-1);
    }
    if (write(fisierIesire, linkSize, strlen(linkSize)) < 0) {
        perror("Eroare la scrierea dimensiunii legaturii!\n");
        exit(-1);
    }
    if (write(fisierIesire, targetSize, strlen(targetSize)) < 0) {
        perror("Eroare la scrierea dimensiunii fisierului target!\n");
        exit(-1);
    }

    char userAccessRight[BUFFER_SIZE] = "Drepturi de acces user legatura: ";
    char groupAccessRight[BUFFER_SIZE] = "Drepturi de acces grup legatura: ";
    char otherAccessRight[BUFFER_SIZE] = "Drepturi de acces altii legatura: ";

    if (linkStat.st_mode & S_IRUSR) {
        strcat(userAccessRight, "R");
    } else {
        strcat(userAccessRight, "-");
    }
    if (linkStat.st_mode & S_IWUSR) {
        strcat(userAccessRight, "W");
    } else {
        strcat(userAccessRight, "-");
    }
    if (linkStat.st_mode & S_IXUSR) {
        strcat(userAccessRight, "X\n");
    } else {
        strcat(userAccessRight, "-\n");
    }

    if (write(fisierIesire, userAccessRight, strlen(userAccessRight)) < 0) {
        perror("Eroare la scrierea drepturilor de acces user pentru legatura!\n");
        exit(-1);
    }

    if (linkStat.st_mode & S_IRGRP) {
        strcat(groupAccessRight, "R");
    } else {
        strcat(groupAccessRight, "-");
    }
    if (linkStat.st_mode & S_IWGRP) {
        strcat(groupAccessRight, "W");
    } else {
        strcat(groupAccessRight, "-");
    }
    if (linkStat.st_mode & S_IXGRP) {
        strcat(groupAccessRight, "X\n");
    } else {
        strcat(groupAccessRight, "-\n");
    }

    if (write(fisierIesire, groupAccessRight, strlen(groupAccessRight)) < 0) {
        perror("Eroare la scrierea drepturilor de acces grup pentru legatura!\n");
        exit(-1);
    }

    if (linkStat.st_mode & S_IROTH) {
        strcat(otherAccessRight, "R");
    } else {
        strcat(otherAccessRight, "-");
    }
    if (linkStat.st_mode & S_IWOTH) {
        strcat(otherAccessRight, "W");
    } else {
        strcat(otherAccessRight, "-");
    }
    if (linkStat.st_mode & S_IXOTH) {
        strcat(otherAccessRight, "X\n\n");
    } else {
        strcat(otherAccessRight, "-\n\n");
    }

    if (write(fisierIesire, otherAccessRight, strlen(otherAccessRight)) < 0) {
        perror("Eroare la scrierea drepturilor de acces altii pentru legatura!\n");
        exit(-1);
    }
}

void scrieDirector(int fisierIesire, char *director) {
    char dirName[BUFFER_SIZE];
    char userID[BUFFER_SIZE];
    char userAccessRight[BUFFER_SIZE] = "Drepturi de acces user: ";
    char groupAccessRight[BUFFER_SIZE] = "Drepturi de acces grup: ";
    char otherAccessRight[BUFFER_SIZE] = "Drepturi de acces altii: ";

    sprintf(dirName, "Nume director: %s\n", director);

    struct stat dirStat;
    if (stat(director, &dirStat) < 0) {
        perror("Eroare la preluarea informatiilor despre director!\n");
        exit(-1);
    }

    sprintf(userID, "Identificatorul Utilizatorului: %d\n", dirStat.st_uid);

    if (write(fisierIesire, dirName, strlen(dirName)) < 0) {
        perror("Eroare la scrierea numelui directorului!\n");
        exit(-1);
    }
    if (write(fisierIesire, userID, strlen(userID)) < 0) {
        perror("Eroare la scrierea identificatorului utilizatorului pentru director!\n");
        exit(-1);
    }

    if (dirStat.st_mode & S_IRUSR) {
        strcat(userAccessRight, "R");
    } else {
        strcat(userAccessRight, "-");
    }
    if (dirStat.st_mode & S_IWUSR) {
        strcat(userAccessRight, "W");
    } else {
        strcat(userAccessRight, "-");
    }
    if (dirStat.st_mode & S_IXUSR) {
        strcat(userAccessRight, "X\n");
    } else {
        strcat(userAccessRight, "-\n");
    }
    if (write(fisierIesire, userAccessRight, strlen(userAccessRight)) < 0) {
        perror("Eroare la scrierea drepturilor de acces user pentru director!\n");
        exit(-1);
    }

    if (dirStat.st_mode & S_IRGRP) {
        strcat(groupAccessRight, "R");
    } else {
        strcat(groupAccessRight, "-");
    }
    if (dirStat.st_mode & S_IWGRP) {
        strcat(groupAccessRight, "W");
    } else {
        strcat(groupAccessRight, "-");
    }
    if (dirStat.st_mode & S_IXGRP) {
        strcat(groupAccessRight, "X\n");
    } else {
        strcat(groupAccessRight, "-\n");
    }
    if (write(fisierIesire, groupAccessRight, strlen(groupAccessRight)) < 0) {
        perror("Eroare la scrierea drepturilor de acces grup pentru director!\n");
        exit(-1);
    }

    if (dirStat.st_mode & S_IROTH) {
        strcat(otherAccessRight, "R");
    } else {
        strcat(otherAccessRight, "-");
    }
    if (dirStat.st_mode & S_IWOTH) {
        strcat(otherAccessRight, "W");
    } else {
        strcat(otherAccessRight, "-");
    }
    if (dirStat.st_mode & S_IXOTH) {
        strcat(otherAccessRight, "X\n\n");
    } else {
        strcat(otherAccessRight, "-\n\n");
    }
    if (write(fisierIesire, otherAccessRight, strlen(otherAccessRight)) < 0) {
        perror("Eroare la scrierea drepturilor de acces altii pentru director!\n");
        exit(-1);
    }
}

void modificaCuloare(char *fisierIntrare){
    int fIn = open(fisierIntrare, O_RDWR);

    if(fIn < 0){
        perror("Eroare la deshiderea imaginilor!\n");
        exit(-1);
    }
    unsigned char headerBMP[54];
    read(fIn, headerBMP, 54);

    int latime = *(int*)&headerBMP[18];
    int inaltime = *(int*)&headerBMP[22];
    int offset = 0;
    if(((latime * 3) % 4) != 0){
        offset = 4 - ((latime * 3) % 4);
    }

    unsigned char pixel[3];
    double pixelGri;
    lseek(fIn, 54, SEEK_SET);

    for(int i = 0; i<inaltime; i++){
        for(int j = 0; j<latime; j++){
            read(fIn, pixel, 3);
            
            pixelGri = 0.299*pixel[2] + 0.587*pixel[1] + 0.114*pixel[0];
            pixel[0] = (unsigned char)pixelGri;
            pixel[1] = (unsigned char)pixelGri;
            pixel[2] = (unsigned char)pixelGri;
            lseek(fIn, -3, SEEK_CUR);
            write(fIn, pixel, 3);
        }
        lseek(fIn, offset, SEEK_CUR);
    }
    close(fIn);
}
void pipeFiuFiu(int ff[2], int fp[2], char *fisier){
    if(close(ff[0]) < 0)
    {
        perror("Eroare la inchidere pipe fiu-fiu!\n");
        exit(-1);
    }
    if(close(fp[0]) < 0)
    {
        perror("Eroare la inchidere pipe fiu-parinte!\n");
        exit(-1);
    }
    if(close(fp[1]) < 0)
    {
        perror("Eroare la inchidere pipe fiu-parinte!\n");
        exit(-1);
    }
    int fis = open(fisier, O_RDONLY);
    if(fis < 0)
    {
        perror("Eroare la deschiderea fisierului!\n");
        exit(-1);
    }
    char buffer[BUFFER_SIZE];
    int rd = -1;
    while((rd = read(fis, buffer, 256)) > 0)
    {
        write(ff[1], &buffer, rd);
    }
    if(close(ff[1]) < 0)
    {
        perror("Eroare la inchiderea scrierii fiu-fiu!\n");
    }
}
void pipeFiuParinte(int ff[2], int fp[2], char c){
    if (close(fp[0]) < 0) {
        perror("Eroare la inchiderea pipe fiu-parinte!\n");
        exit(-1);
    }
    if (close(ff[1]) < 0) {
        perror("Eroare la inchiderea pipe fiu-fiu!\n");
        exit(-1);
    }
    if (dup2(ff[0], 0) < 0) {
        perror("Eroare la redirectarea citirii!\n");
        exit(-1);
    }
    if (close(ff[0]) < 0) {
        perror("Eroare la inchiderea pipe fiu-fiu!\n");
        exit(-1);
    }

    if (dup2(fp[1], STDERR_FILENO) < 0) {
        perror("Eroare la redirectarea scrierii!\n");
        exit(-1);
    }
    if (close(fp[1]) < 0) {
        perror("Eroare la inchiderea pipe fiu-parinte!\n");
        exit(-1);
    }
    char script[100] = "./script.sh ";
    strcat(script, c);
    
    execlp("/bin/bash", "/bin/bash", script, NULL);
    perror("Error executing the script!\n");
    exit(-1);
}
void parcurgeDirector(char *directorIntrare, char *directorIesire, char c) {
    DIR *dir, *dirIesire;
    struct dirent *intrare;
    int nrCopii = 0, status;

    if ((dir = opendir(directorIntrare)) == NULL) {
        perror("Eroare la deschiderea directorului intrare!\n");
        exit(-1);
    }
    printf("Director intrare deschis: %s\n", directorIntrare);

    if ((dirIesire = opendir(directorIesire)) == NULL) {
        perror("Eroare la deschiderea directorului iesire!\n");
        exit(-1);
    }
    printf("Director iesire deschis: %s\n", directorIesire);
    int fp[2], ff[2];
    if(pipe(fp) < 0)
    {
        perror("Eroare la pipe fiu-parinte!\n");
        exit(-1);
    }
    if(pipe(ff) < 0)
    {
        perror("Eroare la pipe fiu-fiu!\n");
        exit(-1);
    }
    while ((intrare = readdir(dir)) != NULL) {
        if (strcmp(intrare->d_name, ".") == 0 || strcmp(intrare->d_name, "..") == 0) {
            continue;
        }
        char buffer[BUFFER_SIZE];
        sprintf(buffer, "%s/%s", directorIntrare, intrare->d_name);
        printf("Element gasit: %s\n", buffer);

        char fisierSatistica[BUFFER_SIZE];
        sprintf(fisierSatistica, "%s_statistica.txt", intrare->d_name);
        int fisierIesire = creareFisier(directorIesire, fisierSatistica);
        struct stat fileStat;
        if (lstat(buffer, &fileStat) < 0) {
            perror("Eroare la preluarea informatiilor despre element!\n");
            exit(-1);
        }

        if (S_ISLNK(fileStat.st_mode)) {
            int pidLnk;
            if((pidLnk = fork()) < 0){
                perror("Eroare la crearea procesului fiu!\n");
                exit(-1);
            }
            ++nrCopii;
            if(pidLnk == 0){
                char target[BUFFER_SIZE];
                int octetiCititi;
                if ((octetiCititi = readlink(buffer, target, sizeof(target) - 1)) != -1) {
                    target[octetiCititi] = '\0';
                    printf("Legatura simbolica: %s -> %s\n", buffer, target);
                    scrieLegaturaSimbolica(fisierIesire, buffer, target, directorIntrare);
                } else {
                    perror("Eroare la citirea legaturii simbolice!\n");
                    exit(-1);
                }
                exit(nrCopii);
            }
        } else if (S_ISREG(fileStat.st_mode)) {
            if (verificareBMP(buffer)) {
                int pidBMP;
                if((pidBMP = fork()) < 0){
                    perror("Eroare la crearea procesului fiu!\n");
                    exit(-1);
                }
                ++nrCopii;
                if(pidBMP == 0){
                    char caleCompleta[BUFFER_SIZE];
                    sprintf(caleCompleta, "%s/%s", directorIntrare, intrare->d_name);
                    int fisierBMP = open(caleCompleta, O_RDONLY);
                    if (fisierBMP < 0) {
                        perror("Eroare la deschiderea fisierului BMP!\n");
                        exit(-1);
                    }
                    scrieNumeFisier(fisierIesire, intrare->d_name);
                    scriereDataBMP(fisierBMP, fisierIesire);
                    scriereDateFisier(fisierBMP, fisierIesire);
                    modificaCuloare(caleCompleta);
                    close(fisierBMP);
                    exit(nrCopii);
                }
            } else {
                int pidNotBMP;
                for(int j=0; j<2; j++){
                    if((pidNotBMP = fork()) < 0){
                        perror("Eroare la crearea procesului fiu!\n");
                        exit(-1);
                    }
                    ++nrCopii;
                    if(pidNotBMP == 0){
                        if(j == 0){
                            char caleCompleta[BUFFER_SIZE];
                            sprintf(caleCompleta, "%s/%s", directorIntrare, intrare->d_name);
                            int fisierSimplu = open(caleCompleta, O_RDONLY);
                            if (fisierSimplu < 0) {
                                perror("Eroare la deschiderea fisierului simplu!\n");
                                exit(-1);
                            }
                            scrieNumeFisier(fisierIesire, intrare->d_name);
                            scriereDateFisier(fisierSimplu, fisierIesire);
                            pipeFiuFiu(ff, fp, caleCompleta);
                            close(fisierSimplu);
                        } else {
                            pipeFiuParinte(ff, fp, c);
                        }
                        exit(nrCopii);
                    }
                }
            }
        } else if (S_ISDIR(fileStat.st_mode)) {
            int pidDIR;
            if((pidDIR = fork()) < 0){
                perror("Eroare la crearea procesului fiu!\n");
                exit(-1);
            }
            ++nrCopii;
            if(pidDIR == 0){
                printf("Este director: %s\n", buffer);
                scrieDirector(fisierIesire, buffer);
                exit(nrCopii);
            }
        }
        close(fisierIesire);
        sleep(1);
    }
    for(int l = 0; l < nrCopii; l++){
        int waitstatus = wait(&status);
        if(waitstatus < 0){
            exit(-1);
        }
        if(WIFEXITED(status)){
            printf("Procesul cu numarul %d s-a incheiat cu codul %d!\n", waitstatus, WEXITSTATUS(status));
        }
    }
    nrCopii = 0;
}
int main(int argc, char *argv[]){
    if(verificareArgument(argc, argv[1], argv[2], argv[3]) == 1){
        parcurgeDirector(argv[1], argv[2], argv[3]);
    }
    return 0;
}