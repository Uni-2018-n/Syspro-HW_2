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

using namespace std;
int action=0;

void handlerCatch(int signo);

int main(int argc, const char** argv) {
    action = -1;
    static struct sigaction act;

    act.sa_handler = handlerCatch;
    sigfillset(&(act.sa_mask));
    sigaction(SIGINT, &act, NULL);
    sigaction(SIGQUIT, &act, NULL);
    sigaction(SIGUSR1, &act, NULL);
    int readfd, writefd;
    if((writefd = open(argv[0], O_WRONLY)) < 0){
        cout << "error" << endl;
    }
    if((readfd = open(argv[1], O_RDONLY)) < 0){
        cout << "Error" << endl;
    }

    int bufferSize;
    if(read(readfd, &bufferSize, sizeof(int)) < 0){
        cout << "Error" << endl;
    }

    int bloomSize = readPipeInt(readfd, bufferSize);
    
    int numOfCountries = readPipeInt(readfd, bufferSize);
    cout << getpid() << ": bloom: " << bloomSize << ", numofcountries: " << numOfCountries << endl;

    string dirs[numOfCountries];
    for(int i=0;i<numOfCountries;i++){
        int tempSize = readPipeInt(readfd, bufferSize);
        dirs[i] = readPipe(readfd, tempSize, bufferSize);
    }

    GlistHeader* main_list = new GlistHeader(bloomSize);
    int countFilesOfDirs[numOfCountries];
    for(int i=0;i<numOfCountries;i++){
        if(strcmp(dirs[i].c_str(), "") == 0){
            continue;
        }
        DIR *curr_dir;
        if((curr_dir = opendir(("input_dir/"+dirs[i]+'/').c_str()))== NULL){
            cout << "error" << endl;
        }
        int count=0;
        struct dirent *dirent;
        while((dirent=readdir(curr_dir)) != NULL){
            if(strcmp(dirent->d_name, ".") == 0 || strcmp(dirent->d_name, "..")==0){
                continue;
            }
            ifstream records("input_dir/"+dirs[i]+"/"+dirent->d_name);
            if(records.fail()){
                cout << "error record open" << endl;
            }
            string line;

            while(getline(records, line)){
                main_list->insertRecord(line, false);
            }
            records.close();
            count++;
        }
        countFilesOfDirs[i]=count;
        closedir(curr_dir);
    }
    string* temp_blooms = main_list->getBlooms();
    writePipeInt(writefd, bufferSize, main_list->getCountViruses());
    for(int i=0;i<main_list->getCountViruses();i++){
        writePipeInt(writefd, bufferSize, temp_blooms[i].length());
        writePipe(writefd, bufferSize, temp_blooms[i]);
        int t = readPipeInt(readfd, bufferSize);
        if(t != 0){
            cout << "ERROR" << endl;
        }
    }

    cout << getpid() << ": " << "Ready for commands" << endl; //TODO: send ready to parent

    int totalRequests=0;
    int acceptedRequests=0;
    int rejectedRequests=0;
    while(true){
        switch(action){
            case 1:
                generateLogFile(numOfCountries, dirs, totalRequests, acceptedRequests, rejectedRequests);
                action = 0;
                break;
            case 2:
                appendData(numOfCountries, dirs, countFilesOfDirs, main_list);

                temp_blooms = main_list->getBlooms();
                writePipeInt(writefd, bufferSize, main_list->getCountViruses());
                for(int i=0;i<main_list->getCountViruses();i++){
                    writePipeInt(writefd, bufferSize, temp_blooms[i].length());
                    writePipe(writefd, bufferSize, temp_blooms[i]);
                    int t = readPipeInt(readfd, bufferSize);
                    if(t != 0){
                        cout << "ERROR" << endl;
                    }
                }
                action = 0;
                break;
            case 3:
                cout << getpid() << ": exiting..." << endl;
                delete main_list;
                close(readfd);
                close(writefd);
                if(unlink(argv[0]) <0){
                    cout << "cant unlink" << endl;
                }
                if(unlink(argv[1]) <0){
                    cout << "cant unling" << endl;
                }
                return 0;
            default:
                int currFunc= readPipeInt(readfd, bufferSize);
                break;
        }
    }
}

void handlerCatch(int signo){
    if(signo == SIGINT || signo == SIGQUIT){
        action = 1;
    }else if(signo == SIGUSR1){
        action = 2;
    }
}