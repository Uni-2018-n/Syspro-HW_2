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

void writePipe(int fd, int size, int bufferSize, string txt){
    char buff[bufferSize];
    if(bufferSize < size){ 
            strncpy(buff, txt.c_str(), bufferSize);
            if(write(fd, buff, bufferSize) != bufferSize){
                //error
            }
            int index = bufferSize+1;
            while(index < sizeof(int)){
                strncpy(buff, txt.c_str()+index, bufferSize);
                if(write(fd, buff, bufferSize) != bufferSize){
                    //error
                }
                index = index + bufferSize +1;
            }
        }else{
            if(write(fd, txt.c_str(), bufferSize) != bufferSize){
                //error
            }
        }
}