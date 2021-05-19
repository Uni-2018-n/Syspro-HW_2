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

void travelRequest(TSHeader* stats, VirlistHeader* viruses, int readfds[], int writefds[], int bufferSize, int activeMonitors, int dirCount, string** toGiveDirs, int monitorPids[], int citizenID, string date, string countryFrom, string countryTo, string virusName){
    //function for the /travelRequest command
    if(viruses->searchVirus(virusName)->getBloom()->is_inside(citizenID) == 0){//first check if we can have a quick response from the parent's bloom filters
        cout << "REQUEST REJECTED - YOU ARE NOT VACCINATED" << endl;//if we find that the citizen isnt vaccinated we simply print it
        stats->insert(0, countryFrom, date);//insert the request to the travel statistics header for later
        return;//and simply return
    }else{
        for(int i=0;i<activeMonitors;i++){//if not find the monitor that handles the countryFrom
            for(int j=0;j<dirCount;j++){
                if(strcmp(countryFrom.c_str(), toGiveDirs[i][j].c_str()) ==0){
                    writePipeInt(writefds[i], bufferSize, 101);//send a 101 protocol messege to indicate that we are at the start of a travelRequest command
                    writePipeInt(writefds[i], bufferSize, citizenID);//transfer the nessesary data to the monitor
                    writePipeInt(writefds[i], bufferSize, virusName.length());
                    writePipe(writefds[i], bufferSize, virusName);
                    int vacced = readPipeInt(readfds[i], bufferSize);//and receive a response 
                    if(vacced == 1){//if response is 1, citizen is vaccinated so read the date vaccinated
                        int tempSize = readPipeInt(readfds[i], bufferSize);
                        string dateV = readPipe(readfds[i], tempSize, bufferSize);
                        int yearV, monthV, dayV;//break the date vaccinated to 3 variables
                        int yearT, monthT, dayT;//same with travel date
                        char dash;
                        stringstream temp(dateV + "-" + date);//break it with string stream for simplicity
                        temp >> yearV >> dash >> monthV >> dash >> dayV >> dash
                             >> yearT >> dash >> monthT >> dash >> dayT;

                        if(((yearT - yearV)*12 + monthT - monthV) <= 6){//calculate the number of months between the two dates to see if the vaccination status will be in range
                            cout << "REQUEST ACCEPTED - HAPPY TRAVELS" << endl;//if it is accept the request
                            writePipeInt(writefds[i], bufferSize, 1);//inform the monitor that the request was accepted(for simplicity)
                            stats->insert(1, countryFrom, date);//insert the request to the stats structure
                            return;
                        }else if(((yearT - yearV)*12 + monthT - monthV) > 6){//if not
                            cout << "REQUEST REJECTED - YOU WILL NEED ANOTHER VACCINATION BEFORE TRAVEL DATE" << endl; //inform the user
                            writePipeInt(writefds[i], bufferSize, 0);//and the monitor
                            stats->insert(0, countryFrom, date);//and insert the request to the stats structure
                            return;
                        }
                    }else{//if the response is 0 it means that the citizen havent got vaccinated so inform and update everything needed
                        cout << "REQUEST REJECTED - YOU ARE NOT VACCINATED" << endl;
                        writePipeInt(writefds[i], bufferSize, 0);
                        stats->insert(0, countryFrom, date);
                        return;
                    }
                }
            }
        }
    }
    cout << "ERROR - TRAVEL REQUEST CountryFrom dosent exist" << endl;//incase something bad happens and countryFrom isnt inside our input_dirs
    return;
}

void addVaccinationRecords(int readfds[], int writefds[], int bufferSize, int activeMonitors, int dirCount, string** toGiveDirs, int monitorPids[], string country, VirlistHeader* viruses){
    //function for the addVaccinationRecord command
    for(int i=0;i<activeMonitors;i++){//find the monitor that has the country we have added files into
        for(int j=0;j<dirCount;j++){
            if(strcmp(toGiveDirs[i][j].c_str(), country.c_str()) == 0){
                kill(monitorPids[i], SIGUSR1);//send a SIGUSR1 signal to the monitor process
                int tempSize= readPipeInt(readfds[i], bufferSize);//and start reading the new updated bloom filters
                string tempBlooms[tempSize];
                for(int j=0;j<tempSize;j++){
                    int ts = readPipeInt(readfds[i], bufferSize);
                    tempBlooms[j] = readPipe(readfds[i], ts, bufferSize);
                    writePipeInt(writefds[i], bufferSize, 0);
                }
                for(int j=0;j<tempSize;j++){//after that update the bloom filters that we just readed from the monitor process
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
    cout << "ERROR country not found" << endl;
}

void searchVaccinationStatus(int readfds[], int writefds[], int bufferSize, int activeMonitors, int monitorPids[], int citizenID){
    //function for /searchVaccinationStatus command
    for(int i=0;i<activeMonitors;i++){//for each monitor
        writePipeInt(writefds[i], bufferSize, 104);//send a 104 protocol message
        writePipeInt(writefds[i], bufferSize, citizenID);//and the citizenID
        if(readPipeInt(readfds[i], bufferSize) == 1){//if the monitor responds with 1 it means that we've found the citizen in the monitor(so we dont need to continue with the next monitors)
            int tempSize = readPipeInt(readfds[i], bufferSize);//start reading information of the citizen and the vaccines that the citizen has done(or not)
            string temp = readPipe(readfds[i], tempSize, bufferSize);
            int age = readPipeInt(readfds[i], bufferSize);
            cout << temp << endl;
            cout << "AGE " << age << endl;
            int len = readPipeInt(readfds[i], bufferSize);//indicated how many vaccination information we have for that citizen
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
    cout << "Citizen dosent exist in any monitor" << endl;
}

void generateLogFileParent(int activeMonitors, int numOfCountries, string** countries, int total, int accepted, int rejected){
    //function that generates a log_file for the parent exactly the same with monitor's generate log file function but with a 2d array for the countries
    //used in SIGINT, SIGQUIT signals and /exit command
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