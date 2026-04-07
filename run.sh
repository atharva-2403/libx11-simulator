#!/bin/bash

if [ "$1" == "cube" ]; then
    gcc cube.c -o cube -lX11 -lm
    ./cube

elif [ "$1" == "pyramid" ]; then
    gcc pyramid.c -o pyramid -lX11 -lm
    ./pyramid

elif [ "$1" == "sphere" ]; then
    gcc sphere.c -o sphere -lX11 -lm
    ./sphere

else
    echo "Usage: ./run.sh [cube|pyramid|sphere]"
fi