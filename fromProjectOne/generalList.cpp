#include "generalList.hpp"
#include "Structures/virusesList.hpp"
#include "classes.hpp"
#include <cstring>
#include <dirent.h>
#include <fstream>
#include <string>
#include <unistd.h>

////////////////////////////listNode
listNode::listNode(citizenRecord* c){
  citizen = c;
  next = NULL;
}

listNode::~listNode(){
  delete citizen;
}

citizenRecord* listNode::getCitizen(){
  return citizen;
}
void listNode::setNext(listNode* n){
  next = n;
}
listNode* listNode::getNext(){
  return next;
}

////////////////////////////GlistHeader
GlistHeader::GlistHeader(int bloom){
  start = NULL;
  end = NULL;
  countries = new ClistHeader();
  viruses = new VirlistHeader(bloom);//bloom is the bloom size needed for the bloom filter inside each virus
}

GlistHeader::~GlistHeader(){
  listNode* temp = start;
  listNode* tmp = NULL;
  while(temp != NULL){
    tmp = temp;
    temp = temp->getNext();
    delete tmp;
  }
  delete countries;
  delete viruses;
}

listNode* GlistHeader::searchCitizen(int id){//simple search method for the citizend based on their id
  listNode* temp = start;
  while(temp != NULL){
    if(*(temp->getCitizen()->citizenId) == id){
      return temp;
    }
    temp = temp->getNext();
  }
  return NULL;
}

void GlistHeader::insertRecord(string line, bool flag){
  string temp[8];//converting the line we have into a string array
  int i=0;
  string word = "";
  for(auto x : line){
    if( x== ' '){
      temp[i] = word;
      i++;
      word ="";
    }else{
      word = word + x;
    }
  }
  temp[i] = word;

  {
    int errors = stoi(temp[4]);//error checking with all the emplementation details
    if(errors < 1 || errors > 120){
      cout << "ERROR found record with age out of boundaries " << line << endl;
      return;
    }
    if((temp[6] == "YES" && temp[7] == "") || (temp[6] == "NO" && temp[7] != "")){
      cout << "ERROR found record with wrong vaccination input " << line << endl;
      return;
    }
  }
  if(temp[6] == "YES"){
    temp[7] = checkAndFormatDate(temp[7]);
    if(temp[7] == ""){
      cout << "ERROR WRONG FORMAT DATE" << endl;
      return;
    }
  }

  listNode* tmp = searchCitizen(stoi(temp[0]));//searching if the citizen is already inside so we wont have any dublication
  if(tmp != NULL){//if its already inside we do a full check if its the same citizen or a citizen with the same id but diffrent data etc
    if(tmp->getCitizen()->firstName.compare(temp[1]) !=0 ||
      tmp->getCitizen()->lastName.compare(temp[2]) !=0 ||
      tmp->getCitizen()->country->compare(temp[3]) !=0 ||
      tmp->getCitizen()->age != stoi(temp[4])){
        cout << "ERROR found record with same id but different data " << line << endl;
        return;
    }

    VirlistNode* temp_virus = viruses->searchVirus(temp[5]);//checking if virus already exists in our list
    if(temp_virus == NULL){
      temp_virus = viruses->insertVirus(temp[5]);//if not we add the virus
    }
    temp_virus->insertRecord(tmp->getCitizen()->citizenId, tmp->getCitizen(), temp[6], temp[7], flag);//and we add the citizen to the virus to be marked as vaccinated or not vaccinated
  }else{//incase the citizen is new and we need to add to the list
    string* temp_country = countries->searchItem(temp[3]);//we check if the country that the citizen is from already exists in the list
    if(temp_country == NULL){
      temp_country = countries->insertItem(temp[3]);
    }
    citizenRecord*  temp_citizen = new citizenRecord(temp[0], temp[1], temp[2], temp_country, temp[4]);//create a new citizenRecord with the data of the citizen

    listNode* new_node = new listNode(temp_citizen);//a new listnode to be added inside the citizens list
    if(start == NULL){//case citizen is first to be inserted
      start = new_node;
      end = new_node;
    }else{//if not, just insert citizen at end of list
      end->setNext(new_node);
      end = new_node;
    }

    VirlistNode* temp_virus = viruses->searchVirus(temp[5]);//search the virus and if needed create it
    if(temp_virus == NULL){
      temp_virus = viruses->insertVirus(temp[5]);
    }
    temp_virus->insertRecord(new_node->getCitizen()->citizenId, new_node->getCitizen(), temp[6], temp[7], flag);//and add the citizen to the virus to be marked vaccinated or not 
  }
}

string* GlistHeader::getBlooms(){
  return viruses->getBlooms();
}

int GlistHeader::getCountViruses(){
  return viruses->getCount();
}

void GlistHeader::vaccineStatusBloom(int i, string v){
  viruses->vaccineStatusBloom(i,v);
}

void GlistHeader::vaccineStatus(int i, string v){
  viruses->vaccineStatus(i, v);
}

void GlistHeader::vaccineStatus(int i){
  viruses->vaccineStatus(i);
}

string GlistHeader::vaccineStatus(int i, string v, bool prints){
  return viruses->vaccineStatus(i, v, prints);
}

SRListHeader* GlistHeader::vaccineStatus(int i, bool prints){//return a SRListHeader list with all the vaccinations the citizen was done
  listNode* citizen = this->searchCitizen(i);
  if(citizen == NULL){
    return NULL;
  }
  SRListHeader* temp = viruses->vaccineStatus(i, prints);
  temp->citizen = citizen->getCitizen();
  return temp;
}

void GlistHeader::insertCitizenRecord(string line){
  insertRecord(line, true);
}

void GlistHeader::vaccinateNow(int i, string fn, string ln, string c, string a, string v){
  if(!viruses->vaccinateNow(i, v)){//check if citizen already vaccinated or already not vaccinated 
    time_t t= time(0);//if not insert citizen into the database with date vaccinated the current date
    tm* n = localtime(&t);
    string dv = to_string(n->tm_mday) + "-" + to_string(n->tm_mon+1) + "-" + to_string(n->tm_year + 1900);
    insertRecord(to_string(i) + " " + fn + " " + ln + " " + c + " " + a + " " + v + " YES " + dv, false);
  }
}

void GlistHeader::listNonVaccinatedPersons(string v){
  viruses->listNonVaccinatedPersons(v);
}