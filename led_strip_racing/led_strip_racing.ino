#include <Adafruit_NeoPixel.h>


/// set this to a value higher than one to make all interactions faster, for debugging purposes.
constexpr int DEBUG_SPEED_SCALE = 1;
/// Led strip pin. A 500 <= R <= 1K in series should be used.
constexpr int PIN_LED = 3;
/// An additional LED strip or some other lights that turn on after the game finishes.
constexpr int PIN_FINAL_LIGHTS = 2;

/// Max number of players. Don't exceed 8.
constexpr int PLAYER_COUNT = 2;
/// Led in the strip. A 5m 60led/m is the best :).
constexpr int PIXEL_COUNT = 900 / DEBUG_SPEED_SCALE;
/// Total laps race.
constexpr int LOOP_COUNT = 1;

/// Acceleration value. When a player pushs the button, his speed increases
/// with this value. Otherwise his speed decreases. 
/// An high value means more speed and power :)
constexpr float ACCELERATION = 0.6;// 0.15;//0.2;
/// Simulate the friction between asphalt and car's "wheels". Car speed 
/// decreases every time with this value.
constexpr float FRICTION = 0.05;//0.012;
/// Weight force simulation. Used in loop and ramp "gravity" simulation.
constexpr float GRAVITY = -0.00005;//0.003;

// copied from Adafruit_NeoPixel, but made `constexpr`
constexpr uint32_t color(uint8_t r, uint8_t g, uint8_t b) {
  return ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;
}

/// Player 1 color. BLUE
constexpr uint32_t COLOR1 = color(0, 0, 255);
/// Player 2 color. GREEN
constexpr uint32_t COLOR2 = color(0, 255, 0);
/// Player 3 color. RED
constexpr uint32_t COLOR3 = color(255, 0, 0);
/// Player 4 color. YELLOW
constexpr uint32_t COLOR4 = color(255, 255, 0);
/// Player 5 color. PURPLE
constexpr uint32_t COLOR5 = color(178, 102, 255);
/// Player 6 color. PINK
constexpr uint32_t COLOR6 = color(204, 0, 102);
/// Player 7 color. ORANGE
constexpr uint32_t COLOR7 = color(204, 102, 0);
/// Player 8 color. LIGHT BLUE
constexpr uint32_t COLOR8 = color(153, 255, 255);

/// 127 is the normal gravity. Values under this value means there is a hill,
/// while values above 127 means there is a downhill.
byte gravity_map[PIXEL_COUNT];

// current speeds, positions and number of loops of all the players
float speeds[PLAYER_COUNT];
float dists[PLAYER_COUNT];
float loops[PLAYER_COUNT];

/// Neopixel class for the led strip.
Adafruit_NeoPixel track = Adafruit_NeoPixel(PIXEL_COUNT, PIN_LED, NEO_GRB + NEO_KHZ800);


void my_tone(uint16_t val){
  char *test = (char *)&val;
  Serial2.write(255);
  Serial2.write(test[1]);
  Serial2.write(test[0]);
}


void set_gravity_range(int start, int end, byte value) {
  for (int i=start; i<=end; ++i) {
    gravity_map[i] = value;
  }
}

uint32_t colors[4]={COLOR1, COLOR2, COLOR3, COLOR4};
void draw_car(int index) {
  track.setPixelColor(((word)dists[index] % PIXEL_COUNT), colors[index]);
  track.setPixelColor(((word)(dists[index]+1) % PIXEL_COUNT), colors[index]);
}


