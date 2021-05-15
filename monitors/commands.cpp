#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <dirent.h>
#include "commands.hpp"

void generateLogFile(int numOfCountries, string countries[], int total, int accepted, int rejected){
    ofstream file;
    file.open("log_file."+to_string(getpid()), fstream::out | fstream::trunc);
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

void appendData(int numofcountries, string countries[], int* fileCounts, GlistHeader* main_list){
    for(int i=0;i<numofcountries;i++){
        DIR *curr_dir;
        if((curr_dir = opendir(("input_dir/"+countries[i]+'/').c_str()))== NULL){
            perror("Child: appendData Cant open dir\n");
        }
        int count=0;
        struct dirent *dirent;
        while((dirent=readdir(curr_dir)) != NULL){
            if(strcmp(dirent->d_name, ".") == 0 || strcmp(dirent->d_name, "..")==0){
                continue;
            }
            count++;
        }
        while(fileCounts[i] < count){
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