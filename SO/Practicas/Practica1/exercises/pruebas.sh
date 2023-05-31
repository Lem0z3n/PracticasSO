#! bin/bash

PROG= student_records
FILE= records.txt 

if [ -x $PROG]; then
	if [ -r $FILE]; then
		records= `cat records.txt`
		i= 0
		for record in $records
		do
		if [ $i == 0	]; then
			student_records -f database.txt -c $record
		elif then
			student_records -f database.txt -a $record
		fi
		i= 1
		done
		student_records -f database.txt -l 
		xxd database.txt

		for record in $records
		do
		query= `echo $record | cut -d ':' -f 2`
		student_records -f database.txt -q -n $query	
		done
	fi
fi			##bucle for record in $records
			##echo $record | cut -d ':' -f 2	 