constexpr uint32_t RAINBOW[] = {
  color(255, 0, 0),
  color(255, 53, 0),
  color(255, 107, 0),
  color(255, 161, 0),
  color(255, 214, 0),
  color(241, 255, 0),
  color(187, 255, 0),
  color(134, 255, 0),
  color(80, 255, 0),
  color(26, 255, 0),
  color(0, 255, 26),
  color(0, 255, 80),
  color(0, 255, 134),
  color(0, 255, 187),
  color(0, 255, 241),
  color(0, 214, 255),
  color(0, 161, 255),
  color(0, 107, 255),
  color(0, 53, 255)
};

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
  my_tone(400);
  delay(2000 / DEBUG_SPEED_SCALE);

  // Yellow Light
  track.setPixelColor(8, color(0, 0, 0));
  track.setPixelColor(7, color(0, 0, 0));
  track.setPixelColor(6, color(255, 255, 0));
  track.setPixelColor(5, color(255, 255, 0));
  aureola(color(0, 0, 0));
  track.show();
  my_tone(600);
  delay(2000 / DEBUG_SPEED_SCALE);

  // Green Light
  track.setPixelColor(8, color(0, 255, 0));
  track.setPixelColor(7, color(0, 255, 0));
  aureola(color(255, 0, 0));
  track.show();
  my_tone(1200);
  delay(2000 / DEBUG_SPEED_SCALE);

  // turn off audio at the end
  my_tone(1);

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
}

/* 
  Star Wars theme  
  Connect a piezo buzzer or speaker to pin 11 or select a new pin.
  More songs available at https://github.com/robsoncouto/arduino-songs                                            
                                              
                                              Robson Couto, 2019
*/
#define NOTE_B0  31
#define NOTE_C1  33
#define NOTE_CS1 35
#define NOTE_D1  37
#define NOTE_DS1 39
#define NOTE_E1  41
#define NOTE_F1  44
#define NOTE_FS1 46
#define NOTE_G1  49
#define NOTE_GS1 52
#define NOTE_A1  55
#define NOTE_AS1 58
#define NOTE_B1  62
#define NOTE_C2  65
#define NOTE_CS2 69
#define NOTE_D2  73
#define NOTE_DS2 78
#define NOTE_E2  82
#define NOTE_F2  87
#define NOTE_FS2 93
#define NOTE_G2  98
#define NOTE_GS2 104
#define NOTE_A2  110
#define NOTE_AS2 117
#define NOTE_B2  123
#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_FS3 185
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_FS6 1480
#define NOTE_G6  1568
#define NOTE_GS6 1661
#define NOTE_A6  1760
#define NOTE_AS6 1865
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
#define NOTE_DS7 2489
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_FS7 2960
#define NOTE_G7  3136
#define NOTE_GS7 3322
#define NOTE_A7  3520
#define NOTE_AS7 3729
#define NOTE_B7  3951
#define NOTE_C8  4186
#define NOTE_CS8 4435
#define NOTE_D8  4699
#define NOTE_DS8 4978
#define REST      0


// change this to make the song slower or faster
int tempo = 140 * DEBUG_SPEED_SCALE;

// notes of the moledy followed by the duration.
// a 4 means a quarter note, 8 an eighteenth , 16 sixteenth, so on
// !!negative numbers are used to represent dotted notes,
// so -4 means a dotted quarter note, that is, a quarter plus an eighteenth!!
int melody[] = {

  // We Wish You a Merry Christmas
  // Score available at https://musescore.com/user/6208766/scores/1497501
  
  NOTE_C5,4, //1
  NOTE_F5,4, NOTE_F5,8, NOTE_G5,8, NOTE_F5,8, NOTE_E5,8,
  NOTE_D5,4, NOTE_D5,4, NOTE_D5,4,
  NOTE_G5,4, NOTE_G5,8, NOTE_A5,8, NOTE_G5,8, NOTE_F5,8,
  NOTE_E5,4, NOTE_C5,4, NOTE_C5,4,
  NOTE_A5,4, NOTE_A5,8, NOTE_AS5,8, NOTE_A5,8, NOTE_G5,8,
  NOTE_F5,4, NOTE_D5,4, NOTE_C5,8, NOTE_C5,8,
  NOTE_D5,4, NOTE_G5,4, NOTE_E5,4,
  NOTE_F5,2,
  
  NOTE_C5,4,

  NOTE_F5,4, NOTE_F5,4, NOTE_F5,4,//17
  NOTE_E5,2, NOTE_E5,4,
  NOTE_F5,4, NOTE_E5,4, NOTE_D5,4,
  NOTE_C5,2, NOTE_A5,4,
  NOTE_AS5,4, NOTE_A5,4, NOTE_G5,4,
  NOTE_C6,4, NOTE_C5,4, NOTE_C5,8, NOTE_C5,8,
  NOTE_D5,4, NOTE_G5,4, NOTE_E5,4,
  NOTE_F5,2,
 
};

