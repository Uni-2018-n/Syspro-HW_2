#ifndef _COMMANDS_HPP_
#define _COMMANDS_HPP_

#include "../fromProjectOne/generalList.hpp"
using namespace std;

void generateLogFile(int numOfCountries, string countries[], int total, int accepted, int rejected);
void appendData(int numofcountries, string path, string countries[], int* fileCounts, GlistHeader* main_list);
int handlFunctionMonitor(int readfd, int writefd, int bufferSize, int currFunc, GlistHeader* main_list);

#endif