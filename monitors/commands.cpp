#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h>

#include "commands.hpp"

void generateLogFile(int numOfCountries, string countries[], int total, int accepted, int rejected){
    cout << "im here" << endl;
    ofstream file;
    file.open("log_file."+to_string(getpid()), fstream::out | fstream::trunc);
    if(file.fail()){
        cout << "failed to create file" << endl;
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