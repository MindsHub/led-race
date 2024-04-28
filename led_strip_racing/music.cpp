#include "music.hpp"


void myTone(uint16_t frequency) {
  // pass the frequency to play to the other Arduino
  char *ptr = (char *) &frequency;
  Serial2.write(255);
  Serial2.write(ptr[1]);
  Serial2.write(ptr[0]);
}

inline void myNoTone() {
  // the value 1 is interpreted by the other Arduino as noTone
  myTone(1);
}


void reproduceMusic(int track_index, int tempo) {
  // this calculates the duration of a whole note in ms
  int wholenote = (60000 * 4) / tempo;

  // iterate over the notes of the melody.
  // Remember, the array is twice the number of notes (notes + durations)
  for (int thisNote = 0; thisNote < melodyCount * 2; thisNote = thisNote + 2) {
    double noteDuration = wholenote * dividerToDuration(getNote(track_index, thisNote + 1));

    // we only play the note for 90% of the duration, leaving 10% as a pause
    myTone(getNote(track_index, thisNote));
    delay(noteDuration*0.9);
    myNoTone();
    delay(noteDuration*0.1);

    // stop the waveform generation before the next note.
    myNoTone();
  }
}

// millis() is wrong when interrupts are sometimes disabled,
// so ask the caller to pass timeMillis instead
void reproduceMusicProgressive(int track_index, double pos, unsigned long timeMillis) {
  static int melodyIndex = 0;
  static unsigned long lastUpdate = 0;
  static double reproduceUntilPos = 0.0;

  // both pos and reproduceUntilPos range between 0.0 and 1.0
  if (pos <= 0.0 || pos >= 1.0) {
    melodyIndex = 0;
    lastUpdate = timeMillis;
    reproduceUntilPos = 0.0;
    myNoTone();

  } else if (pos > reproduceUntilPos) {
    melodyIndex += 1;
    lastUpdate = timeMillis;

    // make sure we're not reading past the melody array size
    if (melodyIndex+1 < melodyCount*2) {
      if (melodyIndex % 2 == 1) {
        reproduceUntilPos += dividerToDuration(getNote(track_index, melodyIndex)) / melodyDuration * 0.1;
        myNoTone();

      } else {
        reproduceUntilPos += dividerToDuration(getNote(track_index, melodyIndex+1)) / melodyDuration * 0.9;
        myTone(getNote(track_index, melodyIndex));
      }
    }

  } else if (timeMillis - lastUpdate > 2000) {
    // if the position didn't change in the last 2 seconds, we can assume the game is not
    // being played anymore, so the sound is just annoying, let's stop it
    myNoTone();
    lastUpdate = timeMillis;
  }
}
