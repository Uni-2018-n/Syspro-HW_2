#ifndef _FUNCS_HPP_
#define _FUNCS_HPP_

#include <iostream>

using namespace std;

string readPipe(int fd, int size, int bufferSize);
string readPipe(int fd, int size, int bufferSize, char buffs[]);

#endif