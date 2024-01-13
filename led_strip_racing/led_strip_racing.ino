#include <Adafruit_NeoPixel.h>
#include "music.hpp"
#include "constants.hpp"
#include "gravity.hpp"

// current speeds, positions and number of loops of all the players
float speeds[PLAYER_COUNT];
float dists[PLAYER_COUNT];
float loops[PLAYER_COUNT];

// Neopixel class for the led strip.
Adafruit_NeoPixel track = Adafruit_NeoPixel(PIXEL_COUNT, PIN_LED, NEO_GRB + NEO_KHZ800);

// The `millis()` function returns wrong values because `track.show()`
// disables interrupts, including the timer interrupts. So we roughly
// fix this by summing a constant amount of time per pixel, stored here.
long additionalMillis = 0;


// special halo effect when starting the race, can be removed
void aureola(uint32_t color) {
  if (PIXEL_COUNT == 900) {
    // specific to 900 pixels and to how the strip is set up
    for (int i=0; i<19; ++i) {
      track.setPixelColor(i+900-77-10, color == 0 ? 0 : RAINBOW[i]);
    }
  }
}

// special effects when starting the race, can be removed
void start_race_vx() {
  track.clear();

  // red light 
  track.setPixelColor(5, color(0, 0, 0));
  track.setPixelColor(6, color(0, 0, 0));
  track.setPixelColor(4, color(255, 0, 0));
  track.setPixelColor(3, color(255, 0, 0));
  aureola(color(255, 0, 0));
  track.show();
  myTone(400);
  delay(2000 / DEBUG_SPEED_SCALE);

  // yellow Light
  track.setPixelColor(8, color(0, 0, 0));
  track.setPixelColor(7, color(0, 0, 0));
  track.setPixelColor(6, color(255, 255, 0));
  track.setPixelColor(5, color(255, 255, 0));
  aureola(color(0, 0, 0));
  track.show();
  myTone(600);
  delay(2000 / DEBUG_SPEED_SCALE);

  // green Light
  track.setPixelColor(8, color(0, 255, 0));
  track.setPixelColor(7, color(0, 255, 0));
  aureola(color(255, 0, 0));
  track.show();
  myTone(1200);
  delay(2000 / DEBUG_SPEED_SCALE);

  // turn off audio at the end
  myNoTone();
}

void start_race() {
  // visual effects for when the race starts, can be removed if unwanted
  start_race_vx();

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
  reproduceMusicProgressive(0.0, 0);
}

void visualizeGravity() {
  // print the gravity array out to serial 
  Serial.begin(2000000);
  Serial.println();
  Serial.println();
  for(int i=0; i<PIXEL_COUNT; ++i) {
    Serial.print(gravityMap(i));
    Serial.print(" ");
    if (i%45 == 44) {
      Serial.println();
    }
  }
  Serial.println();

  // display the gravity on the led strip
  for (int i = 0; i < PIXEL_COUNT; i++) {
    int gm = gravityMap(i);
    track.setPixelColor(i, color(0, gm > 127 ? (gm-128) : 0, gm <= 127 ? (127-gm) : 0));
  }
  for (int i = 0; i < PIXEL_COUNT; i+=10) {
    track.setPixelColor(i, color(255,255,255));
  }
  track.show();

  while(1);
}

void setup() {
  Serial2.begin(115200);
  pinMode(PIN_FINAL_LIGHTS, OUTPUT);
  track.begin();
  
  // uncomment to viusalize the leds with gravity not equal to 127
  // visualizeGravity();

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
  reproduceMusicProgressive(best / (LOOP_COUNT * PIXEL_COUNT), millis() + additionalMillis);
}

void draw_car(int index) {
  track.setPixelColor(((word)dists[index] % PIXEL_COUNT), COLORS[index]);
  track.setPixelColor(((word)(dists[index]+1) % PIXEL_COUNT), COLORS[index]);
}

void loop() {
  // redraw the led strip from scratch
  track.clear();

  // play music progressively to the position of the first player
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

    speeds[i] += GRAVITY * (gravityMap((word)dists[i] % PIXEL_COUNT) - 127);
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
  
  // show the updated pixels on the led strip
  track.show();

  // track.show() sends 24 bits for each pixel, and the bitrate is roughly 800kHz,
  // so this is the number of milliseconds during which interrupts were disabled
  additionalMillis += PIXEL_COUNT * 24 / 800;
}
