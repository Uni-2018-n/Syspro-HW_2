#!/bin/bash
if [ $# -ne 3 ]
then
    echo Wrong Input
    exit 1
fi

if [ ! -e "$1" ] || [ ! -f "$1" ] || [ ! -r "$1" ]
then 
    echo Problem with inputFile
    exit 1
fi

if [ -e "$2" ] && [ -d "$2" ]
then
    echo input_dir already exists!
    exit 1
fi

if ! echo "$3" | grep -qE '^[0-9]+$' 
then 
    echo numFilesPerDirectory not a number!
    exit 1
fi



readarray lines < "$1"

for line in "${lines[@]}"
do
    curr=(${line})
    echo ${curr[3]}
done

exit 0