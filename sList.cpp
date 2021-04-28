#include "sList.hpp"
#include <cstring>
#include <iostream>
//SLnode
SLnode::SLnode(string t){
    txt = t;
    next = NULL;
}
SLnode::SLnode(string t, SLnode* n){
    txt = t;
    next = n;
}

string SLnode::getTxt(){
    return txt;
}

SLnode* SLnode::getNext(){
    return next;
}

void SLnode::setNext(SLnode *n){
    next = n;
}

//SLHeader
SLHeader::SLHeader(){
    start = NULL;
}

SLHeader::~SLHeader(){
    while(start != NULL){
        SLnode* t = start;
        start = start->getNext();
        delete t;
    }
}

void SLHeader::insert(string txt){
    if(start == NULL){
        start= new SLnode(txt);    
        count++;
        return;
    }
    if(strcmp(txt.c_str(), start->getTxt().c_str()) <0){
        start = new SLnode(txt, start);
        count++;
        return;
    }
    if(start != NULL){
        SLnode* temp = start;
        while(temp->getNext() != NULL){
            if(strcmp(txt.c_str(), temp->getNext()->getTxt().c_str()) <0){
                temp->setNext(new SLnode(txt, temp->getNext()));
                count++;
                return;
            }
            temp = temp->getNext();
        }
        temp->setNext(new SLnode(txt));
        count++;
        return;
    }
}

string SLHeader::popFirst(){
    if(start == NULL){
        return "";
    }
    string temp = start->getTxt();
    SLnode* t = start;
    start = start->getNext();
    delete t;
    return temp;
}

void SLHeader::test_print(){
    SLnode* temp = start;
    while(temp != NULL){
        cout << temp->getTxt() << endl;
        temp = temp->getNext();
    }
}