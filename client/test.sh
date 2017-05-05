#!/bin/bash

cat testFirst - | java -cp build Client testFirst &
for i in `seq 2 $1`;
do
java -cp build Client < test 2>&1 > /dev/null &
done

