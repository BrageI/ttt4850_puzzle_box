#!/bin/bash
g++ -o puzzle_box main.cpp \
  -lpthread \
  -Wall \
  event.cpp \
  state_machine.cpp \
  book.cpp \
  sound.cpp -lasound \
  ultrasound_reader.cpp \
  -lpigpio -lrt
