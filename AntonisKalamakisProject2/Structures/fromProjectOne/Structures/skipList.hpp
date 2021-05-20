#ifndef _SKIPLIST_HPP_
#define _SKIPLIST_HPP_

#include <iostream>
#include "../classes.hpp"
using namespace std;

#define MAXIMUM 5


//dual linked list of dual linked lists.
//number of layers is a fixed number. If a layer is empty simply just leave it as NULL.

class SkiplistNode{//layer's nodes
  int* item;
  citizenRecord* citizen;
  string* date_vaccinated;
  SkiplistNode* next;
  SkiplistNode* prev;
  SkiplistNode* lower_level;
public:
  SkiplistNode(int* i, citizenRecord* c, string* dv);
  SkiplistNode(SkiplistNode* i);
  SkiplistNode(SkiplistNode* i, SkiplistNode* n);
  SkiplistNode(int* i, citizenRecord* c, string* dv, SkiplistNode* n);
  ~SkiplistNode();

  int getItem();
  citizenRecord* getCitizen();
  string getDateVaccinated();
  SkiplistNode* getNext();
  SkiplistNode* getPrev();
  SkiplistNode* getLowerLevel();
  void setNext(SkiplistNode* n);
  void setPrev(SkiplistNode* p);

  void print();
};

class SkiplistHeader{//header for each layer
  SkiplistNode* start;
public:
  SkiplistHeader();
  ~SkiplistHeader();
  SkiplistNode* insertAtStart(SkiplistNode* i);
  SkiplistNode* insertItem(int* i, citizenRecord* c, string* dv, int top_lvl);
  SkiplistNode* searchItem(int i);
  SkiplistNode* deleteItem(int i);

  SkiplistNode* getFirst();
  void removeFirst();
  void print();
};
/////////////////////////////////////
class skipNode{//node of each layer
  SkiplistHeader* item;
  skipNode* prev;
  skipNode* next;
public:
  skipNode();
  ~skipNode();
  SkiplistNode* insertAtStart(SkiplistNode* i);

  skipNode* getNext();
  skipNode* getPrev();
  SkiplistHeader* getItem();
  void setNext(skipNode* n);
  void setPrev(skipNode* p);

  void removeFirst();
  void print();
};

class skipHeader{//header of skiplist
  skipNode* start;
  skipNode* end;
  int lvlnum;
  int max_lvl;
  void addLayer();
public:
  skipHeader();
  ~skipHeader();
  SkiplistNode* searchItem(int i);
  SkiplistNode* insertItem(int* i, citizenRecord* c, string* dv);
  SkiplistNode* deleteItem(int i);

  void print();
};
#endif
