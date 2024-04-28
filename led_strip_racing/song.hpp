#pragma once

int getLaps(int track_index);
int getTempo(int track_index);

int noteCount(int track_index);
double trackDuration(int track_index);
int getNote(int track_index, int note_index);

constexpr double dividerToDuration(int divider) {
  return (divider > 0) ? (1.0 / divider) : (-1.5 / divider);
}

extern const int track_count;
