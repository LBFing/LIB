#!/bin/sh

FILE=`ls *.proto`

for line in $FILE
do
	echo $line
	protoc -I=. --cpp_out=. $line
done


