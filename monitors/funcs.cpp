#include "funcs.hpp"
#include <cstring>

#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/errno.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>

string readPipe(int fd, int size, int bufferSize){
    char buff[bufferSize];
    string temp="";
    if(bufferSize < size){
        int index=0;
        while(index < size){
            if(read(fd, buff, bufferSize) < 0){
                //error
            }
            temp = temp + buff;
            index = index + bufferSize;
        }
    }else{
        if(read(fd, buff, bufferSize) < 0){
            //error
        }
        temp = buff;
    }
    return temp;
}

string readPipe(int fd, int size, int bufferSize, char buffs[]){
    char buff[bufferSize];
    string temp ="";
    if(bufferSize < size){
        int index;
        strcpy(buff, buffs);
        temp = temp + buff;
        index = bufferSize;
        while(index < size){
            if(read(fd, buff, bufferSize) < 0){
                //error
            }
            temp = temp + buff;
            index = index + bufferSize;
        }
    }else{
        strcpy(buff, buffs);
        temp = buff;
    }
    // cout << "test: " << temp << endl;
    return temp;
}
