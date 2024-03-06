#include "puzzle.h"
#include "event.h"

string getRandomMusicFile() {

        string musicFiles[] = {"music1.wav", "music2.wav", "music3.wav"};
        string musicFile = musicFiles[rand() % size(musicFiles)];

        return musicFile;
    }

