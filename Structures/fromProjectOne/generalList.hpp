#ifndef _GENERALLIST_HPP_
#define _GENERALLIST_HPP_
#include <iostream>

#include "classes.hpp"
#include "Structures/countryList.hpp"
#include "Structures/virusesList.hpp"

using namespace std;

//simple linked list with a header

class listNode{
  citizenRecord* citizen;
  listNode* next;
public:
  listNode(citizenRecord*);
  ~listNode();

  citizenRecord* getCitizen();
  void setNext(listNode* n);
  listNode* getNext();
};

class GlistHeader{//for simplicity reasons in the header we have the
  listNode* start;//start and end of the citizen list
  listNode* end;
  ClistHeader* countries;//a list with all the countries so we wont have any data doublication
  VirlistHeader* viruses;//and a list with all the viruses with all the implementation purposes
public:
  GlistHeader(int bloom);
  ~GlistHeader();

  listNode* searchCitizen(int);
  void insertRecord(string line, bool flag);
  string* getBlooms();
  int getCountViruses();


  void vaccineStatusBloom(int i, string v);
  void vaccineStatus(int i, string v);
  void vaccineStatus(int i);
  string vaccineStatus(int i, string v, bool prints);
  SRListHeader* vaccineStatus(int i, bool prints);

  void vaccinateNow(int i, string fn, string ln, string c, string a, string v);
  void insertCitizenRecord(string line);
  void listNonVaccinatedPersons(string v);
};

#endif
