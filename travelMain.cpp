#include <csignal>
#include <cstdio>
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
#define PERMS 0666
int action=0;

void handlerCatch(int signo);

int main(int argc, const char** argv){
    static struct sigaction act;//change the signal default signal functions to the signal handler
    act.sa_handler = handlerCatch;
    sigfillset(&(act.sa_mask));
    sigaction(SIGINT, &act, NULL);
    sigaction(SIGQUIT, &act, NULL);
    sigaction(SIGCHLD, &act, NULL);

    int numMonitors, bloomSize, bufferSize;
    char* pathToDirs;
    if(argc != 9){//simple steps to set the command line arguments to parameters
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
    if((inputDir = opendir(pathToDirs))== NULL){//open the input_dir provided in the command line to read the countries
        perror("Parent: Cant open dir\n");
        return -1;
    }

    struct dirent *dirent;
    SLHeader countryList;//create a list with all the countries
    while((dirent=readdir(inputDir)) != NULL){//for each item available in the input dir
        if(strcmp(dirent->d_name, ".") == 0 || strcmp(dirent->d_name, "..")==0){//skip the two subdirs indicating the previus dir
            continue;
        }
        countryList.insert(dirent->d_name);//insert the countrie's name to the list
    }

    int activeMonitors;
    if(countryList.count < numMonitors){//in case the count of the countries is less than the monitors
        activeMonitors = countryList.count;//we only need as many monitors as countries
    }else{
        activeMonitors = numMonitors;//else we use as many monitors as user provided from command line
    }
    string** toGiveDirs = new string*[activeMonitors];//create an 2D array, [i][j], i indicates the index of the monitor and j the index of the country that the monitor will work on
    for(int i=0;i<activeMonitors;i++){
        toGiveDirs[i] = new string[int(countryList.count/activeMonitors)+1];
    }
    int i=0;
    int j=0;
    string curr = countryList.popFirst();
    while(strcmp(curr.c_str(), "") != 0){//for each country in the list (if popFirst returns "" it means that the list is empty)
        toGiveDirs[i][j] = curr;//store the country to the array
        j++;
        if(j == int(countryList.count/activeMonitors)+1){
            j=0;
            i++;
        }
        curr = countryList.popFirst();
    }

    int readfds[activeMonitors];//stores the every filedescriptor for reading 
    int writefds[activeMonitors];//stores the every filedescriptor for writing
    pid_t monitorPids[activeMonitors];//stores the pid number of each child

    for(int i=0;i<activeMonitors;i++){
        string read_temp = "/tmp/fifoR." + to_string(i); //our names FiFos are stored in the /tmp/ folder with fifoR/W.child_index name
        string write_temp = "/tmp/fifoW."+ to_string(i);
        if(((mkfifo(read_temp.c_str(), PERMS)) <0) && (errno != EEXIST)){//make the fifos
            perror("Parent: Cant create read FiFo\n");
        }
        if(((mkfifo(write_temp.c_str(), PERMS)) <0) &&(errno != EEXIST)){
            perror("Parent: Cant create write FiFo\n");
        }

        pid_t pid;
        switch(pid=fork()){//fork the new child
        case -1:
            perror("Parent: Fork ERROR\n");
            exit(-1);
            break;
        case 0:
            execlp("./monitor", read_temp.c_str(), write_temp.c_str(), NULL);//and execlp the new child to the ./monitor process
            perror("Parent: Execlp ERROR\n");
            break;
        default:
            monitorPids[i]=pid;//in the parent process store the pid for use later
        }

        if((readfds[i] = open(read_temp.c_str(), O_RDONLY)) < 0){//since the child is now created, open the fifos and store the file descriptors
            perror("Parent: Cant open read\n");
        }
        if((writefds[i] = open(write_temp.c_str(), O_WRONLY)) < 0){
            perror("Parent: Cant open write\n");
        }
        
        if(write(writefds[i], &bufferSize, sizeof(int)) != sizeof(int)){//send the first data to the monitor(data is the bufferSize, read readme for more)
            perror("Parent: BufferSize data write ERROR\n");
        }
    }
    for(int i=0;i<activeMonitors;i++){//for each monitor, 
        writePipeInt(writefds[i], bufferSize, bloomSize);//send the bloomSize value 
        writePipeInt(writefds[i], bufferSize, int(countryList.count/activeMonitors)+1);//send the number of countries that the monitor will work on
        string tempPath = pathToDirs;
        writePipeInt(writefds[i], bufferSize, tempPath.length());
        writePipe(writefds[i], bufferSize, tempPath);//and send the relative path for the root folder of the countries
        for(int j=0;j<int(countryList.count/activeMonitors)+1;j++){//and start sending each country to the monitor
            writePipeInt(writefds[i], bufferSize, toGiveDirs[i][j].length());
            writePipe(writefds[i], bufferSize, toGiveDirs[i][j]);
        }
    }
    VirlistHeader viruses(bloomSize);//after all of this is done wait for each monitor to receive the bloom filters
    for(int i=0;i<activeMonitors;i++){
        int tempSize= readPipeInt(readfds[i], bufferSize);
        string tempBlooms[tempSize];//1D array to store the encoded bloom filters for each virus from the monitor
        for(int j=0;j<tempSize;j++){
            int ts = readPipeInt(readfds[i], bufferSize);//read the blooms with error checking in case something happen while reading
            while(ts == -1){
                writePipeInt(writefds[i], bufferSize, -1);
                ts = readPipeInt(readfds[i], bufferSize);
            }
            writePipeInt(writefds[i], bufferSize, 0);
            tempBlooms[j] = readPipe(readfds[i], ts, bufferSize);//and store it into a temporary string array
        }
        if(readPipeInt(readfds[i], bufferSize) != 0){//read the confirmation message from the monitor 
            exit(-1);
        }
        for(int j=0;j<tempSize;j++){//for each virus
            int k=tempBlooms[j].find("!");//find the ! indicator to substring the name of the virus
            VirlistNode* curr;
            if((curr = viruses.searchVirus(tempBlooms[j].substr(0, k))) == NULL){//check if the virus exists, 
                curr = viruses.insertVirus(tempBlooms[j].substr(0,k));//if not create it
            }
            tempBlooms[j].erase(0,k+1);//remove the name(and the !) of the encoded string
            curr->insertBloom(tempBlooms[j]);//and finally append the data to the bloom filter
        }
    }

    TSHeader stats;

    cout <<
    "/travelRequest citizenID date countryFrom countryTo virusName" << endl <<
    "/travelStats virusName date1 date2 [country]" << endl <<
    "/addVaccinationRecords country" << endl <<
    "/searchVaccinationStatus citizenID" << endl <<
    "/exit" << endl << endl;

    while(true){
        switch(action){//swtich-case method to see the type of action we need to do
            case 1:{//case 1 indicated SIGINT/SIGQUIT signal handlers
                action = 0;
                delete[] pathToDirs;
                closedir(inputDir);
                int status;
                pid_t pid;
                for(int i=0;i<numMonitors;i++){//for each child 
                    kill(monitorPids[i], SIGKILL);//send a SIGKILL signal to stop them
                    pid = waitpid(monitorPids[i], &status, 0);//wait for the exit status
                    cout << "child " << (long)pid << " got exited " << status << endl;//print it
                    close(readfds[i]);//close the file descriptors 
                    close(writefds[i]);
                    if(unlink(("/tmp/fifoW."+to_string(i)).c_str()) < 0){//unlink so the FiFos will get deleted
                        perror("Parent: action 1 cant unlink Write\n");
                    }
                    if(unlink(("/tmp/fifoR."+to_string(i)).c_str()) <0){
                        perror("Parent: action 1 cant unlink Read\n");
                    }
                }
                generateLogFileParent(activeMonitors, int(countryList.count/activeMonitors)+1, toGiveDirs, stats.total, stats.accepted, stats.rejected);//and generate the log_file
                for(int i=0;i<activeMonitors;i++){
                    delete[] toGiveDirs[i];
                }
                delete[] toGiveDirs;
                return 0;
            }
            case 2:{//case 2 indicated the SIGCHLD signal handler
                action = 0;
                int status;
                pid_t pid;
                pid = wait(&status);//since the process just stoped, wait for the exit status so it wont be a zombie process
                for(int i=0;i<activeMonitors;i++){//and since the wait() function returns the pid of the child that just stoped
                    if(monitorPids[i] == pid){//find the child inside the pid array
                        close(readfds[i]);//close the already existing file descriptors 
                        close(writefds[i]);
                        string read_temp = "/tmp/fifoR." + to_string(i);
                        string write_temp = "/tmp/fifoW." + to_string(i);
                        if(unlink((write_temp.c_str())) < 0){//unlink from the fifos
                            perror("Parent: action 2 cant unlink Write\n");
                        }
                        if(unlink((read_temp.c_str())) <0){
                            perror("Parent: action 2 cant unlink Read\n");
                        }


                        if((mkfifo(read_temp.c_str(), PERMS)) <0){//create the new clean fifos
                            perror("Parent: action 2 cant create Read FiFo\n");
                        }
                        if((mkfifo(write_temp.c_str(), PERMS)) <0){
                            perror("Parent: action 2 cant create Write FiFo\n");
                        }
                        switch(pid=fork()){//fork the new child process
                        case -1:
                            perror("Parent: Fork ERROR\n");
                            exit(-1);
                            break;
                        case 0:
                            execlp("./monitor", read_temp.c_str(), write_temp.c_str(), NULL);
                            perror("Parent: Execlp ERROR\n");
                            break;
                        default:
                            monitorPids[i]=pid;//and finally store the updated pid and fds to the arrays
                            if((readfds[i] = open(read_temp.c_str(), O_RDONLY)) < 0){
                                perror("Parent: Cant open read\n");
                            }
                            if((writefds[i] = open(write_temp.c_str(), O_WRONLY)) < 0){
                                perror("Parent: Cant open write\n");
                            }
                        }

                        if(write(writefds[i], &bufferSize, sizeof(int)) != sizeof(int)){//since we have a completly new child process we need to repeat the initialization routine so the monitor will work with the same data the old one did
                            perror("Parent: BufferSize data write ERROR\n");
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
                            while(ts == -1){
                                writePipeInt(writefds[i], bufferSize, -1);
                                ts = readPipeInt(readfds[i], bufferSize);
                            }
                            writePipeInt(writefds[i], bufferSize, 0);
                            tempBlooms[j] = readPipe(readfds[i], ts, bufferSize);
                        }
                        
                        if(readPipeInt(readfds[i], bufferSize) != 0){
                            exit(-1);
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
            default://default case is the user command case
                while(true){//simple switch-case but for strings
                    string command;
                    cout << "Waiting for command: " << endl;
                    cin >> command;
                    if(cin.fail()){ // if we wait for input and get signal this cin fail and user havent input anything yet so instantly go to specific action
                        cin.clear();
                        break;
                    }
                    if(command == "/exit"){//exit command, just like case 1 
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
                                perror("Parent: /exit cant unlink Write\n");
                            }
                            if(unlink(("/tmp/fifoR."+to_string(i)).c_str()) <0){
                                perror("Parent: /exit cant unlink Read\n");
                            }
                        }
                        generateLogFileParent(activeMonitors, int(countryList.count/activeMonitors)+1, toGiveDirs, stats.total, stats.accepted, stats.rejected);
                        for(int i=0;i<activeMonitors;i++){
                            delete[] toGiveDirs[i];
                        }
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
                        if(i==4){//in case we have a country or not
                            stats.getStats(temp[0], temp[1], temp[2], temp[3]);
                            cout << "Done!" << endl;
                        }else if(i==3){
                            stats.getStats(temp[0], temp[1], temp[2]);
                            cout << "Done!" << endl;
                        }else{
                            cout << "Error /travelStatus wrong input" << endl;
                        }
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

void handlerCatch(int signo){//simple signal handler
    if(signo == SIGINT || signo == SIGQUIT){
        action = 1;
    }else if(signo == SIGCHLD){
        cout << "SIGCHLD" << endl;
        action = 2;
    }
}