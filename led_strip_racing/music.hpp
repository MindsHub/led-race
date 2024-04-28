#pragma once
#include <Arduino.h>
#include "song.hpp"

void myTone(uint16_t frequency);
void myNoTone();

void reproduceMusic(int track_index, int tempo);
void reproduceMusicProgressive(int track_index, double pos, unsigned long realMillis);
