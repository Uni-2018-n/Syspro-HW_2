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
                if(action != 0){
                    return -1;
                }
                perror("error readPipeInt");
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
            if(action != 0){
                return -1;
            }
            perror("error readPipeint");
        }
        temp = buff;
    }
    delete[] buff;
    if(temp.length() == 0){
        return -1;
    }
    return stoi(temp);
}

string readPipe(int fd, int size, int bufferSize){
    string temp;
    if(bufferSize < size){
        int index=0;
        while(index <= size){
            char buff[bufferSize];
            if(read(fd, buff, bufferSize) < 0){
                if(action != 0){
                    return "";
                }
                cout << "error readPipe" << endl;
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
            if(action != 0){
                return "";
            }
            cout << "error readPipe" << endl;
        }
        temp = buff;
    }
    return temp;
}


void writePipeInt(int fd, int bufferSize, int t){
    string txt = to_string(t);
    char* buff = new char[bufferSize];
    if(bufferSize < int(sizeof(int))){
        if(txt.length() < sizeof(int)){
            int l = sizeof(int)-txt.length();
            for(int i=0;i<l;i++){
                txt = "0" + txt;
            }
        }
        int index = 0;
        while(index <= int(sizeof(int))){
            memset(buff, '\0', bufferSize);
            strncpy(buff, txt.c_str()+index, bufferSize);
            if(write(fd, buff, bufferSize) != bufferSize){
                if(action != 0){
                    return;
                }
                cout << "error writePipeInt" << endl;
            }
            index = index+ bufferSize;
        }
    }else{
        if(write(fd, txt.c_str(), bufferSize) != bufferSize){
            if(action != 0){
                return;
            }
            cout << "error writePipeInt" << endl;
        }
    }
    delete[] buff;
}

void writePipe(int fd, int bufferSize, string txt){
    int size = txt.length();
    if(bufferSize < size){
        int index =0;
        string test = "";
        while(index <= size){
            if(write(fd, txt.c_str()+index, bufferSize) != bufferSize){
                if(action != 0){
                    return;
                }
                cout << "error writePipe" << endl;
            }
            index = index + bufferSize;
        }
    }else{
        if(write(fd, txt.c_str(), bufferSize) != bufferSize){
            if(action != 0){
                return;
            }
            cout << "error writePipe" << endl;
        }
    }
}