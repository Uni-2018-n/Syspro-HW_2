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

mkdir -p $2

declare -A dict

readarray lines < "$1"
for line in "${lines[@]}"
do
    curr=(${line})
    if [ ! -e "$2/${curr[3]}" ]
    then
        mkdir -p "$2/${curr[3]}"
    fi
    if [ ! -v dict[${curr[3]}] ]
    then 
        touch "$2/${curr[3]}/${curr[3]}-1.txt"
        echo -n "$line" >> "$2/${curr[3]}/${curr[3]}-1.txt"
        dict[${curr[3]}]=2
        continue
    fi
    if [ ! -e "$2/${curr[3]}/${curr[3]}-${dict[${curr[3]}]}.txt" ]
    then
        touch "$2/${curr[3]}/${curr[3]}-${dict[${curr[3]}]}.txt"
    fi

    echo -n "$line" >> "$2/${curr[3]}/${curr[3]}-${dict[${curr[3]}]}.txt"

    if [ "${dict[${curr[3]}]}" -eq "$3" ]
    then 
        dict[${curr[3]}]=1
    else
        dict[${curr[3]}]="$((${dict[${curr[3]}]} + 1))"
    fi
done

exit 0