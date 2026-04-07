#!/bin/bash

if [ "$1" == "cube" ]; then
    gcc -Wall -Wextra -O2 -o cube cube.c -lX11 -lm
    ./cube

elif [ "$1" == "pyramid" ]; then
    gcc -Wall -Wextra -O2 -o pyramid pyramid.c -lX11 -lm
    ./pyramid

elif [ "$1" == "sphere" ]; then
    gcc -Wall -Wextra -O2 -o sphere sphere.c -lX11 -lm
    ./sphere

else
    echo "Usage: ./run.sh [cube|pyramid|sphere]"
fi
