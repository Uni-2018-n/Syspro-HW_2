#include <cstdlib>
#include <fcntl.h>
#include <iostream>
#include <cstring>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>

#include "sList.hpp"

using namespace std;

#define PERMS 0666

int main(int argc, const char** argv) {
    int numMonitors, bloomSize, bufferSize;
    char* pathToDirs;
    if(argc != 9){
        cout << "Error! Wrong parameters:" << endl;
        cout << "./travelMonitor -m numMonitors -b bufferSize -s sizeOfBloom -i input_dir" << endl;
        return -1;
    }
    for(int i=1;i<argc;i++){
        if(strcmp(argv[i], "-m")==0){
            numMonitors = atoi(argv[i+1]);
        }else if(strcmp(argv[i], "-b")==0){
            bufferSize = atoi(argv[i+1]);
        }else if(strcmp(argv[i], "-s")==0){
            bloomSize = atoi(argv[i+1]) *8;
        }else if(strcmp(argv[i], "-i")==0){
            pathToDirs = new char[strlen(argv[i+1])+1]();
            strcpy(pathToDirs, argv[i+1]);
            break;
        }
    }
    DIR *inputDir;
    if((inputDir = opendir(pathToDirs))== NULL){
        cout << "Error! Directory can not open!" << endl;
        return -1;
    }

    struct dirent *dirent;
    SLHeader countryList;
    while((dirent=readdir(inputDir)) != NULL){
        if(strcmp(dirent->d_name, ".") == 0 || strcmp(dirent->d_name, "..")==0){
            continue;
        }
        countryList.insert(dirent->d_name);
    }

    int activeMonitors;
    if(countryList.count < numMonitors){
        activeMonitors = countryList.count;
    }else{
        activeMonitors = numMonitors;
    }
    string curr = countryList.popFirst();
    string toGiveDirs[activeMonitors][int(countryList.count/activeMonitors)+1];
    int i=0;
    int j=0;
    while(strcmp(curr.c_str(), "") != 0){
        toGiveDirs[i][j] = curr;
        i++;
        if(i ==activeMonitors){
            i=0;
            j++;
        }
        curr = countryList.popFirst();
    }

    int readfds[numMonitors];
    int writefds[numMonitors];
    pid_t monitorPids[numMonitors];

    for(int i=0;i<numMonitors;i++){
        string read_temp = "/tmp/fifoR." + to_string(i);
        string write_temp = "/tmp/fifoW."+ to_string(i);
        if((mkfifo(read_temp.c_str(), PERMS)) <0){
            cout << "Error cant create read" << endl;
        }
        if((mkfifo(write_temp.c_str(), PERMS)) <0){
            cout << "Error cant create write" << endl;
        }

        pid_t pid;
        switch(pid=fork()){
        case -1:
            cout << "Error fork" << endl;
            break;
        case 0:
            execlp("./monitor", read_temp.c_str(), write_temp.c_str(), NULL);
            cout << "Error execlp" << endl;
            break;
        default:
            monitorPids[i]=pid;
        }

        if((readfds[i] = open(read_temp.c_str(), O_RDONLY)) < 0){
            cout << "Error cant open read" << endl;
        }
        if((writefds[i] = open(write_temp.c_str(), O_WRONLY)) < 0){
            cout << "Error cant open write" << endl;
        }
    }

    closedir(inputDir);
    for(int i=0;i<numMonitors;i++){
        close(readfds[i]);
        close(writefds[i]);
    }
    
    return 0;
}