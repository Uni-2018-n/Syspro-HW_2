#include "protocol.hpp"
#include "fromProjectOne/classes.hpp"
#include "fromProjectOne/generalList.hpp"
#include "funcs.hpp"
#include <string>

int handlFunctionMonitor(int readfd, int writefd, int bufferSize, int currFunc, GlistHeader* main_list){
    switch(currFunc){
        case 101:{ //brackets because cpp dosent like creating variables inside cases
            //reads a last int from parent indicating if the request was accepted or requested 
            int id = readPipeInt(readfd, bufferSize);
            int tempSize = readPipeInt(readfd, bufferSize);
            string vir = readPipe(readfd, tempSize, bufferSize);
            string temp = main_list->vaccineStatus(id, vir, false);
            if(temp.length() != 0){
                writePipeInt(writefd, bufferSize, 1);
                writePipeInt(writefd, bufferSize, temp.length());
                writePipe(writefd, bufferSize, temp);
            }else{
                writePipeInt(writefd, bufferSize, 0);
            }
            return readPipeInt(readfd, bufferSize);
        }
        case 104:{
            int id = readPipeInt(readfd, bufferSize);
            SRListHeader* returnedViruses = main_list->vaccineStatus(id, true);
            if(returnedViruses->len > 0){
                writePipeInt(writefd, bufferSize, 1);
                string temp = to_string(*(returnedViruses->citizen->citizenId)) + " " + returnedViruses->citizen->firstName + " " + returnedViruses->citizen->lastName + " " + returnedViruses->citizen->getCountry();
                writePipeInt(writefd, bufferSize, temp.length());
                writePipe(writefd, bufferSize, temp);
                writePipeInt(writefd, bufferSize, returnedViruses->citizen->getAge());
                writePipeInt(writefd, bufferSize, returnedViruses->len);
                for(int i=0;i<returnedViruses->len;i++){
                    SRListNode* poped = returnedViruses->pop();
                    writePipeInt(writefd, bufferSize, poped->vir.length());
                    writePipe(writefd, bufferSize, poped->vir);
                    writePipeInt(writefd, bufferSize, poped->vacced);
                    if(poped->vacced == 1){
                        writePipeInt(writefd, bufferSize, poped->date.length());
                        writePipe(writefd, bufferSize, poped->date);
                    }
                    delete poped;
                }
            }else{
                writePipeInt(writefd, bufferSize, 0);
            }
            delete returnedViruses;
            return -1;
        }
    }
    return -1;
}