// sizeof gives the number of bytes, each int value is composed of two bytes (16 bits)
// there are two values per note (pitch and duration), so for each note there are four bytes
int notes = sizeof(melody) / sizeof(melody[0]) / 2;

// this calculates the duration of a whole note in ms
int wholenote = (60000 * 4) / tempo;

int divider = 0, noteDuration = 0;
double bpm_length;
double reproduce_until;
int music_index=0;
unsigned long last_update=0;
void reproduce_progressive(double pos){
  if(millis()-last_update>1000){
    my_tone(1);
  }

  if (pos<2) {
    music_index = 0;
    reproduce_until = 0.0;
    last_update = millis();

  } else if (pos>=2 && pos>reproduce_until) {
    last_update = millis();
    music_index += 1;

    if(music_index%2 == 1) {
      if(melody[music_index]>0) {
        reproduce_until += (bpm_length/melody[music_index+1])*0.1;
      } else {
        reproduce_until += (bpm_length/-melody[music_index+1])*0.15;
      }
      my_tone(1);

    } else {
      if(melody[music_index+1]>0) {
        reproduce_until += (bpm_length/melody[music_index+1])*0.9;
      } else {
        reproduce_until += (bpm_length/-melody[music_index+1])*1.35;
      }
      my_tone(melody[music_index]);
    }
  }
}


void setup() {
  Serial2.begin(115200);

  pinMode(PIN_FINAL_LIGHTS, OUTPUT);
    
  double whole_bpm=0.0;
  for (int i = 0; i < notes; i++) {
    if (melody[i*2+1] < 0)
      whole_bpm -= 1.5/melody[i*2+1];
    else
      whole_bpm += 1.0/melody[i*2+1];
  }
  bpm_length=LOOP_COUNT*PIXEL_COUNT/whole_bpm;

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

  loops[0] = 0;
  loops[1] = 0;
  loops[2] = 0;
  loops[3] = 0;
  
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

void reproduce_music() {
  // iterate over the notes of the melody. 
  // Remember, the array is twice the number of notes (notes + durations)
  for (int thisNote = 0; thisNote < notes * 2; thisNote = thisNote + 2) {

    // calculates the duration of each note
    divider = melody[thisNote + 1];
    if (divider > 0) {
      // regular note, just proceed
      noteDuration = (wholenote) / divider;
    } else if (divider < 0) {
      // dotted notes are represented with negative durations!!
      noteDuration = (wholenote) / abs(divider);
      noteDuration *= 1.5; // increases the duration in half for dotted notes
    }

    // we only play the note for 90% of the duration, leaving 10% as a pause
    my_tone(melody[thisNote]);
    delay(noteDuration*0.9);
    my_tone(1);
    delay(0.1);
    
    // stop the waveform generation before the next note.
    my_tone(1);
  }
}


void winner_fx() {
  digitalWrite(PIN_FINAL_LIGHTS, HIGH);
  reproduce_music();
  digitalWrite(PIN_FINAL_LIGHTS, LOW);
}

void loop()
{
  // redraw the track from start
  track.clear();

  double best=0.0;
  for(int i=0; i<PLAYER_COUNT; i++){
    if(best<dists[i]){
      best=dists[i];
    }
  }
  reproduce_progressive(best);

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
        for (int j = 0; j < PIXEL_COUNT; j += 10) {
          track.setPixelColor(j, colors[i]);
        }
        track.show();
        winner_fx();

        start_race();
        return;
      }
    }
  }

  // draw all cars
  for (int j = 0; j < PLAYER_COUNT; j++) {
    draw_car(j);
  }
  
  track.show();
}
