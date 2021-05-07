#ifndef _COMMANDS_HPP_
#define _COMMANDS_HPP_

#include <iostream>
#include "../fromProjectOne/generalList.hpp"
using namespace std;

void generateLogFile(int numOfCountries, string countries[], int total, int accepted, int rejected);
void appendData(int numofcountries, string countries[], int* fileCounts, GlistHeader* main_list);
#endif