#!/bin/zsh

mkdir -p build;
cd ./build; cmake ..; make debug; cd ..; ./build/rendering_engine 
