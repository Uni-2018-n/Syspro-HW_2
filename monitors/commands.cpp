#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <dirent.h>
#include "commands.hpp"
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