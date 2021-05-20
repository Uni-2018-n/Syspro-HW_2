#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <dirent.h>
#include "commands.hpp"
#include "../funcs.hpp"

//functions for the monitor process
void generateLogFile(int numOfCountries, string countries[], int total, int accepted, int rejected){
    //generates a log_file.pid file with all the countries and all the accepted/rejected/total request counters
    ofstream file;
    file.open("log_file."+to_string(getpid()), fstream::out | fstream::trunc);//create the file
    if(file.fail()){
        perror("Child: failed to create log_file\n");
        return;
    }
    for(int i=0;i<numOfCountries;i++){
        file << countries[i] << endl;
    }
    file << "TOTAL TRAVEL REQUESTS " << total << endl;
    file << "ACCEPTED " << accepted << endl;
    file << "REJECTED " << rejected << endl;
    file.close();
}

void appendData(int numofcountries, string path, string countries[], int* fileCounts, GlistHeader* main_list){
    //appends data to the main_list's structures
    for(int i=0;i<numofcountries;i++){
        DIR *curr_dir;
        if((curr_dir = opendir((path+ '/' +countries[i]+'/').c_str()))== NULL){//for each country
            perror("Child: appendData Cant open dir\n");
        }
        int count=0;
        struct dirent *dirent;
        while((dirent=readdir(curr_dir)) != NULL){//we count the files of the directory
            if(strcmp(dirent->d_name, ".") == 0 || strcmp(dirent->d_name, "..")==0){
                continue;
            }
            count++;
        }
        while(fileCounts[i] < count){//and if the fileCount[i] (made previously) is diffrent from the count we just calculated
                                     //it means that there is more files inside this directory so read them and insert them to the main_list
            fileCounts[i]++;
            ifstream records("input_dir/"+countries[i]+'/'+countries[i]+'-'+to_string(fileCounts[i]).c_str()+".txt");
            if(records.fail()){
                perror("Child: appendData file open ERROR\n");
            }
            string line;

            while(getline(records, line)){
                main_list->insertRecord(line, false);
            }
            records.close();
        }
        closedir(curr_dir);
    }
}

int handlFunctionMonitor(int readfd, int writefd, int bufferSize, int currFunc, GlistHeader* main_list){
    switch(currFunc){//simple switch-case for each protocol message case
        case 101:{ //brackets because cpp dosent like creating variables inside cases
                   //101 protocol message for /travelRequest command
            int id = readPipeInt(readfd, bufferSize);//read the citizenID
            int tempSize = readPipeInt(readfd, bufferSize);//and the virus name
            string vir = readPipe(readfd, tempSize, bufferSize);
            string temp = main_list->vaccineStatus(id, vir, false);//checks if the citizen is vaccinated(false means that we dont want to print something from this function) and returns "" if citizen not vaccinated or a date if vaccinated
            if(temp.length() != 0){//handle everything as planed if vaccinated or not
                writePipeInt(writefd, bufferSize, 1);
                writePipeInt(writefd, bufferSize, temp.length());
                writePipe(writefd, bufferSize, temp);
            }else{
                writePipeInt(writefd, bufferSize, 0);
            }
            return readPipeInt(readfd, bufferSize);//read the 1 or 0 that indicated if the request was accepted or rejected
        }
        case 104:{//104 case is for  /searchVaccinationStatus command
            int id = readPipeInt(readfd, bufferSize);//read the citizenID
            SRListHeader* returnedViruses = main_list->vaccineStatus(id, false);//vaccineStatus function with boolean returns a SRListHeader list header with all the vaccinations that a citizen has done instead of printing messages
            if(returnedViruses != NULL && returnedViruses->len > 0){//check if the citizen exists
                writePipeInt(writefd, bufferSize, 1);//send 1 to indicate that the citizen exists here
                string temp = to_string(*(returnedViruses->citizen->citizenId)) + " " + returnedViruses->citizen->firstName + " " + returnedViruses->citizen->lastName + " " + returnedViruses->citizen->getCountry();//format the information string(id, name, country)
                writePipeInt(writefd, bufferSize, temp.length());
                writePipe(writefd, bufferSize, temp);
                writePipeInt(writefd, bufferSize, returnedViruses->citizen->getAge());//send the citizen's age
                writePipeInt(writefd, bufferSize, returnedViruses->len);//and send how many vaccinations the citizen has done
                for(int i=0;i<returnedViruses->len;i++){//for each vaccination
                    SRListNode* poped = returnedViruses->pop();
                    writePipeInt(writefd, bufferSize, poped->vir.length());//send the information to the parent(0 or 1 and date if 1)
                    writePipe(writefd, bufferSize, poped->vir);
                    writePipeInt(writefd, bufferSize, poped->vacced);
                    if(poped->vacced == 1){
                        writePipeInt(writefd, bufferSize, poped->date.length());
                        writePipe(writefd, bufferSize, poped->date);
                    }
                    delete poped;
                }
            }else{
                writePipeInt(writefd, bufferSize, 0);//else simple send 0 for the case that the citizen dosent exist
            }
            delete returnedViruses;
            return -1;
        }
    }
    return -1;
}