#include <iostream>
#include <cstring>
#include "bloomFilter.hpp"
#include <unistd.h>

using namespace std;

bloomFilter::bloomFilter(int l):
len(l){
  array = new char[len]();//initializes an array of chars with 0 so every bit is empty
  for(int i=0;i<len;i++){
    array[i] = 0;
  }
}

bloomFilter::~bloomFilter(){
  delete[] array;
}

void bloomFilter::insert(int s){
  unsigned char* s_char = new unsigned char[sizeof(s)]();
  for(int i=0;i<k;i++){//we need to change k bits per number
    memcpy(s_char, &s, sizeof(s));
    int bit= hash_i(s_char,i) % len;
    int array_index = bit / 8; //find the index in the array
    int bit_index = bit % 8; //find the index of the bit inside the array's cell
    array[array_index] |= 1 << bit_index;// store it
  }
  delete[] s_char;
}

void bloomFilter::insert(string k){
  for(int i=0;i<len;i++){
    array[i] = array[i] | k[i]; //logical or each k[i] with the array[i] to simply append any new data but also keep the old data
  }
}

bool bloomFilter::is_inside(int s){
  unsigned char* s_char = new unsigned char[sizeof(s)]();
  for(int i=0;i<k;i++){//hash k times the number
    memcpy(s_char, &s, sizeof(s));
    int bit= hash_i(s_char,i) % len;
    int array_index = bit / 8;
    int bit_index = bit % 8;
    if((array[array_index] & (1 << bit_index)) ==0){
      delete[] s_char;
      return false;//even if one bit is 0 return, if all of them are 1 will go to the end of the function
    }
  }
  delete[] s_char;
  return true;
}

string bloomFilter::toString(){//since our bloom array is an array of chars create a string object and fill it with the data from the array
  string temp = "";
  for(int i=0;i<len;i++){
    temp = temp + array[i];
  }
  return temp;
}



//given hash funcitons 
unsigned long bloomFilter::djb2(unsigned char *str) {
	unsigned long hash = 5381;
	int c;
	while ((c = *str++)) {
		hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
	}
	return hash;
}


unsigned long bloomFilter::sdbm(unsigned char *str) {
	unsigned long hash = 0;
	int c;

	while ((c = *str++)) {
		hash = c + (hash << 6) + (hash << 16) - hash;
	}

	return hash;
}


unsigned long bloomFilter::hash_i(unsigned char *str, unsigned int i) {
	return djb2(str) + i*sdbm(str) + i*i;
}
