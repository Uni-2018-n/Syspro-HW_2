#ifndef _CLASSES_HPP_
#define _CLASSES_HPP_
#include <iostream>

using namespace std;

class citizenRecord{
public:
  int* citizenId;
  string firstName;
  string lastName;
  string* country;
  int age;
  citizenRecord(string, string, string, string*, string);
  ~citizenRecord();
  string getCountry(){
    return *country;
  }
  int getAge(){
    return age;
  }
  void print();
};

class SRListNode{//list with all vaccinations for a citizen
  public:
    string vir;
    int vacced;
    string date;
    SRListNode* next;
    SRListNode(string v, int vacc, string d, SRListNode* n);
};

class SRListHeader{
    SRListNode* start;
  public:
    int len;
    citizenRecord* citizen;
    SRListHeader();
    ~SRListHeader();
    void insert(string vir, int vacced, string date);
    SRListNode* pop();
};

string flipDate(string t);
string checkAndFormatDate(string t);
#endif
