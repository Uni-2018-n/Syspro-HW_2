#ifndef _PARENTCOMMANDS_HPP_
#define _PARENTCOMMANDS_HPP_

#include "fromProjectOne/Structures/virusesList.hpp"
#include <iostream>

using namespace std;

void addVaccinationRecords(int readfds[], int writefds[], int bufferSize, int activeMonitors, int dirCount, string** toGiveDirs, int monitorPids[], string country, VirlistHeader viruses);

#endif