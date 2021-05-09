#ifndef _PARENTCOMMANDS_HPP_
#define _PARENTCOMMANDS_HPP_

#include "fromProjectOne/Structures/virusesList.hpp"
#include <iostream>
#include <string>

using namespace std;

void travelRequest(VirlistHeader* viruses, int readfds[], int writefds[], int bufferSize, int activeMonitors, int dirCount, string** toGiveDirs, int monitorPids[], int citizenID, string date, string countryFrom, string countryTo, string virusName);
void addVaccinationRecords(int readfds[], int writefds[], int bufferSize, int activeMonitors, int dirCount, string** toGiveDirs, int monitorPids[], string country, VirlistHeader* viruses);
void searchVaccinationStatus(int readfds[], int writefds[], int bufferSize, int activeMonitors, int monitorPids[], int citizenID);


#endif