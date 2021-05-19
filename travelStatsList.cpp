#include "travelStatsList.hpp"
#include "fromProjectOne/classes.hpp"
#include <cstring>

//TSNode
TSNode::TSNode(int o, string c, string d){
    outcome = o;
    country = c;
    date = d;
    next = NULL;
}

//TSHeader
TSHeader::TSHeader(){
    start = NULL;
    end = NULL;
    total =0;
    rejected =0;
    accepted =0;
}

void TSHeader::insert(int status, string country, string date){//simple insert at end of the list function
    date = checkAndFormatDate(date);//check and format date function called so we have yyyy-mm-dd format
    if(start == NULL){
        start =  new TSNode(status, country, date);
        end = start;
    }else{
        end->next = new TSNode(status, country, date);
        end = end->next;
    }

    if(status){//update the counters as needed
        accepted++;
    }else{
        rejected++;
    }
    total++;
}

void TSHeader::getStats(string virName, string date1, string date2){//simple get stats function between the 2 provided dates
    date1 = checkAndFormatDate(date1);//again check and format dates to be easier to compare
    date2 = checkAndFormatDate(date2);

    int total =0;//new counters since we only want to count in the specific range of dates
    int rejected =0;
    int accepted =0;

    TSNode* temp = start;
    while(temp != NULL){//for every node
        if(temp->date >= date1 && temp->date <= date2){
            if(temp->outcome){
                accepted++;
            }else{
                rejected++;
            }
            total++;
        }
        temp = temp->next;
    }
    cout << "TOTAL REQUESTS " << total << endl
         << "ACCEPTED " << accepted << endl
         << "REJECTED " << rejected << endl;
}

void TSHeader::getStats(string virName, string date1, string date2, string country){//same function but with country taken into consideration
    date1 = checkAndFormatDate(date1);
    date2 = checkAndFormatDate(date2);

    int total =0;
    int rejected =0;
    int accepted =0;

    TSNode* temp = start;
    while(temp != NULL){
        if(temp->country == country){//first check for country
            if(temp->date >= date1 && temp->date <= date2){//then if its in date range
                if(temp->outcome){
                    accepted++;
                }else{
                    rejected++;
                }
                total++;
            }
        }
        temp = temp->next;
    }
    cout << "TOTAL REQUESTS " << total << endl
         << "ACCEPTED " << accepted << endl
         << "REJECTED " << rejected << endl;
}