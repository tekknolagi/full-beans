#!/bin/bash

OS_NAME=`uname -o 2>/dev/null || uname -s`

if [ $OS_NAME == "Msys" ]; then
    GLFLAG="-lgdi32"
elif [ $OS_NAME == "Darwin" ]; then
    GLFLAG="-framework Cocoa"
else
    GLFLAG="-lX11"
fi

CFLAGS="-I../ -Wall -std=c11 -pedantic $GLFLAG -lm -O2 -g"

gcc main.c renderer.c ../microui.c $CFLAGS

