#!/bin/zsh

mkdir -p build;
cd ./build; cmake ..; cd ..; ./build/rendering_engine 
