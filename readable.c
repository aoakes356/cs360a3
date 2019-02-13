#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <sys/sysmacros.h>
#include <stdio.h>
#include <stdlib.h>



int dirTraverse();

/* Recursively get each file
 * 1. Get the current working directory
 * 2. Create a file status buffer (shouldn't need to malloc).
 * 3. use opendir on the current directory
 * 4. For each file in the current working directory
 *  - if it's a file that is readable by the current process, print its name.
 *  - use stat to check if it is a symbolic link/valid permissions
 *  - if not, print the dir, chdir() to that directory and call the function again
*/  


int main(int argc, char** argv){
    if(argc > 1){
        if(chdir(argv[1]) < 0){
            write(2, strerror(errno), strlen(strerror(errno)));
            write(2, "\n",2);
            printf("Error with given directory name: %s\n",strerror(errno));
            return 1; 
        }
    }
    if(dirTraverse() < 0){
        printf("Error!\n");
    }
    return 0;
}

// Traverses from the current working directory through all directories 'below' it.
int dirTraverse(){
    char buffer[1024] = {0};
    char file[1023] = {0};
    getcwd(buffer, 1023); // buffer and dir point to the same thing.
    struct stat statbuff;
    DIR* cd = opendir(buffer);
    struct dirent* read;
    errno = 0;
    while((read = readdir(cd)) != NULL && errno == 0){
        if(!strncmp(read->d_name,".",2) || !strncmp(read->d_name,"..",3)) continue;
        sprintf(file,"%s/%s",buffer,read->d_name);
        if(lstat(file,&statbuff) == -1) {
            // Error handling goes here.
            write(2, strerror(errno), strlen(strerror(errno)));
            write(2, "\n",2);
            return -1;
        }
        // From useful example in inode docs.
        switch (statbuff.st_mode & S_IFMT) {
            case S_IFDIR:  
                printf("d:%s\n",file);   
                if(chdir(file) < 0){
                    write(2, strerror(errno), strlen(strerror(errno)));
                    write(2, "\n",2);
                    return -1; 
                } 
                dirTraverse();
                break;
            case S_IFLNK:  printf("s:%s\n",file); break;
            case S_IFREG:  printf("f:%s\n",file);   break;
            default:       break;
        }
        errno = 0;
    }
    return 1;
}


