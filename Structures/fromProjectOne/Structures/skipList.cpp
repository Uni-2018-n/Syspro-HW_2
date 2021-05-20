#include <iostream>
#include "skipList.hpp"
using namespace std;

//////////////////////////// SkiplistNode
SkiplistNode::SkiplistNode(int* i, citizenRecord* c, string* dv){//initialize i
  item = i;
  citizen = c;
  date_vaccinated = dv;
  lower_level = NULL;
  next = NULL;
  prev = NULL;
}

SkiplistNode::SkiplistNode(SkiplistNode* i){//copy i and set lower_level
  item = i->item;
  date_vaccinated = NULL;
  citizen = NULL;
  lower_level = i;
  next = NULL;
  prev = NULL;
}

SkiplistNode::SkiplistNode(SkiplistNode* i, SkiplistNode* n){//copy i and set it to lower lever also add next node
  item = i->item;
  date_vaccinated = NULL;
  citizen = NULL;
  next = n;
  prev = NULL;
  lower_level= i;
}

SkiplistNode::SkiplistNode(int* i, citizenRecord* c, string* dv, SkiplistNode* n){//initialize i and set next node
  item = i;
  date_vaccinated = dv;
  citizen = c;
  next = n;
  prev = NULL;
  lower_level = NULL;
}

SkiplistNode::~SkiplistNode(){
  if(date_vaccinated != NULL){
    delete date_vaccinated;
  }
}

void SkiplistNode::print(){
  citizen->print();
}

int SkiplistNode::getItem(){
  return *item;
}
citizenRecord* SkiplistNode::getCitizen(){
  return citizen;
}
SkiplistNode* SkiplistNode::getNext(){
  return next;
}
void SkiplistNode::setNext(SkiplistNode* n){
  next = n;
}
SkiplistNode* SkiplistNode::getLowerLevel(){
  return lower_level;
}
void SkiplistNode::setPrev(SkiplistNode* p){
  prev = p;
}
SkiplistNode* SkiplistNode::getPrev(){
  return prev;
}

string SkiplistNode::getDateVaccinated(){
  return *date_vaccinated;
}

//////////////////////////// SkiplistHeader
SkiplistHeader::SkiplistHeader(){
  start = NULL;
}

SkiplistHeader::~SkiplistHeader(){
  SkiplistNode* temp = start;
  SkiplistNode* tmp = NULL;
  while(temp != NULL){
    tmp = temp->getNext();
    delete temp;
    temp= tmp;
  }
}

SkiplistNode* SkiplistHeader::insertAtStart(SkiplistNode* i){
  SkiplistNode* new_item = new SkiplistNode(i, start);
  if(start != NULL){
    start->setPrev(new_item);
    new_item->setNext(start);
  }
  start = new_item;
  return new_item;
}

SkiplistNode* SkiplistHeader::searchItem(int i){//returns NULL or the node if found
  SkiplistNode* temp = start;
  if(temp == NULL){//if layer empty return null
    return NULL;
  }
  if(temp->getItem() == i){//if first item of the layer is the desired item
    while(temp->getLowerLevel() != NULL){//go to the last layer
      temp= temp->getLowerLevel();
    }
    return temp;//and return the item's node
  }else if(temp->getItem() > i){//if the layer first item of this layer is biger than the item we need to find
    return temp;//we cant find it from here so we return the first item to tell the skipHeader to search the lower layer
  }
  if(temp->getNext() == NULL){//if only one item exists inside the layer
    if(temp->getItem() < i){
      while(temp->getNext() == NULL){//go to the lower level for every level that this item is the last on the level
        temp = temp->getLowerLevel();
        if(temp == NULL){//if this is null then we cant find it because we've gone to the last layer(full list) and this is the last item in that list
          return NULL;
        }
      }
    }
  }
  //if we go here we will have one node with item < than the requested item and with existing next node

  while(temp->getNext() != NULL){//check if next node exists(to keep the loop going or not)
    if(temp->getNext()->getItem() == i){//if we've found the item
      temp = temp->getNext();//go to the node with the item
      while(temp->getLowerLevel() != NULL){//go to the original node of the item(last layer's node for the item)
        temp = temp->getLowerLevel();
      }
      return temp;//and return it
    }
    if(temp->getNext()->getItem() > i){//if the next item is bigger than the desired item
      if(temp->getLowerLevel() == NULL){//check if this is the last list
        return NULL;//if yes item dosent exist
      }
      temp = temp->getLowerLevel();//if not go to the lower list to check if there is the node that we need to find between this node and the one that we just found > than the item
    }else if(temp->getNext()->getItem() < i){//if the next node's item is < than the one we need to find
      temp= temp->getNext();//we need to check the next node
      while(temp->getNext() == NULL){//if that node dosent exist we need to go one layer down
        if(temp->getLowerLevel() == NULL){//if we've reached the end of the layers 
          return NULL;//couldnt find the desired item
        }
        temp =temp->getLowerLevel();
      }
    }
  }
  return NULL;//wont go here in any case.
}

