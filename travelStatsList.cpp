#include "travelStatsList.hpp"
#include "fromProjectOne/classes.hpp"
#include <cstring>

TSNode::TSNode(int o, string c, string d){
    outcome = o;
    country = c;
    date = d;
    next = NULL;
}

TSHeader::TSHeader(){
    start = NULL;
    end = NULL;
    total =0;
    rejected =0;
    accepted =0;
}

void TSHeader::insert(int status, string country, string date){
    date = checkAndFormatDate(date);
    if(start == NULL){
        start=  new TSNode(status, country, date);
        end = start;
    }else{
        end->next = new TSNode(status, country, date);
        end = end->next;
    }

    if(status){
        accepted++;
    }else{
        rejected++;
    }
    total++;
}

void TSHeader::getStats(string virName, string date1, string date2){
    date1 = checkAndFormatDate(date1);
    date2 = checkAndFormatDate(date2);

    int total =0;
    int rejected =0;
    int accepted =0;

    TSNode* temp = start;
    while(temp != NULL){
        if(temp->date >= date1 && temp->date <= date2){
            if(temp->outcome){
                accepted++;
            }else{
                rejected++;
            }
            total++;
        }
    }
    cout << "TOTAL REQUESTS " << total << endl
         << "ACCEPTED " << accepted << endl
         << "REJECTED " << rejected << endl;
}

void TSHeader::getStats(string virName, string date1, string date2, string country){
    date1 = checkAndFormatDate(date1);
    date2 = checkAndFormatDate(date2);

    int total =0;
    int rejected =0;
    int accepted =0;

    TSNode* temp = start;
    while(temp != NULL){
        if(temp->country == country){
            if(temp->date >= date1 && temp->date <= date2){
                if(temp->outcome){
                    accepted++;
                }else{
                    rejected++;
                }
                total++;
            }
        }
    }
    cout << "TOTAL REQUESTS " << total << endl
         << "ACCEPTED " << accepted << endl
         << "REJECTED " << rejected << endl;
}