#include <iostream>
#include <cstring>

#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/errno.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>

#include <dirent.h>


using namespace std;

int main(int argc, const char** argv) {
    int readfd, writefd;
    if((writefd = open(argv[0], O_WRONLY)) < 0){
        cout << "error" << endl;
    }
    if((readfd = open(argv[1], O_RDONLY)) < 0){
        cout << "Error" << endl;
    }

    


    close(readfd);
    close(writefd);
    if(unlink(argv[0]) <0){
        cout << "cant unlink" << endl;
    }
    if(unlink(argv[1]) <0){
        cout << "cant unling" << endl;
    }
    return 0;
}