SkiplistNode* SkiplistHeader::insertItem(int* i, citizenRecord* c, string* dv, int top_lvl){
  SkiplistNode** level_array = new SkiplistNode*[MAXIMUM];//level array is used to keep track the possition we need to put the node in the layers (if needed)
  int lvl_counter=0;
  SkiplistNode* temp = start;
  if(temp == NULL){//this is the case that the skiplist is empty so we need to add a new node on the last layer(adding the node to the top layers wont happen here)
    SkiplistNode* new_node = new SkiplistNode(i, c, dv, start);
    start = new_node;
    delete[] level_array;
    return new_node;
  }
  if(temp->getItem() == *i){//in case first item in layer is the item we have return NULL cause it already exists
    delete[] level_array;
    return NULL;
  }else if(temp->getItem() > *i){//if first item of the layer is > than the item we need to insert
    if(temp->getLowerLevel() == NULL && temp->getItem() > *i){//case this is needs to be the first node of last layer
      SkiplistNode* new_node = new SkiplistNode(i, c, dv, start);
      start = new_node;
      delete[] level_array;
      return new_node;
    }

    delete[] level_array;
    return temp;//cant found this from this level
  }
  if(temp->getNext() == NULL){//incase this layer only has one item we need to go to a lower level with more items and add the positions that we've gone through to the level_array
    if(temp->getItem() < *i){
      while(temp->getNext() == NULL){
        if(temp->getLowerLevel() == NULL){//case we are in lowest level, the item needs to be in the end
          SkiplistNode* new_node = new SkiplistNode(i, c, dv);
          temp->setNext(new_node);
          new_node->setPrev(temp);
          SkiplistNode* to_return = new_node;
          {//this code is used multiple times. Top_lvl is used to determine how many layers we can go up(from the coin toss)
            while(lvl_counter> 0 && top_lvl>0){//loop untill we have put the item in every layer needed
              lvl_counter--;//lvl_counter is used to use the level_array array in correct order so we will add the nodes in the right order
              SkiplistNode* tmp = new SkiplistNode(new_node, level_array[lvl_counter]->getNext());//create a tmp node with data from the new node of the inserted item and lower_level the node of the level_array in that step
              if(level_array[lvl_counter]->getNext()!= NULL){//fix the connections to the layer's linked list so we wont miss the tmp node
                level_array[lvl_counter]->getNext()->setPrev(tmp);
              }
              level_array[lvl_counter]->setNext(tmp);
              tmp->setPrev(level_array[lvl_counter]);
              new_node = tmp;
              top_lvl--;//and go to add to the next level
            }
          }
          delete[] level_array;
          return to_return;
        }
        level_array[lvl_counter] = temp;//since we are going one layer down we need to save the spot in the array
        lvl_counter++;
        temp = temp->getLowerLevel();
      }
    }
  }

  while(temp->getNext() != NULL){//same technique with the search function but when we find the node with same item we return null and if we find a node with next > than the item we simply add it between them
    if(temp->getNext()->getItem() == *i){
      delete[] level_array;
      return NULL;//found
    }
    if(temp->getNext()->getItem() > *i){//case next node's item is > than the item we need to insert
      if(temp->getLowerLevel() == NULL){//this means that this is the layer with the full list so we need to add a new node there
        //dosent exist
        SkiplistNode* new_node = new SkiplistNode(i, c, dv, temp->getNext());
        if(temp->getNext() != NULL){
          temp->getNext()->setPrev(new_node);
        }
        temp->setNext(new_node);
        new_node->setPrev(temp);
        SkiplistNode* to_return = new_node;
        {
          while(lvl_counter> 0 && top_lvl > 0){
            lvl_counter--;
            SkiplistNode* tmp = new SkiplistNode(new_node, level_array[lvl_counter]->getNext());
            if(level_array[lvl_counter]->getNext()!= NULL){
              level_array[lvl_counter]->getNext()->setPrev(tmp);
            }
            level_array[lvl_counter]->setNext(tmp);
            tmp->setPrev(level_array[lvl_counter]);
            new_node = tmp;
            top_lvl--;
          }
        }
        delete[] level_array;
        return to_return;
      }
      level_array[lvl_counter] = temp;
      lvl_counter++;
      temp = temp->getLowerLevel();//if not go one layer down and check if there is an item between the node that we are and the next node that we just checked
    }else if(temp->getNext()->getItem() < *i){//if the next one is smaller than the item
      temp= temp->getNext();//repeat with the next node
      while(temp->getNext() == NULL){//if the next node dosent exist go ahead and go to the lower level untill a next node exists
        if(temp->getLowerLevel() == NULL){//if we've reached the lower level this means that we are in the end of the last layer so we need to add it there
          SkiplistNode* new_node = new SkiplistNode(i, c, dv);
          temp->setNext(new_node);
          new_node->setPrev(temp);
          SkiplistNode* to_return = new_node;
          {
            while(lvl_counter> 0 && top_lvl>0){
              lvl_counter--;
              SkiplistNode* tmp = new SkiplistNode(new_node, level_array[lvl_counter]->getNext());
              if(level_array[lvl_counter]->getNext()!= NULL){
                level_array[lvl_counter]->getNext()->setPrev(tmp);
              }
              level_array[lvl_counter]->setNext(tmp);
              tmp->setPrev(level_array[lvl_counter]);
              new_node = tmp;
              top_lvl--;
            }
          }
          delete[] level_array;
          return to_return;
        }
        level_array[lvl_counter] = temp;
        lvl_counter++;
        temp =temp->getLowerLevel();
      }
    }
  }
  return NULL;
}

