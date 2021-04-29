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

    int bufferSize;
    if(read(readfd, &bufferSize, sizeof(int)) > 0){
        //error
    }
    cout << "child " << getpid() << ": " << bufferSize << endl;

    char buff[bufferSize];
    int numOfCountries;
    if(bufferSize < int(sizeof(int))){
        int index;
        string temp;
        if(read(readfd, buff, bufferSize) < 0){
            //error
        }
        temp = temp + buff;
        index = bufferSize;
        while(index < int(sizeof(int))){
            if(read(readfd, buff, bufferSize) < 0){
                //error
            }
            temp = temp + buff;
            index = index + bufferSize;
        }
        numOfCountries = stoi(temp);
    }else{
        if(read(readfd, buff, bufferSize) < 0){
            //error
        }
        numOfCountries = atoi(buff);
    }
    string dirs[numOfCountries];
    for(int i=0;i<numOfCountries;i++){
        if(read(readfd, buff, bufferSize) < 0){
                //error       
        }
        if(strcmp(buff, "DirDone") == 0){
            break;
        }
        int tempSize = atoi(buff);
        string curr ="";
        if(tempSize >= bufferSize){
            if(read(readfd, buff, bufferSize) < 0){
                //error
            }
            curr = buff;
        }else{
            int index=0;
            while(index < tempSize){
                if(read(readfd, buff, bufferSize) < 0){
                //error
                }
                curr = curr+buff;
                index = bufferSize;
            }
        }
        dirs[i] = curr;
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