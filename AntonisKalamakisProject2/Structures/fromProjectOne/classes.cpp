#include <cstring>

#include "classes.hpp"
citizenRecord::citizenRecord(string id, string fn, string ln, string* ct, string a){
  citizenId = new int(stoi(id));
  firstName = fn;
  lastName= ln;
  country= ct;
  age = stoi(a);
}

citizenRecord::~citizenRecord(){
  delete citizenId;
}

void citizenRecord::print(){
  cout << *citizenId << " " <<
  firstName << " " <<
  lastName << " " <<
  *country << " " <<
  age << endl;
}

string flipDate(string t){//used to flip the date incase we need to print a date in normal form
    string new_date= "";
    new_date=t.substr(0, t.find('-'))+new_date;
    t=t.substr(t.find('-')+1);
    new_date=t.substr(0, t.find('-')+1)+new_date;
    t=t.substr(t.find('-')+1);
    new_date=t+'-'+new_date;
    return new_date;
}

string checkAndFormatDate(string t){//used to format and check the date
  for(int i=0;i<(int)t.length();i++){//for example fixes a date from beeing: 1-2-2001 -> 01-02-2001 etc
    int x = t[i];
    if(x != '-'){
      if(x < '0' || x > '9'){
        return "";
      }
    }
  }
  string new_date  = "";

  if(t.find('-') != 2 && t.find('-') != 1){
    string to_return = "";
    string temp="";
    for (int i=0;i<(int)t.length();i++) {
      if(t[i] == '-'){
        to_return = '-' + temp + to_return ;
        temp = "";
        continue;
      }
      temp = temp+t[i];
    }
    to_return = temp + to_return;
    t= to_return;
    if(t.find('-') == 1){
      new_date.append('0' + t.substr(0,t.find('-')+1));
    }else{
      new_date.append(t.substr(0,t.find('-')+1));
    }
    t=t.substr(t.find('-')+1);
    if(t.find('-') == 1){
      new_date.append('0' + t.substr(0,t.find('-')+1));
    }else{
      new_date.append(t.substr(0,t.find('-')+1));
    }
    t=t.substr(t.find('-')+1);
    new_date.append(t);
    to_return = "";
    temp = "";
    for (int i=0;i<(int)new_date.length();i++) {
      if(new_date[i] == '-'){
        to_return = '-' + temp + to_return ;
        temp = "";
        continue;
      }
      temp = temp+new_date[i];
    }
    to_return = temp + to_return;
    return to_return;
  }else{
    if(t.find('-') == 1){
      new_date.append('0' + t.substr(0,t.find('-')+1));
    }else{
      new_date.append(t.substr(0,t.find('-')+1));
    }
    t=t.substr(t.find('-')+1);
    if(t.find('-') == 1){
      new_date.append('0' + t.substr(0,t.find('-')+1));
    }else{
      new_date.append(t.substr(0,t.find('-')+1));
    }
    t=t.substr(t.find('-')+1);
    new_date.append(t);

    string to_return = "";
    string temp="";
    for (int i=0;i<(int)new_date.length();i++) {
      if(new_date[i] == '-'){
        to_return = '-' + temp + to_return ;
        temp = "";
        continue;
      }
      temp = temp+new_date[i];
    }
    to_return = temp + to_return;
    return to_return;
  }
}

////////////////////SRListNode
SRListNode::SRListNode(string v, int vacc, string d, SRListNode* n){
  vir = v;
  vacced = vacc;
  date = d;
  next = n;
}
////////////////////SRListHeader
SRListHeader::SRListHeader(){
  start = NULL;
  len = 0;
}
SRListHeader::~SRListHeader(){
  SRListNode* temp= start;
  while(temp != NULL){
    SRListNode* t= temp;
    start = temp->next;
    delete t;
  }
}
void SRListHeader::insert(string vir, int vacced, string date){//simple insert at start of list function
  start = new SRListNode(vir, vacced, date, start);
  len++;
}

SRListNode* SRListHeader::pop(){//pop the first node or return null if empty
  if(start == NULL){
    return NULL;
  }
  SRListNode* temp= start;
  start = temp->next;
  len--;
  return temp;
}