SkiplistNode* SkiplistHeader::deleteItem(int i){//returns null if item couldnt be found 
  SkiplistNode* temp = start;
  if(temp == NULL){
    return NULL;
  }
  if(temp->getItem() == i){//just like before but go to the lower level and simply delete the nodes return the node from last layer
    while(temp->getLowerLevel() != NULL){
      if(temp->getPrev() != NULL){
        temp->getPrev()->setNext(temp->getNext());
        if(temp->getNext() != NULL){
          temp->getNext()->setPrev(temp->getPrev());
        }
        SkiplistNode* tmp = temp;
        temp = temp->getLowerLevel();
        if(temp != NULL){
          delete tmp;
        }
        continue;
      }
      temp= temp->getLowerLevel();
    }
    if(temp->getPrev() != NULL){
      temp->getPrev()->setNext(temp->getNext());
      if(temp->getNext() != NULL){
        temp->getNext()->setPrev(temp->getPrev());
      }
    }
    return temp;//found
  }else if(temp->getItem() > i){
    return temp;//cant found this from this level
  }
  if(temp->getNext() == NULL){//incase only one item exists in this layer, go to the lower layer
    if(temp->getItem() < i){
      while(temp->getNext() == NULL){
        temp = temp->getLowerLevel();
        if(temp == NULL){
          return NULL;
        }
      }
    }
  }

  while(temp->getNext() != NULL){//same with insert and search but remove the nodes and fix the connections inside the linked lists
    if(temp->getNext()->getItem() == i){
      temp = temp->getNext();
      while(temp->getLowerLevel() != NULL){
        SkiplistNode* tmp = temp;
        if(temp->getPrev() != NULL){
          temp->getPrev()->setNext(temp->getNext());
        }
        if(temp->getNext() != NULL){
          temp->getNext()->setPrev(temp->getPrev());
        }
        temp = temp->getLowerLevel();
        delete tmp;
      }
      if(temp->getPrev() != NULL){
        temp->getPrev()->setNext(temp->getNext());
      }
      if(temp->getNext() != NULL){
        temp->getNext()->setPrev(temp->getPrev());
      }
      return temp;//found
    }
    if(temp->getNext()->getItem() > i){
      if(temp->getLowerLevel() == NULL){
        return NULL;//dosent exist
      }
      temp = temp->getLowerLevel();
    }else if(temp->getNext()->getItem() < i){
      temp= temp->getNext();
      while(temp->getNext() == NULL){
        if(temp->getLowerLevel() == NULL){
          return NULL;//dosent exist
        }
        temp =temp->getLowerLevel();
      }
    }
  }
  return NULL;
}

