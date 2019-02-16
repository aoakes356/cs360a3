#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <sys/sysmacros.h>
#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
int dirTraverse(char* path);  
int customError(char* message); // Always returns -1.
int main(int argc, char** argv){
    char buffer[4096];
    int i;
    if(argc > 1){ 
        if(realpath(argv[1],buffer) == NULL) {
            customError("Invalid Directory");
            return 1;
        }
        if(dirTraverse(buffer) < 0){
            customError("Main: Error traversing directories\n");
            return 1;
        }
    }else if(dirTraverse(getcwd(buffer,1023)) < 0){
        customError(NULL);
        return 1;
    }

    return 0;
}
// Traverses from the current working directory through all directories 'below' it.
int dirTraverse(char* path){
    printf("<------PATH TO BE OPENED-----> %s\n",path);
    if(access(path,R_OK) < 0){
        printf("Error Opening given directory name: %s\n",path);
        return -1; 
    }
    char file[4096] = {0};
    struct stat statbuff;
    int res = 1;
    DIR* cd = opendir(path);
    if(cd == NULL || errno != 0){
        printf("Failed to open directory: %s\n",path);
        return -1;
    }
    struct dirent* read;
    errno = 0;
    // Read all files and directories in the current directory
    // Recursively call the function again if a directory is found
    while((read = readdir(cd)) != NULL ){
        // Make sure that you are not looping
        if(!strncmp(read->d_name,".",256) || !strncmp(read->d_name,"..",256)) continue;
        // Check if the current directory is root dir, if so dont need the additional slash.
        if(!strncmp(path,"/",256)){
            sprintf(file,"%s%s",path,read->d_name);
        }else{
            sprintf(file,"%s/%s",path,read->d_name);
        }
        // Check for read permission 
        if(access(file, R_OK) < 0){
            printf("Skipping %s \n",file);
            errno = 0;
            continue;
        }
        if(lstat(file,&statbuff) < 0) {
            printf("Not readable: %s\n",file);
            return -1;
        }
        // Check the status buffer of each file to determine if it's a regular file
        switch (statbuff.st_mode & S_IFMT) {
            case S_IFDIR:  
                printf("%s\n",file);
                // print then traverse into the next directory.   
                if(dirTraverse(file) < 0){
                    return -1;
                }
                break;
            case S_IFREG:  printf("%s\n",file); break;
            default:       break;
        }
    }
    if(!res){
        printf("Error was caught in while \n");
        return -1;
    
    }
    // Close the file and check if an error caused the loop to exit.
    if(errno != 0 ){
        printf("Error was not caught in while loop?!?!??!?\n");
        return -1;
    }
    if(closedir(cd) < 0){
        printf("Not closable: %s\n",file);
        return -1; 
    }
    return 0;
}

int customError(char* message){
    if(errno != 0){
        write(2, strerror(errno), strlen(strerror(errno)));
        write(2, "\n",2);
    }if(message != NULL){
        printf("%s\n",message);
        write(2, message,strlen(message));
        write(2,"\n",2);
    }
    return -1;
}
