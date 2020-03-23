#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#define REPORT_FILE "packages_report.txt"

//------Initializing------
int installed = 0;
int removed = 0;
int upgraded = 0;
int currentlyInstalled = 0;

struct package{
    char* name;
    char* installDate;
    char* updateDate;
    int updates;
    char* removeDate;
    char checked;
    struct package* next;
};

//-----Creating a hash table to store the packages------
struct hash{
   struct package* packages[64]; 
};

struct package* newPackage(char* name, char* date){
    struct package* p = (struct package*)malloc(sizeof(struct package));
    p->name = name;
    p->installDate = date;
    p->updateDate = "-";
    p->removeDate = "-";
    p->checked = 'F';
    p->updates = 0;
}

//------Hash table methods-----
struct hash* createHash(){
    struct hash *h = (struct hash*)malloc(sizeof(struct hash));
    return h;
}

int hashCode(char *name){
    return (strlen(name)%64);
}

void insert(struct hash* h, char* name,char* date){
    struct package* p = newPackage(name, date);
    int hashcode = hashCode(p->name);
    if(h->packages[hashcode] == NULL){
        h->packages[hashcode] = p;
        return;
    }
    else{
        struct package* tmp = h->packages[hashcode];
        while(tmp){
            if(tmp->next == NULL){
                tmp->next = p;
                return;
            }
            tmp = tmp->next;
        }
    }
}

struct package* search(struct hash* h, char* name){
    int pos = hashCode(name);
    struct package* p = h->packages[pos];
    while(p){
        if(strcmp(p->name,name) == 0){
            return p;
        }
        p = p->next;
    }
    return NULL;
}


//------Obtaining values from the file
char* getDateTime(char* line){
    char *date = malloc(sizeof(char)*19);
    for(int i=1; i<17; i++){
        date[i-1] = line[i];
    }
    return date;
}

char* getAction(char* line){ //install / remove / upgrade
    char *action = malloc(sizeof(char)*4);
    int pos = 19;
    int counter = 0;
    while(line[pos] != ' '){
        pos+= 1;
    }
    pos+= 1;
    while(counter < 3){
        action[counter] = line[pos];
        pos++;
        counter++;
    }
    return action;
}

char* getName(char* line,int offset){
    char *name = malloc(sizeof(char)*32);
    int pos = 19;
    while(line[pos] != ' '){
        pos+= 1;
    }
    pos+= offset;
    int counter = 0;
    while(line[pos] != ' '){
        name[counter] = line[pos];
        pos+= 1;
        counter +=1;
    }
    return name;
}



int readLine(int fd, char *buffer, int *state){
    memset(buffer, 0, 128*sizeof(char));
    char c[1];
    int counter = 0;
    
    if(*state == 0){
        *state = 1;
        while(read(fd, c, 1)){
            if(c[0] != '\n'){
                buffer[counter] = c[0];
            }
            else{
                return 1;
            }
            counter+=1;
        }
    }
    else if(*state == 1){     
        while(read(fd, c, 1)){
            if(c[0] != '\n'){
                buffer[counter] = c[0];
            }
            else{
                return 1;
            }
            counter+= 1;
        }
    }
    return -1;
}



//void analizeLog(char *logFile, char *report);

void analizeLog(char *logFile, char *report) {
    printf("Generating Report from: [%s] log file\n", logFile);
    struct hash* h = createHash();
    int fd, outfd;
    int buf = 512;
    char buffer[buf];
    int x = 0 ; //starting to read
    int *z = &x;
    fd=open(logFile,O_RDONLY);
    if(fd == -1){
        printf("Error\n");
        exit(-1);
    }
    int counter = 1;
    while(readLine(fd, buffer, z) > 0){
        analize(buffer, h);
    }
    close(fd);
    outfd = open(report, O_CREAT | O_WRONLY | O_TRUNC,S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
    
    //test cases
    if(outfd < 0){
        printf("Error opening the report file\n");
        exit(-1);
    }
    if(writeRegister(outfd) == -1){
        printf("Error writing the report file\n");
    }
    if(writePackages(outfd, h) == -1){
        printf("Error writing the report file\n");
    }
    close(outfd);
    printf("Report is generated at: [%s]\n", report);
}

void analize(char *buffer, struct hash* h){
    char *date;
    char *action = getAction(buffer);
    char *name;
    if(strcmp(action, "ins") == 0){
        name = getName(buffer,11);
        date = getDateTime(buffer);
        struct package *p = search(h, name);
        if(p){
            p->installDate = date;
        }
        else{
            insert(h, name, date);
            installed+= 1;
            currentlyInstalled += 1;
        }
    }
    else if(strcmp(action, "rem") == 0){
        name = getName(buffer, 9);
        date = getDateTime(buffer);
        struct package *p = search(h, name);
        if(p){
            p->removeDate = date;
            removed+= 1;
            currentlyInstalled-= 1;
        }
        else{
            exit(-1);
        }
    } 
    else if(strcmp(action,"upg") == 0){
        name = getName(buffer, 10);
        date = getDateTime(buffer);
        struct package* p = search(h, name);
        if(p){
            p->updateDate = date;
            p->updates+= 1;
            if(p->checked == 'F'){
                upgraded+= 1;
                p->checked = 'T';
            }
        }
        else{
            printf("ERROR, MISSING PACKAGE\n");
            exit(-1);
        }
    }
}

int writeRegister(int outfd){
    char first[256];
    sprintf(first,"Pacman Packages Report\n----------------------\n- Installed packages : %d\n- Removed packages   : %d\n- Upgraded packages  : %d\n- Current installed  : %d\n\n", installed, removed, upgraded, currentlyInstalled);
    if(write(outfd, first, strlen(first))==-1){
        return -1;
    }
    return 1;
}
int writePackages(int outfd,struct hash* h){
    char packageInfo[512]="List of packages\n----------------\n";
    if(write(outfd, packageInfo, strlen(packageInfo)) == -1){
        return -1;
        }
    for(int i=0; i<64; i++){    
        struct package* p = h->packages[i];
        while(p){
            memset(packageInfo, 0, 256*sizeof(char));
            sprintf(packageInfo,"- Package Name        : %s\n  - Install date      : %s\n  - Last update date  : %s\n  - How many updates  : %d\n  - Removal date      : %s\n", p->name, p->installDate, p->updateDate, p->updates, p->removeDate);
            if(write(outfd, packageInfo, strlen(packageInfo)) == -1){
                return -1;
            }
            p = p->next;
        }
    }
    return 1;
}


int main(int argc, char **argv) {
    if (argc < 2) {
	   printf("Usage:./pacman-analizer.o pacman.log\n");
	   return 1;
    }
    analizeLog(argv[1], REPORT_FILE);
    return 0;
}
