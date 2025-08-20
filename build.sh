#!/bin/sh

set -e

mkdir -p bin
cc main.c -o bin/testing $(sdl2-config --cflags --libs)
