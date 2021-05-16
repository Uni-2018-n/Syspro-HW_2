#include "protocol.hpp"
#include "fromProjectOne/classes.hpp"
#include "fromProjectOne/generalList.hpp"
#include "funcs.hpp"
#include <string>

int handlFunctionMonitor(int readfd, int writefd, int bufferSize, int currFunc, GlistHeader* main_list){
    switch(currFunc){//simple switch-case for each protocol message case
        case 101:{ //brackets because cpp dosent like creating variables inside cases
                   //101 protocol message for /travelRequest command
            int id = readPipeInt(readfd, bufferSize);//read the citizenID
            int tempSize = readPipeInt(readfd, bufferSize);//and the virus name
            string vir = readPipe(readfd, tempSize, bufferSize);
            string temp = main_list->vaccineStatus(id, vir, false);//checks if the citizen is vaccinated(false means that we dont want to print something from this function) and returns "" if citizen not vaccinated or a date if vaccinated
            if(temp.length() != 0){//handle everything as planed if vaccinated or not
                writePipeInt(writefd, bufferSize, 1);
                writePipeInt(writefd, bufferSize, temp.length());
                writePipe(writefd, bufferSize, temp);
            }else{
                writePipeInt(writefd, bufferSize, 0);
            }
            return readPipeInt(readfd, bufferSize);//read the 1 or 0 that indicated if the request was accepted or rejected
        }
        case 104:{//104 case is for  /searchVaccinationStatus command
            int id = readPipeInt(readfd, bufferSize);//read the citizenID
            SRListHeader* returnedViruses = main_list->vaccineStatus(id, false);//vaccineStatus function with boolean returns a SRListHeader list header with all the vaccinations that a citizen has done instead of printing messages
            if(returnedViruses->len > 0){//check if the citizen exists
                writePipeInt(writefd, bufferSize, 1);//send 1 to indicate that the citizen exists here
                string temp = to_string(*(returnedViruses->citizen->citizenId)) + " " + returnedViruses->citizen->firstName + " " + returnedViruses->citizen->lastName + " " + returnedViruses->citizen->getCountry();//format the information string(id, name, country)
                writePipeInt(writefd, bufferSize, temp.length());
                writePipe(writefd, bufferSize, temp);
                writePipeInt(writefd, bufferSize, returnedViruses->citizen->getAge());//send the citizen's age
                writePipeInt(writefd, bufferSize, returnedViruses->len);//and send how many vaccinations the citizen has done
                for(int i=0;i<returnedViruses->len;i++){//for each vaccination
                    SRListNode* poped = returnedViruses->pop();
                    writePipeInt(writefd, bufferSize, poped->vir.length());//send the information to the parent(0 or 1 and date if 1)
                    writePipe(writefd, bufferSize, poped->vir);
                    writePipeInt(writefd, bufferSize, poped->vacced);
                    if(poped->vacced == 1){
                        writePipeInt(writefd, bufferSize, poped->date.length());
                        writePipe(writefd, bufferSize, poped->date);
                    }
                    delete poped;
                }
            }else{
                writePipeInt(writefd, bufferSize, 0);//else simple send 0 for the case that the citizen dosent exist
            }
            delete returnedViruses;
            return -1;
        }
    }
    return -1;
}
