#pragma once
#include <Arduino.h>
#include "song.h"

void myTone(uint16_t frequency);
void myNoTone();

void reproduceMusic(int tempo);
void reproduceMusicProgressive(double pos);
