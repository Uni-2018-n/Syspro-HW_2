#include "protocol.hpp"
#include "fromProjectOne/classes.hpp"
#include "fromProjectOne/generalList.hpp"
#include "funcs.hpp"
#include <string>

void handlFunctionMonitor(int readfd, int writefd, int bufferSize, int currFunc, GlistHeader* main_list){
    int tempSize;
    int id;
    string vir;
    string temp;
    SRListHeader* returnedViruses;
    SRListNode* poped;
    switch(currFunc){
        case 101:
            id = readPipeInt(readfd, bufferSize);
            tempSize = readPipeInt(readfd, bufferSize);
            vir = readPipe(readfd, tempSize, bufferSize);
            temp = main_list->vaccineStatus(id, vir, false);
            if(temp.length() != 0){
                writePipeInt(writefd, bufferSize, 1);
                writePipeInt(writefd, bufferSize, temp.length());
                writePipe(writefd, bufferSize, temp);
            }else{
                writePipeInt(writefd, bufferSize, 0);
            }
            return;
        case 104:
            id = readPipeInt(readfd, bufferSize);
            returnedViruses = main_list->vaccineStatus(id, true);
            if(returnedViruses->len > 0){
                writePipeInt(writefd, bufferSize, 1);
                string temp = to_string(*(returnedViruses->citizen->citizenId)) + " " + returnedViruses->citizen->firstName + " " + returnedViruses->citizen->lastName + " " + returnedViruses->citizen->getCountry();
                writePipeInt(writefd, bufferSize, temp.length());
                writePipe(writefd, bufferSize, temp);
                writePipeInt(writefd, bufferSize, returnedViruses->citizen->getAge());
                writePipeInt(writefd, bufferSize, returnedViruses->len);
                for(int i=0;i<returnedViruses->len;i++){
                    poped = returnedViruses->pop();
                    writePipeInt(writefd, bufferSize, poped->vir.length());
                    writePipe(writefd, bufferSize, poped->vir);
                    writePipeInt(writefd, bufferSize, poped->vacced);
                    if(poped->vacced == 1){
                        writePipeInt(writefd, bufferSize, poped->date.length());
                        writePipe(writefd, bufferSize, poped->date);
                    }
                }
            }else{
                writePipeInt(writefd, bufferSize, 0);
            }
            delete returnedViruses;
            return;
        default:
        break;
    }
}
