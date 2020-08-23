#!/usr/bin/zsh
clang++ -c -g comgr.cc -o comgr.o `llvm-config --cxxflags`
clang++ -g comgr.o -o comgr `llvm-config --cxxflags --ldflags --libs all` -pthread -ldl -ltinfo -lz -lclang -lclang-cpp10
