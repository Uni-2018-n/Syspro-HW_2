#ifndef _VIRUSESLIST_HPP_
#define _VIRUSESLIST_HPP_
#include <iostream>
#include <string>

#include "bloomFilter.hpp"
#include "skipList.hpp"
#include "../classes.hpp"
using namespace std;

class VirlistNode{
  string item;//name of the virus
  bloomFilter* bloom;//bloom filter for each virus
  skipHeader* vaccinated;//two skip lists for each virus
  skipHeader* notVaccinated;
  VirlistNode* next;
public:
  VirlistNode(string i, int l);
  ~VirlistNode();

  string getItem();
  bloomFilter* getBloom();
  skipHeader* getVacced();
  skipHeader* getNotVacced();
  VirlistNode* getNext();

  void setItem(string i);
  void setNext(VirlistNode* n);

  void insertRecord(int* id, citizenRecord* c, string v, string dv, bool flag);
  void insertBloom(string n);
};

class VirlistHeader{
  VirlistNode* start;
  VirlistNode* end;
  int vir_count;
  int bloom_len;//used so when we creat a new virus it gets it as argument
public:
  VirlistHeader(int);
  ~VirlistHeader();

  string* getBlooms();
  int getCount();

  VirlistNode* insertVirus(string i);
  VirlistNode* searchVirus(string);
  void vaccineStatusBloom(int i, string v);
  void vaccineStatus(int i, string v);
  void vaccineStatus(int i);
  string vaccineStatus(int i, string v, bool prints);
  SRListHeader* vaccineStatus(int i, bool prints);
  bool vaccinateNow(int i, string v);
  void listNonVaccinatedPersons(string v);
};

#endif