void SkiplistHeader::print(){
  SkiplistNode* temp = start;
  while(temp != NULL){
    temp->print();
    temp = temp->getNext();
  }
}

SkiplistNode* SkiplistHeader::getFirst(){
  return start;
}

void SkiplistHeader::removeFirst(){
  SkiplistNode* temp = start->getNext();
  if(start != NULL && start->getLowerLevel() != NULL){
    delete start;
  }
  start= temp;
  if(start != NULL){
    start->setPrev(NULL);
  }
}

//////////////////////////////////////// skipNode
skipNode::skipNode(){
  item= new SkiplistHeader();
  next= NULL;
  prev= NULL;
}

SkiplistNode* skipNode::insertAtStart(SkiplistNode* i){
  return item->insertAtStart(i);
}

skipNode::~skipNode(){
  delete item;
}

void skipNode::print(){
  item->print();
}

skipNode* skipNode::getNext(){
  return next;
}
skipNode* skipNode::getPrev(){
  return prev;
}
SkiplistHeader* skipNode::getItem(){
  return item;
}
void skipNode::setNext(skipNode* n){
  next = n;
}
void skipNode::setPrev(skipNode* p){
  prev = p;
}

void skipNode::removeFirst(){
  item->removeFirst();
}

///////////////////////////////////// skipHeader
skipHeader::skipHeader(){
  skipNode* temp = new skipNode();
  start = temp;
  end = temp;
  lvlnum = 1;
  max_lvl = MAXIMUM;
  for(int i=0;i<max_lvl;i++){//create all the layers
    addLayer();
  }
}

skipHeader::~skipHeader(){
  skipNode* temp = start;
  skipNode* tmp = NULL;
  while(temp != NULL){
    tmp = temp->getNext();
    delete temp;
    temp = tmp;
  }
}

void skipHeader::addLayer(){//simple function that adds a node at the end of the list
  skipNode* temp = new skipNode();
  temp->setPrev(end);
  end->setNext(temp);
  end = end->getNext();
}

