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

int readPipeInt(int fd, int bufferSize){
    char* buff = new char[bufferSize];
    string temp="";
    if(bufferSize < int(sizeof(int))){
        int index =0;
        while(index <= int(sizeof(int))){
            memset(buff, 0, bufferSize);
            if(read(fd, buff, bufferSize) < 0){
                //error
            }
            if(strcmp(buff, "") == 0){
                break;
            }
            for(int k=0;k<bufferSize;k++){
                temp = temp + buff[k];
            }
            index += bufferSize;
        }
    }else{
        if(read(fd, buff, bufferSize) < 0){
            //error
        }
        temp = buff;
    }
    delete[] buff;
    return stoi(temp);
}

string readPipe(int fd, int size, int bufferSize){
    // char* buff = new char[bufferSize];
    string temp;
    if(bufferSize < size){
        int index=0;
        while(index <= size){
            // memset(buff, '\0', bufferSize);
            char buff[bufferSize];
            if(read(fd, buff, bufferSize) < 0){
                //error
            }
            int p=bufferSize;
            if(index + bufferSize > size){
                p=size-index;
            }
            for(int k=0;k<p;k++){
                temp = temp + buff[k];
            }
            index = index + bufferSize;
        }
    }else{
        char buff[bufferSize];
        if(read(fd, buff, bufferSize) < 0){
            //error
        }
        temp = buff;
    }
    // delete[] buff;
    return temp;
}