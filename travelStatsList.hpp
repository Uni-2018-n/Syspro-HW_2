#ifndef _TRAVELSTATSLIST_HPP_
#define _TRAVELSTATSLIST_HPP_

#include <string>
using namespace std;
class TSNode{
public:
    int outcome;
    string country;
    string date;
    TSNode* next;
    TSNode(int o, string c, string d);
};

class TSHeader{
    TSNode* start;
    TSNode* end;
public:
    int total;
    int rejected;
    int accepted;
    TSHeader();
    void insert(int status, string country, string date);
    void getStats(string virName, string date1, string date2);
    void getStats(string virName, string date1, string date2, string country);
};

#endif