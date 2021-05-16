#include <csignal>
#include <iostream>
#include <fstream>
#include <cstring>
#include <string>

#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/errno.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>

#include <dirent.h>

#include <signal.h>

#include "../funcs.hpp"
#include "../fromProjectOne/generalList.hpp"
#include "../fromProjectOne/Structures/bloomFilter.hpp"
#include "commands.hpp"
#include "../protocol.hpp"

using namespace std;
int action=0;

void handlerCatch(int signo);

int main(int argc, const char** argv) {    
    static struct sigaction act;//setup signal handlers for the desired signals
    act.sa_handler = handlerCatch;
    sigfillset(&(act.sa_mask));
    sigaction(SIGINT, &act, NULL);
    sigaction(SIGQUIT, &act, NULL);
    sigaction(SIGUSR1, &act, NULL);
    int readfd, writefd;
    if((writefd = open(argv[0], O_WRONLY)) < 0){//open the fds 
        perror("Child: Cant open write\n");
    }
    if((readfd = open(argv[1], O_RDONLY)) < 0){
        perror("Child: Cant open read\n");
    }

    int bufferSize;
    if(read(readfd, &bufferSize, sizeof(int)) < 0){//read the bufferSize
        perror("Child: BufferSize data read ERROR\n");
    }

    int bloomSize = readPipeInt(readfd, bufferSize);//and read the bloom size 
    int numOfCountries = readPipeInt(readfd, bufferSize);//and the number of countries that this monitor process will work on

    int tempSize = readPipeInt(readfd, bufferSize);
    string pathToDirs = readPipe(readfd, tempSize, bufferSize);//finally read the relative path

    string dirs[numOfCountries];
    for(int i=0;i<numOfCountries;i++){//and read and store the countries
        tempSize = readPipeInt(readfd, bufferSize);
        dirs[i] = readPipe(readfd, tempSize, bufferSize);
    }

    GlistHeader* main_list = new GlistHeader(bloomSize);//use a GlistHeader just like the first project to store the viruses, countries and the citizens
    int countFilesOfDirs[numOfCountries];//array usefull for SIGUSR1 signal(see next)
    for(int i=0;i<numOfCountries;i++){//for each country 
        if(strcmp(dirs[i].c_str(), "") == 0){//skip the empty one(there could be 1 empty incase the count of all the countries is odd)
            continue;
        }
        DIR *curr_dir;
        if((curr_dir = opendir((pathToDirs + '/' + dirs[i] + '/').c_str()))== NULL){//open the directory 
            perror("Child: Cant open dir\n");
        }
        int count=0;//simple counter to be stored later in countFilesOfDirs to count how many files there is in the country
        struct dirent *dirent;
        while((dirent=readdir(curr_dir)) != NULL){//and for each file inside the country's directory
            if(strcmp(dirent->d_name, ".") == 0 || strcmp(dirent->d_name, "..")==0){
                continue;
            }
            ifstream records("input_dir/"+dirs[i]+"/"+dirent->d_name);//open the file
            if(records.fail()){
                perror("Child: file open ERROR\n");
            }
            string line;

            while(getline(records, line)){//and provide each line inside the main list like we did in project one
                main_list->insertRecord(line, false);
            }
            records.close();
            count++;
        }
        countFilesOfDirs[i]=count;
        closedir(curr_dir);
    }
    string* temp_blooms = main_list->getBlooms();//after everything is done encode all the bloom filters in a string array to be easier to send to the parent
    writePipeInt(writefd, bufferSize, main_list->getCountViruses());//and start sending
    for(int i=0;i<main_list->getCountViruses();i++){
        writePipeInt(writefd, bufferSize, temp_blooms[i].length());
        int t = readPipeInt(readfd, bufferSize);
        while(t != 0){
            writePipeInt(writefd, bufferSize, temp_blooms[i].length());
            t = readPipeInt(readfd, bufferSize);
        }
        writePipe(writefd, bufferSize, temp_blooms[i]);
    }

    writePipeInt(writefd, bufferSize, 0);//finally inform the parent that everything is ok and ready to receive commands and signals

    int totalRequests=0;
    int acceptedRequests=0;
    int rejectedRequests=0;
    while(true){
        switch(action){//simple switch-case to handl the actions 
            case 1://case 1 is when monitor receives a SIGINT or SIGQUIT signal
                action = 0;
                generateLogFile(numOfCountries, dirs, totalRequests, acceptedRequests, rejectedRequests);//simply generate the log file and continue
                break;
            case 2://case 2 is for when the monitor receives SIGUSR1 signal
                action = 0;
                appendData(numOfCountries, pathToDirs, dirs, countFilesOfDirs, main_list);//use the appendData function to append the data in the main_list
                temp_blooms = main_list->getBlooms();//re-encode the blooms
                writePipeInt(writefd, bufferSize, main_list->getCountViruses());//and send the updated blooms to the parent process
                for(int i=0;i<main_list->getCountViruses();i++){
                    writePipeInt(writefd, bufferSize, temp_blooms[i].length());
                    int t = readPipeInt(readfd, bufferSize);
                    while(t != 0){
                        writePipeInt(writefd, bufferSize, temp_blooms[i].length());
                        t = readPipeInt(readfd, bufferSize);
                    }
                    writePipe(writefd, bufferSize, temp_blooms[i]);
                }
                break;
            case 3://case 3 is exit case, we dont really use this part of the code(read readme for more)
                cout << getpid() << ": exiting..." << endl;
                delete main_list;
                delete[] temp_blooms;
                close(readfd);
                close(writefd);
                if(unlink(argv[0]) <0){
                    perror("Child: action 3 cant unlink Read\n");
                }
                if(unlink(argv[1]) <0){
                    perror("Child: action 3 cant unlink Write\n");
                }
                return 0;
            default://default case is when the monitor wait to receive an int to see what command the parent process told the monitor to run
                int currFunc= readPipeInt(readfd, bufferSize);
                if(currFunc != -1){
                    int t = handlFunctionMonitor(readfd, writefd, bufferSize, currFunc, main_list);//by passing the command to the handler function 
                    if(t == 1){//this function could return 1 or 0 if the function was a /travelRequest, if yes update the variables
                        acceptedRequests++;
                        totalRequests++;
                    }else if(t == 0){
                        rejectedRequests++;
                        totalRequests++;
                    }
                }
                break;
        }
    }
}

void handlerCatch(int signo){//simple signal handler function
    if(signo == SIGINT || signo == SIGQUIT){
        action = 1;
    }else if(signo == SIGUSR1){
        action = 2;
    }
}