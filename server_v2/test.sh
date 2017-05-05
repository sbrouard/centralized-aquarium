#!/bin/bash

echo load $1 > test
echo launch >> test
echo show aquarium >> test
cat test - | ./server
