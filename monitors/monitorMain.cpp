#include <iostream>
#include <cstring>

#include <stdio.h>
#include <stdlib.h>

#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/errno.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>

#include <dirent.h>

#include "funcs.hpp"

using namespace std;

int main(int argc, const char** argv) {
    int readfd, writefd;
    if((writefd = open(argv[0], O_WRONLY)) < 0){
        cout << "error" << endl;
    }
    if((readfd = open(argv[1], O_RDONLY)) < 0){
        cout << "Error" << endl;
    }

    int bufferSize;
    if(read(readfd, &bufferSize, sizeof(int)) > 0){
        //error
    }

    char buff[bufferSize];
    int numOfCountries =stoi(readPipe(readfd, int(sizeof(int)), bufferSize));
    string dirs[numOfCountries];
    for(int i=0;i<numOfCountries;i++){
        if(read(readfd, buff, bufferSize) < 0){
                cout << "error" << endl;
        }
        if(strcmp(buff, "DirDone") == 0){
            break;
        }
        int tempSize = stoi(readPipe(readfd, int(sizeof(int)), bufferSize, buff));
        dirs[i] = readPipe(readfd, tempSize, bufferSize);
    }

    // for(int i=0;i<numOfCountries;i++){
    //     cout << getpid() << ": " << dirs[i] << endl;
    // }

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