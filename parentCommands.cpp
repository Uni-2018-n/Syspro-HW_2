#include <csignal>
#include <cstring>
#include <iostream>

#include "fromProjectOne/Structures/virusesList.hpp"
#include "parentCommands.hpp"
#include "funcs.hpp"

using namespace std;

void addVaccinationRecords(int readfds[], int writefds[], int bufferSize, int activeMonitors, int dirCount, string** toGiveDirs, int monitorPids[], string country, VirlistHeader viruses){
    int flag=0;
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
                    if((curr = viruses.searchVirus(tempBlooms[j].substr(0, k))) == NULL){
                        curr = viruses.insertVirus(tempBlooms[j].substr(0,k));
                    }
                    tempBlooms[j].erase(0,k);
                    curr->insertBloom(tempBlooms[j]);
                }
                flag = 1;
                break;
            }
            if(flag != 0){
                break;
            }
        }
        if(flag != 0){
            break;
        }
    }
}