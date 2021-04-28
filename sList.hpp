#ifndef _SLIST_HPP_
#define _SLIST_HPP_
//sorted list for strings

#include <string>

using namespace std;

class SLnode{
    string txt;
    SLnode* next;
public:
    SLnode(string t);
    SLnode(string t, SLnode* n);
    string getTxt();
    SLnode* getNext();
    void setNext(SLnode* n);
};

class SLHeader{
    SLnode* start;
public:
    int count=0;
    SLHeader();
    ~SLHeader();
    void insert(string txt);
    string popFirst();
    void test_print();
};

#endif