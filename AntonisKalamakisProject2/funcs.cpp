#include "funcs.hpp"
#include <cstring>

#include <unistd.h>

int readPipeInt(int fd, int bufferSize){//reading an interger at the fd file descriptor and with bufferSize buffer
    char* buff = new char[bufferSize];
    string temp="";
    if(bufferSize < int(sizeof(int))){//if we have a smaller bufferSize we need to break the data into parts
        int index =0;
        while(index <= int(sizeof(int))){//untill we have read sizeof(int) bytes, 
            memset(buff, 0, bufferSize);//clear the memory of our buffer array
            if(read(fd, buff, bufferSize) < 0){//read bufferSize bytes
                if(action != 0){//incase read fails and action variable is != 0, this means that we have a signal to run
                    return -1;
                }
                perror("readPipeInt data read ERROR\n");//if not its a read malfunction
            }
            if(strcmp(buff, "") == 0){//check if its the end of the data stream
                break;
            }
            for(int k=0;k<bufferSize;k++){//and copy the data just read to the temp string to do stoi later
                temp = temp + buff[k];
            }
            index += bufferSize;//and update the index variable
        }
    }else{
        if(read(fd, buff, bufferSize) < 0){//if we have a bufferSize >= with an int its as simple as one read function to read the data
            if(action != 0){
                return -1;
            }
            perror("readPipeInt Data read ERROR\n");
        }
        temp = buff;
    }
    delete[] buff;
    if(temp.length() == 0){//incase there was a problem reading retrun -1, to solve stoi issues
        return -1;
    }
    return stoi(temp);
}

string readPipe(int fd, int size, int bufferSize){//function for reading a string
    //size is the size of string we are going to read, bufferSize is the size of buffer
    string temp;
    if(bufferSize < size){
        int index=0;
        while(index <= size){//same technique with the read int function
            char buff[bufferSize];
            if(read(fd, buff, bufferSize) < 0){
                if(action != 0){
                    return "";
                }
                perror("readPipe data read ERROR\n");
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
    }else{//case we have a bigger buffer than the size of our string
        char buff[bufferSize];
        if(read(fd, buff, bufferSize) < 0){//one simple read
            if(action != 0){
                return "";
            }
            perror("readPipe Data read ERROR\n");
        }
        temp = buff;
    }
    return temp;
}


void writePipeInt(int fd, int bufferSize, int t){//t is the int we want to send 
    string txt = to_string(t);//make it a string for simplicity reasons
    char* buff = new char[bufferSize];
    if(bufferSize < int(sizeof(int))){
        if(txt.length() < sizeof(int)){//if we have less digits than 4 (since sizeof(int) = 4)
            int l = sizeof(int)-txt.length();
            for(int i=0;i<l;i++){//append as many 0 needed in front of the string to be able to send 4 bytes(because the receiving end always reads 4 bytes)
                txt = "0" + txt;
            }
        }
        int index = 0;
        while(index <= int(sizeof(int))){//write each time bufferSize bytes untill we have sended all the data
            memset(buff, '\0', bufferSize);
            strncpy(buff, txt.c_str()+index, bufferSize);
            if(write(fd, buff, bufferSize) != bufferSize){
                if(action != 0){
                    return;
                }
                perror("writePipeInt data write ERROR\n");
            }
            index = index+ bufferSize;
        }
    }else{//case bufferSize is enough to send the integer
        if(write(fd, txt.c_str(), bufferSize) != bufferSize){
            if(action != 0){
                return;
            }
            perror("writePipeInt Data write ERROR\n");
        }
    }
    delete[] buff;
}

void writePipe(int fd, int bufferSize, string txt){//txt is the data we want to send, bufferSize is the buffer we can send
    int size = txt.length();
    if(bufferSize < size){
        int index =0;
        string test = "";
        while(index <= size){//break string to parts of bufferSize, send the data untill we've send the message
            if(write(fd, txt.c_str()+index, bufferSize) != bufferSize){
                if(action != 0){
                    return;
                }
                perror("writePipe data write ERROR\n");
            }
            index = index + bufferSize;
        }
    }else{//incase bufferSize is enough to send the data
        if(write(fd, txt.c_str(), bufferSize) != bufferSize){
            if(action != 0){
                return;
            }
            perror("writePipe Data write ERROR\n");
        }
    }
}