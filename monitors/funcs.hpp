#ifndef _FUNCS_HPP_
#define _FUNCS_HPP_

#include <iostream>

using namespace std;

int readPipeInt(int fd, int bufferSize);
string readPipe(int fd, int size, int bufferSize);
string readPipe(int fd, int size, int bufferSize, char buffs[]);

#endif