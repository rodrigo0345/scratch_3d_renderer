#!/bin/zsh

mkdir -p build;
cd ./build; cmake ..; make; cd ..; ./build/rendering_engine 
