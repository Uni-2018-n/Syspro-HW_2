#include <iostream>
#include "countryList.hpp"


ClistNode::ClistNode(string i){
  item = new string(i);
  next = NULL;
}

ClistNode::~ClistNode(){
  delete item;
}


ClistHeader::ClistHeader(){
  start = NULL;
  end = NULL;
}

ClistHeader::~ClistHeader(){
  ClistNode* temp = start;
  ClistNode* tmp = NULL;
  while(temp != NULL){
    tmp = temp;
    temp= temp->next;
    delete tmp;
  }
}

string* ClistHeader::searchItem(string i){
  ClistNode* temp = start;
  while(temp != NULL){
    if(temp->item->compare(i) == 0){
      return temp->item;
    }
    temp = temp->next;
  }
  return NULL;
}

string* ClistHeader::insertItem(string i){
  ClistNode* new_node = new ClistNode(i);
  if(start == NULL){
    start = new_node;
    end = new_node;
  }else{
    end->next = new_node;
    end = new_node;
  }
  return new_node->item;
}
