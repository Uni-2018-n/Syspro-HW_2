#include <csignal>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>

#include "fromProjectOne/Structures/virusesList.hpp"
#include "fromProjectOne/classes.hpp"
#include "parentCommands.hpp"
#include "funcs.hpp"

using namespace std;

void addVaccinationRecords(int readfds[], int writefds[], int bufferSize, int activeMonitors, int dirCount, string** toGiveDirs, int monitorPids[], string country, VirlistHeader* viruses){
    for(int i=0;i<activeMonitors;i++){
        for(int j=0;j<dirCount;j++){
            if(strcmp(toGiveDirs[i][j].c_str(), country.c_str()) == 0){
                kill(monitorPids[i], SIGUSR1);
                int tempSize= readPipeInt(readfds[i], bufferSize);
                string tempBlooms[tempSize];
                for(int j=0;j<tempSize;j++){
                    int ts = readPipeInt(readfds[i], bufferSize);
                    tempBlooms[j] = readPipe(readfds[i], ts, bufferSize);
                    writePipeInt(writefds[i], bufferSize, 0);
                }
                for(int j=0;j<tempSize;j++){
                    int k=tempBlooms[j].find("!");
                    VirlistNode* curr;
                    if((curr = viruses->searchVirus(tempBlooms[j].substr(0, k))) == NULL){
                        curr = viruses->insertVirus(tempBlooms[j].substr(0,k));
                    }
                    tempBlooms[j].erase(0,k+1);
                    curr->insertBloom(tempBlooms[j]);
                }
                return;
            }
        }
    }
}

void travelRequest(TSHeader* stats, VirlistHeader* viruses, int readfds[], int writefds[], int bufferSize, int activeMonitors, int dirCount, string** toGiveDirs, int monitorPids[], int citizenID, string date, string countryFrom, string countryTo, string virusName){
    if(viruses->searchVirus(virusName)->getBloom()->is_inside(citizenID) == 0){
        cout << "REQUEST REJECTED - YOU ARE NOT VACCINATED" << endl;
        stats->insert(0, countryFrom, date);
        return;
    }else{
        for(int i=0;i<activeMonitors;i++){
            for(int j=0;j<dirCount;j++){
                if(strcmp(countryFrom.c_str(), toGiveDirs[i][j].c_str()) ==0){
                    writePipeInt(writefds[i], bufferSize, 101);
                    writePipeInt(writefds[i], bufferSize, citizenID);
                    writePipeInt(writefds[i], bufferSize, virusName.length());
                    writePipe(writefds[i], bufferSize, virusName);
                    int vacced = readPipeInt(readfds[i], bufferSize);
                    if(vacced == 1){
                        int tempSize = readPipeInt(readfds[i], bufferSize);
                        string dateV = readPipe(readfds[i], tempSize, bufferSize);
                        int yearV, monthV, dayV;
                        int yearT, monthT, dayT;
                        char dash;
                        stringstream temp(dateV + "-" + date);
                        temp >> yearV >> dash >> monthV >> dash >> dayV >> dash
                             >> yearT >> dash >> monthT >> dash >> dayT;

                        if(((yearT - yearV)*12 + monthT - monthV) <= 6){
                            cout << "REQUEST ACCEPTED - HAPPY TRAVELS" << endl;
                            writePipeInt(writefds[i], bufferSize, 1);
                            stats->insert(1, countryFrom, date);
                            return;
                        }else if(((yearT - yearV)*12 + monthT - monthV) > 6){
                            cout << "REQUEST REJECTED - YOU WILL NEED ANOTHER VACCINATION BEFORE TRAVEL DATE" << endl;
                            writePipeInt(writefds[i], bufferSize, 0);
                            stats->insert(0, countryFrom, date);
                            return;
                        }
                    }else{
                        cout << "REQUEST REJECTED - YOU ARE NOT VACCINATED" << endl;
                        writePipeInt(writefds[i], bufferSize, 0);
                        stats->insert(0, countryFrom, date);
                        return;
                    }
                }
            }
        }
    }
    cout << "ERROR - TRAVEL REQUEST" << endl;
    return;
}



void searchVaccinationStatus(int readfds[], int writefds[], int bufferSize, int activeMonitors, int monitorPids[], int citizenID){
    for(int i=0;i<activeMonitors;i++){
        writePipeInt(writefds[i], bufferSize, 104);
        writePipeInt(writefds[i], bufferSize, citizenID);
        if(readPipeInt(readfds[i], bufferSize) == 1){
            int tempSize = readPipeInt(readfds[i], bufferSize);
            string temp = readPipe(readfds[i], tempSize, bufferSize);
            int age = readPipeInt(readfds[i], bufferSize);
            cout << temp << endl;
            cout << "AGE " << age << endl;
            int len = readPipeInt(readfds[i], bufferSize);
            for(int i=0;i<len;i++){
                tempSize = readPipeInt(readfds[i], bufferSize);
                string vir = readPipe(readfds[i], tempSize, bufferSize);
                int vacced = readPipeInt(readfds[i], bufferSize);
                if(vacced){
                    tempSize = readPipeInt(readfds[i], bufferSize);
                    string date = readPipe(readfds[i], tempSize, bufferSize);
                    cout << vir << " VACCINATED ON " << flipDate(date) << endl;
                }else{
                    cout << vir << " NOT YET VACCINATED" << endl;
                }
            }
            return;
        }
    }

}

void generateLogFileParent(int activeMonitors, int numOfCountries, string** countries, int total, int accepted, int rejected){
    ofstream file;
    file.open("log_file."+to_string(getpid()), fstream::out | fstream::trunc);
    if(file.fail()){
        perror("Parent: failed to create log_file\n");
        return;
    }
    for(int i=0;i<activeMonitors;i++){
        for(int j=0;j<numOfCountries;j++){
            file << countries[i][j] << endl;
        }
    }
    file << "TOTAL TRAVEL REQUESTS " << total << endl;
    file << "ACCEPTED " << accepted << endl;
    file << "REJECTED " << rejected << endl;
    file.close();
}