#ifndef _FUNCS_HPP_
#define _FUNCS_HPP_

#include <iostream>

using namespace std;
extern int action;

int readPipeInt(int fd, int bufferSize);
string readPipe(int fd, int size, int bufferSize);
void writePipeInt(int fd, int bufferSize, int t);
void writePipe(int fd, int bufferSize, string txt);

#endif