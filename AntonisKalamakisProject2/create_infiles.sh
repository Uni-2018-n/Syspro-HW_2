#!/bin/bash
if [ $# -ne 3 ] #check if the provided input is correct
then
    echo Wrong Input
    exit 1
fi

if [ ! -e "$1" ] || [ ! -f "$1" ] || [ ! -r "$1" ] #check if the files needed are in fact files
then 
    echo Problem with inputFile
    exit 1
fi

if [ -e "$2" ] && [ -d "$2" ] #and check if the name of the input_dir we will create already exists or not
then
    echo input_dir already exists!
    exit 1
fi

if ! echo "$3" | grep -qE '^[0-9]+$' #finally check if the numFilesPerDirectory variable is a number or not
then 
    echo numFilesPerDirectory not a number!
    exit 1
fi

mkdir -p $2 #create the directory

declare -A dict #create a dictionary array 

readarray lines < "$1"  #read each line from the input file and add it into an array
for line in "${lines[@]}" #for each line in the array
do
    curr=(${line}) #make the line a array of words
    if [ ! -e "$2/${curr[3]}" ] #since forth word is the country see if there is a directory for the country
    then
        mkdir -p "$2/${curr[3]}" #if not make it
    fi
    if [ ! -v dict[${curr[3]}] ] #if country dosent exist in the dictionary
    then 
        touch "$2/${curr[3]}/${curr[3]}-1.txt" #create the first file for the country
        echo -n "$line" >> "$2/${curr[3]}/${curr[3]}-1.txt" #print the line in the first file
        dict[${curr[3]}]=2 #top the file counter(so next loop prints to the next file)
        continue #and continue
    fi
    if [ ! -e "$2/${curr[3]}/${curr[3]}-${dict[${curr[3]}]}.txt" ] #if the file isnt yet created(file with index 1<)
    then
        touch "$2/${curr[3]}/${curr[3]}-${dict[${curr[3]}]}.txt" #create it
    fi

    echo -n "$line" >> "$2/${curr[3]}/${curr[3]}-${dict[${curr[3]}]}.txt" #and print the line

    if [ "${dict[${curr[3]}]}" -eq "$3" ] #if the file count has reached the limit of files provided from user
    then 
        dict[${curr[3]}]=1 #reset to one
    else
        dict[${curr[3]}]="$((${dict[${curr[3]}]} + 1))" #else top it
    fi
done

exit 0