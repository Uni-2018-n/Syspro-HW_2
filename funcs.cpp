#include "funcs.hpp"

#include <cstdlib>
#include <fcntl.h>
#include <iostream>
#include <cstring>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
using namespace std;
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
                    //error
            }
            index = index+ bufferSize;
        }
    }else{
        if(write(fd, txt.c_str(), bufferSize) != bufferSize){
            //error
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
                //
            }
            // cout << txt.c_str()+index << endl;
            index = index + bufferSize;
        }
    }else{
        if(write(fd, txt.c_str(), bufferSize) != bufferSize){
            //error
        }
    }
}