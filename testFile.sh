#!/bin/bash
if [ $# -ne 4 ]
then
  echo Wrong Input
  exit 1
fi

OUTFILE=input.txt;
touch "${OUTFILE}"; rm "${OUTFILE}"; touch "${OUTFILE}";
if [ $4 -eq 1 ]
then
  id=( `shuf -i 1-9999 -r -n$3` ) #array of id
else
  if [ $3 -gt 10000 ]
  then
    echo "ERROR: cant generate more ID's than 10000(0-9999)"
    id=( `shuf -i 0-9999 -n10000` )
  else
    id=( `shuf -i 0-9999 -n$3` )
  fi
fi
c=( `shuf -r -n$3 $2` ) #array of countries
v=( `shuf -r -n$3 $1` ) #array of viruses
a=( `shuf -i 1-120 -r -n$3` ) #array of ages
i=0;
while [ $i -lt "$3" ]
  do

    idd=${id[$i]};
    echo -n "${idd} "

    t=`expr $RANDOM % 12`; #we need to get a random value from 3 to 12
    let nam=t+3;
    fn=`shuf -zer -r -n$nam {a..z} | tr -d '\0'`
    echo -n "${fn} "

    t=`expr $RANDOM % 12`;
    let nam=t+3;
    ln=`shuf -zer -r -n$nam {a..z} | tr -d '\0'`
    echo -n "${ln} "

    cc=${c[$i]};
    echo -n "${cc} "

    aa=${a[$i]};
    echo -n "${aa} "

    echo -n "${v[$i]} "

    t=`expr $RANDOM % 100`;
    let tmp=t+1;
    if [ ${tmp} -gt 50 ]
    then
      vacced="YES"
      t=`expr $RANDOM % 30`;
      let d=t+1;
      t=`expr $RANDOM % 12`;
      let m=t+1;
      t=`expr $RANDOM % 601`; #years can get from 1700-2300
      let y=t+1700;
      vacced="${vacced} ${d}-${m}-${y}";
    else
      vacced="NO"
    fi

    echo -n "${vacced}"
    echo
    i=$(($i+1))
    if [ $4 -eq 1 ] && [ $i -lt "$3" ] #if dublicates are allowed and we can make more records
    then
      t=`expr $RANDOM % 100`;
      let p=t+1;
      while [ $p -lt 20 ] && [ $i -lt "$3" ] #20% chance of having a duplicate
      do
        echo -n "${idd} ${fn} ${ln} ${cc} ${aa} "
        echo -n "${v[$i]} "

        t=`expr $RANDOM % 100`;
        let tmp=t+1;
        if [ ${tmp} -gt 50 ]
        then
          vacced="YES"
          t=`expr $RANDOM % 30`;
          let d=t+1;
          t=`expr $RANDOM % 12`;
          let m=t+1;
          t=`expr $RANDOM % 601`;
          let y=t+1700;
          vacced="${vacced} ${d}-${m}-${y}";
        else
          vacced="NO"
        fi
        echo -n "${vacced}"
        echo
        t=`expr $RANDOM % 100`;
        let p=t+1;
        i=$(($i+1))
      done
    fi
done > "$OUTFILE"

exit 0
