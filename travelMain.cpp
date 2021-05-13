#include <csignal>
#include <iostream>
#include <cstdlib>
#include <stdlib.h>
#include <stdio.h>
#include <cstring>
#include <string>

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

#include <dirent.h>

#include <signal.h>

#include "fromProjectOne/Structures/virusesList.hpp"
#include "funcs.hpp"
#include "parentCommands.hpp"
#include "sList.hpp"
#include "fromProjectOne/Structures/bloomFilter.hpp"
#include "travelStatsList.hpp"

using namespace std;
int action=0;
#define PERMS 0666

void handlerCatch(int signo);

int main(int argc, const char** argv) {
    static struct sigaction act;
    act.sa_handler = handlerCatch;
    sigfillset(&(act.sa_mask));
    sigaction(SIGINT, &act, NULL);
    sigaction(SIGQUIT, &act, NULL);
    sigaction(SIGCHLD, &act, NULL);
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
            bloomSize = atoi(argv[i+1]);
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
    string** toGiveDirs = new string*[activeMonitors];
    for(int i=0;i<activeMonitors;i++){
        toGiveDirs[i] = new string[int(countryList.count/activeMonitors)+1];
    }
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

    int readfds[activeMonitors];
    int writefds[activeMonitors];
    pid_t monitorPids[activeMonitors];

    for(int i=0;i<activeMonitors;i++){
        string read_temp = "/tmp/fifoR." + to_string(i);
        string write_temp = "/tmp/fifoW."+ to_string(i);
        if((mkfifo(read_temp.c_str(), PERMS)) <0){
            cout << getpid() << ": Error cant create read" << endl;
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
        
        if(write(writefds[i], &bufferSize, sizeof(int)) != sizeof(int)){
            //error
        }
    }
    for(int i=0;i<activeMonitors;i++){
        writePipeInt(writefds[i], bufferSize, bloomSize);
        writePipeInt(writefds[i], bufferSize, int(countryList.count/activeMonitors)+1);
        string tempPath = pathToDirs;
        writePipeInt(writefds[i], bufferSize, tempPath.length());
        writePipe(writefds[i], bufferSize, tempPath);
        for(int j=0;j<int(countryList.count/activeMonitors)+1;j++){
            writePipeInt(writefds[i], bufferSize, toGiveDirs[i][j].length());
            writePipe(writefds[i], bufferSize, toGiveDirs[i][j]);
        }
    }
    VirlistHeader viruses(bloomSize);
    for(int i=0;i<activeMonitors;i++){
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
            tempBlooms[j].erase(0,k+1);
            curr->insertBloom(tempBlooms[j]);
        }
    }

    cout << getpid() << ": Parent ready for commands" << endl;

    TSHeader stats;

    cout <<
    "/travelRequest citizenID date countryFrom countryTo virusName" << endl <<
    "/travelStats virusName date1 date2 [country]" << endl <<
    "/addVaccinationRecords country" << endl <<
    "/searchVaccinationStatus citizenID" << endl <<
    "/exit" << endl << endl;

    while(true){
        switch(action){
            case 1:{
                action =0;
                delete[] pathToDirs;
                closedir(inputDir);
                int status;
                pid_t pid;
                for(int i=0;i<numMonitors;i++){
                    kill(monitorPids[i], SIGKILL);
                    pid = waitpid(monitorPids[i], &status, 0);
                    cout << "child " << (long)pid << " got exited " << status << endl;
                    close(readfds[i]);
                    close(writefds[i]);
                    if(unlink(("/tmp/fifoW."+to_string(i)).c_str()) < 0){
                        cout << "cant unlink" << endl;
                    }
                    if(unlink(("/tmp/fifoR."+to_string(i)).c_str()) <0){
                        cout << "cant unlink" << endl;
                    }
                }
                generateLogFileParent(activeMonitors, int(countryList.count/activeMonitors)+1, toGiveDirs, stats.total, stats.accepted, stats.rejected);
                delete[] toGiveDirs;
                return 0;
            }
            case 2:{
                action =0;
                int status;
                pid_t pid;
                pid = wait(&status);
                for(int i=0;i<activeMonitors;i++){
                    if(monitorPids[i] == pid){
                        close(readfds[i]);
                        close(writefds[i]);
                        if(unlink(("/tmp/fifoW."+to_string(i)).c_str()) < 0){
                            cout << "cant unlink" << endl;
                        }
                        if(unlink(("/tmp/fifoR."+to_string(i)).c_str()) <0){
                            cout << "cant unlink" << endl;
                        }
                        string read_temp = "/tmp/fifoR." + to_string(i);
                        string write_temp = "/tmp/fifoW." + to_string(i);
                        if((mkfifo(read_temp.c_str(), PERMS)) <0){
                            cout << getpid() << ": Error cant create read" << endl;
                        }
                        if((mkfifo(write_temp.c_str(), PERMS)) <0){
                            cout << getpid() << ": Error cant create write" << endl;
                        }
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
                            if((readfds[i] = open(read_temp.c_str(), O_RDONLY)) < 0){
                                cout << "Error cant open read" << endl;
                            }
                            if((writefds[i] = open(write_temp.c_str(), O_WRONLY)) < 0){
                                cout << "Error cant open write" << endl;
                            }
                        }

                        if(write(writefds[i], &bufferSize, sizeof(int)) != sizeof(int)){
                            //error
                        }
                        writePipeInt(writefds[i], bufferSize, bloomSize);
                        writePipeInt(writefds[i], bufferSize, int(countryList.count/activeMonitors)+1);
                        string tempPath = pathToDirs;
                        writePipeInt(writefds[i], bufferSize, tempPath.length());
                        writePipe(writefds[i], bufferSize, tempPath);
                        for(int j=0;j<int(countryList.count/activeMonitors)+1;j++){
                            writePipeInt(writefds[i], bufferSize, toGiveDirs[i][j].length());
                            writePipe(writefds[i], bufferSize, toGiveDirs[i][j]);
                        }

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
                            tempBlooms[j].erase(0,k+1);
                            curr->insertBloom(tempBlooms[j]);
                        }
                        break;
                    }
                }
            }
            break;
            default:
                while(true){//simple switch-case but for strings
                    string command;
                    cout << "Waiting for command: ";
                    cin >> command;
                    if(cin.fail()){ // if we wait for input and get signal this cin fail and user havent input anything yet so instantly go to specific action
                        cin.clear();
                        break;
                    }
                    if(command == "/exit"){
                        delete[] pathToDirs;
                        closedir(inputDir);
                        int status;
                        pid_t pid;
                        for(int i=0;i<numMonitors;i++){
                            kill(monitorPids[i], SIGKILL);
                            pid = waitpid(monitorPids[i], &status, 0);
                            cout << "child " << (long)pid << " got exited " << status << endl;           
                            close(readfds[i]);
                            close(writefds[i]);
                            if(unlink(("/tmp/fifoW."+to_string(i)).c_str()) < 0){
                                cout << "cant unlink" << endl;
                            }
                            if(unlink(("/tmp/fifoR."+to_string(i)).c_str()) <0){
                                cout << "cant unlink" << endl;
                            }
                        }
                        generateLogFileParent(activeMonitors, int(countryList.count/activeMonitors)+1, toGiveDirs, stats.total, stats.accepted, stats.rejected);
                        delete[] toGiveDirs;
                        return 0;
                    }
                    cin.get();
                    string line;
                    getline(cin, line);//here we need to finish the command and then do the action
                    string temp[8];//convert the readed string to a string array for more simplicity
                    int i=0;
                    string word = "";
                    for(auto x : line){
                        if( x== ' '){
                            temp[i] = word;
                            i++;
                            word ="";
                        }else{
                            word = word + x;
                        }
                    }
                    temp[i] = word;
                    i++;
                    if(command == "/travelRequest"){
                        travelRequest(&stats, &viruses, readfds, writefds, bufferSize, activeMonitors, int(countryList.count/activeMonitors)+1, toGiveDirs, monitorPids, stoi(temp[0]), temp[1], temp[2], temp[3], temp[4]);
                        cout << "Done!" << endl;
                    }else if(command == "/travelStats"){
                        if(i==3){
                            stats.getStats(temp[0], temp[1], temp[2], temp[3]);
                        }else{
                            stats.getStats(temp[0], temp[1], temp[2]);
                        }
                        cout << "Done!" << endl;
                    }else if(command == "/addVaccinationRecords"){
                        addVaccinationRecords(readfds, writefds, bufferSize, activeMonitors, int(countryList.count/activeMonitors)+1, toGiveDirs, monitorPids, temp[0], &viruses);
                        cout << "Done!" << endl;
                    }else if(command == "/searchVaccinationStatus"){
                        searchVaccinationStatus(readfds, writefds, bufferSize, activeMonitors, monitorPids, stoi(temp[0]));
                        cout << "Done!" << endl;
                    }
                    cout << endl;

                    if(action != 0){//after command is done do action
                        break;
                    }
                }
        }
    }
}

void handlerCatch(int signo){
    if(signo == SIGINT || signo == SIGQUIT){
        action = 1;
    }else if(signo == SIGCHLD){
        action = 2;
    }
}