SkiplistNode* skipHeader::searchItem(int i){
  skipNode* temp = end;
  while(temp->getItem()->getFirst() == NULL){//case no items in SkiplistNodes
    temp = temp->getPrev();
    if(temp == NULL){//case no items in skip SkiplistNode
      return NULL;
    }
  }
  SkiplistNode* tmp = temp->getItem()->searchItem(i);//start searching from the first SkiplistNode with items
  if(tmp != NULL){//if it found something
      while(tmp->getItem() > i && temp->getPrev() != NULL){//if tmp->item > i that means that we need to start searching from lower level
        temp = temp->getPrev();
        tmp = temp->getItem()->searchItem(i);//search lower level
        if(tmp == NULL){//item not found
          return NULL;
        }
      }
      if(temp->getPrev() == NULL && tmp != NULL && tmp->getItem() != i){//case every first item of each level is > i
        return NULL;
      }
      return tmp;
  }else{
    return NULL;
  }
}

SkiplistNode* skipHeader::insertItem(int* i, citizenRecord* c, string* dv){
  int top_lvl = 0;//calculate in how many levels the item will go to
  {
    int f=1;
    while(f && top_lvl < max_lvl){
      if((rand() % 2 + 1) == 1){//random number 1-2 to be fair toss
        top_lvl++;
      }else{
        f=0;
      }
    }
  }
  int skiped_layers = 0;//used to see how many layers was skiped because first item is > i
  skipNode* temp = end;//use of dual linked SkiplistNode, start == SkiplistNode with all the items. with that in mind, we start our search with the SkiplistNode with the less amount of items
  while(temp->getItem()->getFirst() == NULL){//same with search
    if(temp->getPrev() == NULL){//if for example SkiplistNode with all the items is empty, need to add it there
      break;
    }
    temp = temp->getPrev();
    skiped_layers++;
  }
  SkiplistNode* tmp = temp->getItem()->insertItem(i, c, dv, top_lvl);
  bool final;
  bool final_tmp= false;
  if(tmp != NULL){
    if(tmp->getItem() == *i){//case inserted
      final= true;
    }
    while(tmp->getItem() > *i && temp->getPrev() != NULL){
      skiped_layers++;
      temp = temp->getPrev();
      tmp = temp->getItem()->insertItem(i, c, dv, top_lvl);
      if(tmp == NULL){//item couldnt be inserted, already exists
        final = false;
        final_tmp = true;
        break;
      }
    }
    if(!final_tmp){
      final = true;
    }
  }else{//item couldnt be inserted, already exists
    final = false;
  }
  SkiplistNode* to_return = tmp;
  if(final){//insert the item to every level that it needs to
    int t= max_lvl-skiped_layers;
    t= top_lvl-t;
    while(t>0 && temp->getNext() != NULL){
      temp = temp->getNext();
      tmp = temp->insertAtStart(tmp);//the first item of every skiped SkiplistNode is >i so insert i as first
      t--;
    }
    return to_return;
  }else{
    delete dv;
    return NULL;
  }
}

SkiplistNode* skipHeader::deleteItem(int i){
  skipNode* temp = end;
  while(temp->getItem()->getFirst() == NULL){//case no items in SkiplistNodes
    temp = temp->getPrev();
    if(temp == NULL){//case no items in skip SkiplistNode
      return NULL;
    }
  }
  SkiplistNode* tmp = temp->getItem()->deleteItem(i);//start searching from the first SkiplistNode with items
  if(tmp != NULL){//if it found something
      while(tmp->getItem() > i && temp->getPrev() != NULL){//if tmp->item > i that means that we need to start searching from lower level
        temp = temp->getPrev();
        tmp = temp->getItem()->deleteItem(i);//search lower level
        if(tmp == NULL){//item not found
          return NULL;
        }
      }
      if(temp->getPrev() == NULL && tmp != NULL && tmp->getItem() != i){//case every first item of each level is > i
        return NULL;
      }

      if(tmp != NULL){
        skipNode* temp = end;
        while(temp != NULL){
          if(temp->getItem()->getFirst() != NULL){
            if(temp->getItem()->getFirst()->getItem() == i){
              temp->getItem()->removeFirst();
            }
          }
          temp = temp->getPrev();
        }
      }

      return tmp;
  }else{
    return NULL;
  }
}

void skipHeader::print(){
  start->print();
}