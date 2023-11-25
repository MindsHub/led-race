#include <Adafruit_NeoPixel.h>
#include "music.h"
#include "constants.h"

/// 127 is the normal gravity. Values under this value means there is a hill,
/// while values above 127 means there is a downhill.
byte gravity_map[PIXEL_COUNT];

// current speeds, positions and number of loops of all the players
float speeds[PLAYER_COUNT];
float dists[PLAYER_COUNT];
float loops[PLAYER_COUNT];

/// Neopixel class for the led strip.
Adafruit_NeoPixel track = Adafruit_NeoPixel(PIXEL_COUNT, PIN_LED, NEO_GRB + NEO_KHZ800);


void set_gravity_range(int start, int end, byte value) {
  for (int i=start; i<=end; ++i) {
    gravity_map[i] = value;
  }
}

void draw_car(int index) {
  track.setPixelColor(((word)dists[index] % PIXEL_COUNT), COLORS[index]);
  track.setPixelColor(((word)(dists[index]+1) % PIXEL_COUNT), COLORS[index]);
}

void aureola(uint32_t color) {
  if (PIXEL_COUNT == 900) {
    for (int i=0; i<19; ++i) {
      track.setPixelColor(i+900-77-10, color == 0 ? 0 : RAINBOW[i]);
    }
  }
}


void start_race() {
  track.clear();
  track.show();
  delay(2000 / DEBUG_SPEED_SCALE);
  // Start the Semaphore. TODO: Add start button in v1.0.1

  // Red light 
  track.setPixelColor(5, color(0, 0, 0));
  track.setPixelColor(6, color(0, 0, 0));
  track.setPixelColor(4, color(255, 0, 0));
  track.setPixelColor(3, color(255, 0, 0));
  aureola(color(255, 0, 0));
  track.show();
  myTone(400);
  delay(2000 / DEBUG_SPEED_SCALE);

  // Yellow Light
  track.setPixelColor(8, color(0, 0, 0));
  track.setPixelColor(7, color(0, 0, 0));
  track.setPixelColor(6, color(255, 255, 0));
  track.setPixelColor(5, color(255, 255, 0));
  aureola(color(0, 0, 0));
  track.show();
  myTone(600);
  delay(2000 / DEBUG_SPEED_SCALE);

  // Green Light
  track.setPixelColor(8, color(0, 255, 0));
  track.setPixelColor(7, color(0, 255, 0));
  aureola(color(255, 0, 0));
  track.show();
  myTone(1200);
  delay(2000 / DEBUG_SPEED_SCALE);

  // turn off audio at the end
  myNoTone();

  // discard any button press happened in the meantime
  while (Serial2.available()) {
    Serial2.read();
  }

  // start afresh
  for (int k = 0; k < PLAYER_COUNT; k++) {
    loops[k] = 0;
    dists[k] = 0;
    speeds[k] = 0;
  }

  // reset progressive music
  reproduceMusicProgressive(0.0);
}


void setup() {
  Serial2.begin(115200);

  pinMode(PIN_FINAL_LIGHTS, OUTPUT);

  for (int i = 0; i < PIXEL_COUNT; i++) {
    gravity_map[i] = 127;
  };
  set_gravity_range(51, 64, 255);
  set_gravity_range(90, 110, 255);
  set_gravity_range(110, 120, 0);
  set_gravity_range(158, 175, 0);
  set_gravity_range(188, 209, 255);
  set_gravity_range(240, 274, 255);
  set_gravity_range(275, 295, 100);
  set_gravity_range(299, 315, 255);
  set_gravity_range(362, 370, 0);
  set_gravity_range(382, 392, 0);
  set_gravity_range(404, 413, 0);
  set_gravity_range(430, 437, 0);
  set_gravity_range(460, 498, 255);
  set_gravity_range(555, 590, 0);
  set_gravity_range(602, 621, 0);
  set_gravity_range(625, 631, 220);
  set_gravity_range(636, 650, 100);
  set_gravity_range(653, 700, 255);
  set_gravity_range(705, 761, 0);
  set_gravity_range(768, 820, 255);
  set_gravity_range(824, 890, 0);
  track.begin();
  
  // Uncomment to viusalize the leds with gravity not equal to 127
  /*for (int i = 0; i < PIXEL_COUNT; i++) {
    int gm = gravity_map[i];
    track.setPixelColor(i, color(0, gm > 127 ? (gm-128) : 0, gm <= 127 ? (127-gm) : 0));
  }
  for (int i = 0; i < PIXEL_COUNT; i+=10) {
    track.setPixelColor(i, color(255,255,255));
  }
  track.show();
  while(1);*/

  start_race();
}


void onPlayerWon(int i) {
  for (int j = 0; j < PIXEL_COUNT; j += 10) {
    track.setPixelColor(j, COLORS[i]);
  }
  track.show();
  
  digitalWrite(PIN_FINAL_LIGHTS, HIGH);
  reproduceMusic(140 / DEBUG_SPEED_SCALE);
  digitalWrite(PIN_FINAL_LIGHTS, LOW);

  start_race();
}

void handleProgressiveMusic() {
  double best = 0.0;
  for (int i=0; i<PLAYER_COUNT; i++) {
    if (best < dists[i]) {
      best = dists[i];
    }
  }
  reproduceMusicProgressive(best / (LOOP_COUNT * PIXEL_COUNT));
}

void loop() {
  // redraw the track from start
  track.clear();

  // play music progressively to the position of the track
  handleProgressiveMusic();

  // check for button presses from the other Arduino
  bool pressed[PLAYER_COUNT];
  for (int i=0; i<PLAYER_COUNT; i++) {
    pressed[i] = false;
  }
  while (Serial2.available()) {
    int z = Serial2.read();
    if (z > 0 && z <= PLAYER_COUNT)
      pressed[z-1] = true;
  }

  // update speeds, distances and loop count for all players
  for (int i = 0; i < PLAYER_COUNT; i++) {
    if(pressed[i]) {
      speeds[i] += ACCELERATION;
    }

    if ((gravity_map[(word)dists[i] % PIXEL_COUNT]) < 127)
      speeds[i] -= GRAVITY * (127 - (gravity_map[(word)dists[i] % PIXEL_COUNT]));
    if ((gravity_map[(word)dists[i] % PIXEL_COUNT]) > 127)
      speeds[i] += GRAVITY * ((gravity_map[(word)dists[i] % PIXEL_COUNT]) - 127);

    speeds[i] -= speeds[i] * FRICTION;
    dists[i] += speeds[i];

    if (dists[i] > PIXEL_COUNT * loops[i]) {
      loops[i]++;
      if(loops[i] > LOOP_COUNT) {
        onPlayerWon(i);
        return; // restart loop() from the beginning
      }
    }
  }

  // draw all cars
  for (int j = 0; j < PLAYER_COUNT; j++) {
    draw_car(j);
  }
  
  track.show();
}
