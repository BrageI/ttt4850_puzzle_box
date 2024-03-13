#!/bin/bash
g++ -o puzzle_box main.cpp \
  -lpthread \
  -Wall \
  event.cpp \
  state_machine.cpp \
  book.cpp \
  ultrasound_reader.cpp \
  led_control.cpp -lws2811 \
  -lpigpio -lrt
