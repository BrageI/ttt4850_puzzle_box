#pragma once

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <vector>
using namespace std;


// Represents a music file with its corresponding puzzle order
struct MusicFileInfo {
    std::string fileName;
    std::vector<int> puzzleOrder;
};

MusicFileInfo musicFiles[] = {
    {"1.wav", {1, 2, 3, 4, 5}},
    {"2.wav", {1, 2, 3, 4, 5}},
    {"3.wav", {1, 2, 3, 4, 5}},
    {"4.wav", {1, 2, 3, 4, 5}},
    {"5.wav", {1, 2, 3, 4, 5}},
    {"6.wav", {1, 2, 3, 4, 5}},
    {"7.wav", {1, 2, 3, 4, 5}},
    {"8.wav", {1, 2, 3, 4, 5}},
    {"9.wav", {1, 2, 3, 4, 5}},
    {"10.wav", {1, 2, 3, 4, 5}},
    {"11.wav", {1, 2, 3, 4, 5}}

};

string getRandomMusicFile();



