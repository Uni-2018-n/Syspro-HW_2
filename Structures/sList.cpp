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
    if(start == NULL){//case list is empty
        start= new SLnode(txt);
        count++;
        return;
    }
    if(strcmp(txt.c_str(), start->getTxt().c_str()) <0){//our node is smaller than the first node of the list
        start = new SLnode(txt, start);//insert at start
        count++;
        return;
    }
    if(start != NULL){//otherwise go ahead and find a spot in the list 
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

string SLHeader::popFirst(){//since we have a sorted list popFirst is really usefull
    if(start == NULL){
        return "";//incase of an empty list we send "" to indicate it
    }
    string temp = start->getTxt();
    SLnode* t = start;
    start = start->getNext();
    delete t;
    return temp;//we care about the string and not the node so its easier to delete nodes from here
}