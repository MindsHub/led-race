/**
 * @file led_strip_race.ino
 * @author Christian Corsetti (c.corsetti.01@gmail.com)
 * @brief Funny led race with Arduino and WS2812b led strip. Made in Fablab Valsamoggia.
 * @version 0.0.0
 * @date 29-04-2019
 * 
 * @copyright MIT License
 * Copyright (c) [year] [fullname]
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:

 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 * 
 * @note CHANGES
 * @parblock
 *  29-04-2019 - Christian Corsetti:
 *      - Add multiple players.
 * 
 *  01-05-2019 - Christian Corsetti:
 *      - Bug fix: remove third "ghost" player when there are only two players.
 * 
 *  03-05-2019 - Christian Corsetti:
 *      - Add comments and clean code.
 *  27-05-2019 - Christian Corsetti:
 *      - Remove loop and ramps, reduce ACEL param.
 *      - Bug fix on loop create function: missing k.
 * **FROZEN HERE V0.0.0**
 * @endparblock
 */
/*----------------------------------------------------------------------------*/
/*                                   INCLUDE                                  */
/*----------------------------------------------------------------------------*/
#include <Adafruit_NeoPixel.h>
/*----------------------------------------------------------------------------*/
/*                                   DEFINE                                   */
/*----------------------------------------------------------------------------*/

#define DEBUG_SPEED_SCALE 1
/// Led strip pin. A 500 <= R <= 1K in series should be used.
#define PIN_LED A0
/// Player 1 Pin switch. Internal pull-up; place the switch between PIN and GND.
#define PIN_P1 18
/// Player 1 Pin switch. Internal pull-up; place the switch between PIN and GND.
#define PIN_P2 19
/// Player 1 Pin switch. Internal pull-up; place the switch between PIN and GND.
#define PIN_P3 20
/// Player 1 Pin switch. Internal pull-up; place the switch between PIN and GND.
#define PIN_P4 21
/// Speaker audio output. a C = 2.2uF should be placed in series between
/// PIN and 8 Ohm speaker.
#define PIN_LED_GRAANDI 2
#define PIN_AUDIO 3 
/// Max number of players. Don't exceed 8.
#define MAX_PLAYERS 4
/// Led in the strip. A 5m 60led/m is the best :).
#define NPIXELS (900 / DEBUG_SPEED_SCALE)
/// Player 1 color. RED
#define COLOR1 track.Color(255, 0, 0)
/// Player 2 color. GREEN
#define COLOR2 track.Color(0, 255, 0)
/// Player 3 color. BLUE
#define COLOR3 track.Color(0, 0, 255)
/// Player 4 color. YELLOW
#define COLOR4 track.Color(255, 255, 0)
/// Player 5 color. PURPLE
#define COLOR5 track.Color(178, 102, 255)
/// Player 6 color. PINK
#define COLOR6 track.Color(204, 0, 102)
/// Player 7 color. ORANGE
#define COLOR7 track.Color(204, 102, 0)
/// Player 8 color. LIGHT BLUE
#define COLOR8 track.Color(153, 255, 255)

/// 127 is the normal gravity. Values under this value means there is a hill,
/// while values above 127 means there is a downhill.
byte gravity_map[NPIXELS];
/// Lenght of start beep.
int TBEEP = 3;
/// It's pretty self-explanatory.
float speeds[MAX_PLAYERS];
/// It's pretty self-explanatory.
float dists[MAX_PLAYERS];
/// It's pretty self-explanatory.
float loops[MAX_PLAYERS];
/// It's pretty self-explanatory.
float flag_sws[MAX_PLAYERS];
/// It's pretty self-explanatory.
int PIN[MAX_PLAYERS];
/// It's pretty self-explanatory.
uint32_t COLORS[MAX_PLAYERS];
/// Total laps race.
byte loop_max = 1;
/// Acceleration value. When a player pushs the button, his speed increases
/// with this value. Otherwise his speed decreases. 
/// An high value means more speed and power :)
float ACEL = 0.6;// 0.15;//0.2;
/// Simulate the friction between asphalt and car's "wheels". Car speed 
/// decreases every time with this value.
float friction = 0.05;//0.012;
/// Weight force simulation. Used in loop and ramp "gravity" simulation.
float g_force = 0.004;//0.003; 
/// Neopixel class for the led strip.
Adafruit_NeoPixel track = Adafruit_NeoPixel(NPIXELS, PIN_LED, NEO_GRB + NEO_KHZ800);
/// It's pretty self-explanatory. (in milliseconds).
int tdelay = 5;
//==============================================================================
/**
 * @brief  Set the gravity array for a ramp.
 * @note   
 * @param  ramp_high: high of the ramp; set the "steep".
 * @param  ramp_startl: start led strip for the hill.
 * @param  ramp_centerl: center led of the ramp.
 * @param  ramp_endl: last led of the ramp.
 * @retval None
 */
void my_tone(uint16_t val){
  
  char *test=(char *)&val;
  Serial2.write(255);
  Serial2.write(test[1]);
  Serial2.write(test[0]);
  Serial.write(255);
  Serial.write(test[1]);
  Serial.write(test[0]);
  //Serial.println(val);
}

void set_ramp(byte ramp_high, byte ramp_startl, byte ramp_centerl, byte ramp_endl)
{
    float k;

    k = (float)ramp_high / (ramp_centerl - ramp_startl);
    // Set the hill leds
    for (int i = 0; i < (ramp_centerl - ramp_startl); i++)
    {
        gravity_map[ramp_startl + i] = 127 - i * k;
    }
    gravity_map[ramp_centerl] = 127;

    k = (float)ramp_high / (ramp_endl - ramp_centerl);
    // Set downhill leds
    for (int i = 0; i < (ramp_endl - ramp_centerl); i++)
    {
        gravity_map[ramp_centerl + i + 1] = 127 + ramp_high - i * k;
    }
}
/**
 * @brief  Set the gravity array for a death loop.
 * @note   
 * @param  high: high of the ramp; set the "steep".
 * @param  start: start led strip for the hill.
 * @param  center: center led of the loop.
 * @param  end: last led of the loop.
 * @retval None
 */
void set_loop(byte high, byte start, byte center, byte end)
{
    float k;

    k = (float)high / (center - start);
   
    for (int i = 0; i < (center - start); i++)
    {
        gravity_map[start + i] = 127 - i * k;
    }
    gravity_map[center] = 255;

    k = (float)high / (end - center);
    for (int i = 0; i < (end - center); i++)
    {
        gravity_map[center + i + 1] = 127 + high - i * k;
    }
}
/**
 * @brief  Car one: color GREEN.
 * @note   the number of led increase with the laps.
 * @retval None
 */
void draw_car1(void)
{
    for (int i = 0; i <= loops[0]; i++)
    {
        track.setPixelColor(((word)dists[0] % NPIXELS) + i,
                            track.Color(0, 255 - i * 20, 0));
    }
}
/**
 * @brief  Car two: color RED.
 * @note   the number of led increase with the laps.
 * @retval None
 */
void draw_car2(void)
{
    for (int i = 0; i <= loops[1]; i++)
    {
        track.setPixelColor(((word)dists[1] % NPIXELS) + i,
                            track.Color(255 - i * 20, 0, 0));
    }
}
/**
 * @brief  Car two: color BLUE.
 * @note   the number of led increase with the laps.
 * @retval None
 */
void draw_car3(void)
{
    for (int i = 0; i <= loops[2]; i++)
    {
        track.setPixelColor(((word)dists[2] % NPIXELS) + i,
                            track.Color(0, 0, 255 - i * 20));
    }
}
/**
 * @brief  Car two: color YELLOW.
 * @note   the number of led increase with the laps.
 * @retval None
 */
void draw_car4(void)
{
    for (int i = 0; i <= loops[3]; i++)
    {
        track.setPixelColor(((word)dists[3] % NPIXELS) + i,
                            track.Color(255 - i * 20, 255 - i * 20, 0));
    }
}
/// Functions pointer array. Used for call draw_cars function with a loop. 
/// Increases the execution speed.
void (*draw_cars[MAX_PLAYERS])(void);



void start_race()
{
    for (int i = 0; i < NPIXELS; i++)
    {
        track.setPixelColor(i, track.Color(0, 0, 0));
    }
    track.show();
    delay(2000 / DEBUG_SPEED_SCALE);
    // Start the Semaphore. TODO: Add start button in v1.0.1
    // Red light 
    track.setPixelColor(5, track.Color(0, 0, 0));
    track.setPixelColor(6, track.Color(0, 0, 0));
    track.setPixelColor(4, track.Color(255, 0, 0));
    track.setPixelColor(3, track.Color(255, 0, 0));
    track.show();
    my_tone(400);
    delay(2000 / DEBUG_SPEED_SCALE);
    //my_tone(1);
    // Yellow Light
    track.setPixelColor(8, track.Color(0, 0, 0));
    track.setPixelColor(7, track.Color(0, 0, 0));
    track.setPixelColor(6, track.Color(255, 255, 0));
    track.setPixelColor(5, track.Color(255, 255, 0));
    track.show();
    my_tone(600);
    delay(2000 / DEBUG_SPEED_SCALE);
    //my_tone(1);
    // Green Light
    track.setPixelColor(8, track.Color(0, 255, 0));
    track.setPixelColor(7, track.Color(0, 255, 0));
    track.show();
    my_tone(1200);
    delay(2000 / DEBUG_SPEED_SCALE);
    my_tone(1);
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

  // Happy Birthday
  // Score available at https://musescore.com/user/8221/scores/26906

  NOTE_C4,4, NOTE_C4,8, 
  NOTE_D4,-4, NOTE_C4,-4, NOTE_F4,-4,
  NOTE_E4,-2, NOTE_C4,4, NOTE_C4,8, 
  NOTE_D4,-4, NOTE_C4,-4, NOTE_G4,-4,
  NOTE_F4,-2, NOTE_C4,4, NOTE_C4,8,

  NOTE_C5,-4, NOTE_A4,-4, NOTE_F4,-4, 
  NOTE_E4,-4, NOTE_D4,-4, NOTE_AS4,4, NOTE_AS4,8,
  NOTE_A4,-4, NOTE_F4,-4, NOTE_G4,-4,
  NOTE_F4,-2,
 
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
    /*Serial.print("here ");
    Serial.print(millis());
    Serial.print(" ");
    Serial.println(last_update);*/
    my_tone(1);
  }
  if(pos<2){
    music_index=0;
    reproduce_until=0.0;
    last_update=millis();
  }
  if(pos>=2&&pos>reproduce_until){
    last_update=millis();
    //tone(PIN_AUDIO, 400, 10);
    music_index+=1;
    if(music_index%2==1){
      //reproduce_until+=(bpm_length/melody[music_index])*0.1;
      if(melody[music_index]>0){
          reproduce_until+=(bpm_length/melody[music_index+1])*0.1;
        }else{
          reproduce_until+=(bpm_length/-melody[music_index+1])*0.15;
        }
      my_tone(1);
    }else{
      if(melody[music_index+1]>0){
          reproduce_until+=(bpm_length/melody[music_index+1])*0.9;
        }else{
          reproduce_until+=(bpm_length/-melody[music_index+1])*1.35;
        }
      //reproduce_until+=(bpm_length/melody[music_index+1])*0.9;
      my_tone(melody[music_index]);
    }
  }
}


void setup()
{
  Serial.begin(115200);
  Serial2.begin(115200);
  //Serial2.setTimeout(0);
  /*pinMode(5, INPUT_PULLUP);
  pinMode(6, INPUT_PULLUP);
  pinMode(7, INPUT_PULLUP);
  while(true){
    if(digitalRead(5)==LOW||digitalRead(6)==LOW||digitalRead(7)==LOW){
      my_tone(400);
      delay(10);
    }else{
      my_tone(1);
      delay(10);
    }
  }*/
  pinMode(PIN_P1, INPUT_PULLUP);
    pinMode(PIN_P2, INPUT_PULLUP);
    pinMode(PIN_P3, INPUT_PULLUP);
    pinMode(PIN_P4, INPUT_PULLUP);
    
  double whole_bpm=0.0;
  for(int i=0; i<notes; i++){
    if(melody[i*2+1]<0)
      whole_bpm-=1.5/melody[i*2+1];
    else
      whole_bpm+=1.0/melody[i*2+1];
  }
  bpm_length=loop_max*NPIXELS/whole_bpm;
  
  /*while (true){
    if (digitalRead(6)==HIGH){
      pinMode(PI)
    }
    //tone(PIN_AUDIO, 400, 100);
    winner_fx();
    //delay(10000);
  }*/
    for (int i = 0; i < NPIXELS; i++)
    {
        gravity_map[i] = 127;
    };
    track.begin();
    
    pinMode(PIN_AUDIO, OUTPUT);
    //set_ramp(28, 80, 101, 124);
    //set_ramp(26, 135, 158, 178);
    //set_loop(10, 20, 50, 80);
    //for (int i = 0; i < NPIXELS; i++)
    //{
      //  track.setPixelColor(i, track.Color(125, 0, (45 - gravity_map[i]) / 8)); //127
    //}

    PIN[0] = PIN_P1;
    PIN[1] = PIN_P2;
    PIN[2] = PIN_P3;
    PIN[3] = PIN_P4;

    COLORS[0] = COLOR2;
    COLORS[1] = COLOR1;
    COLORS[2] = COLOR3;
    COLORS[3] = COLOR5;

    loops[0] = 0;
    loops[1] = 0;
    loops[2] = 0;
    loops[3] = 0;

    draw_cars[0] = draw_car1;
    draw_cars[1] = draw_car2;
    draw_cars[2] = draw_car3;
    draw_cars[3] = draw_car4;

    //randomSeed(analogRead(1));
    
    track.show();
    //tone(PIN_AUDIO, 400);
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
    // Wait for the specief duration before playing the next note.
    
    
    // stop the waveform generation before the next note.
    my_tone(1);
  }
}


void winner_fx()
{
    pinMode(PIN_LED_GRAANDI, OUTPUT);
    digitalWrite(PIN_LED_GRAANDI, HIGH);
    reproduce_music();
    
    digitalWrite(PIN_LED_GRAANDI, LOW);
}
int count=0;
void loop()
{
  
    double best=0.0;
    for(int i=0; i<MAX_PLAYERS; i++){
      if(best<dists[i]){
        best=dists[i];
      }
    }
    reproduce_progressive(best);
    for(int i=0;i<NPIXELS;i++)
    {
        track.setPixelColor(i, track.Color(0,0,0));
    }
    // Color the ramp and loops
    // Viusalize the leds with gravity not equal to 127.
    //for (int i = 0; i < NPIXELS; i++)
    //{
      //  track.setPixelColor(i, track.Color(0, 0, (127 - gravity_map[i]) / 8));
    //}
    bool intrr[4];
    for(int i=0; i<4; i++){
      intrr[i]=false;
    }
    while(Serial2.available()){
      //Serial.print((char)Serial2.read());
      int z = Serial2.read();
      //Serial2.read();
      //Serial.println(z);
      if(z>0)
        intrr[z-1]=true;
    }
      for (int i = 0; i < MAX_PLAYERS; i++)
      {
      
        if((flag_sws[i] == 1) && (intrr[i]==false))//(digitalRead(PIN[i]) == 0))
        {
            flag_sws[i] = 0;
            speeds[i] += ACEL;
        }
        if ((flag_sws[i] == 0) && (intrr[i]==true))
        {
            flag_sws[i] = 1;
        }
        if ((gravity_map[(word)dists[i] % NPIXELS]) < 127)
            speeds[i] -= g_force * (127 - (gravity_map[(word)dists[i] % NPIXELS]));
        if ((gravity_map[(word)dists[i] % NPIXELS]) > 127)
            speeds[i] += g_force * ((gravity_map[(word)dists[i] % NPIXELS]) - 127);

        speeds[i] -= speeds[i] * friction;
        dists[i] += speeds[i];
    }
    for (int j = 0; j < MAX_PLAYERS; j++)
    {
        if (dists[j] > NPIXELS * loops[j])
        {
            loops[j]++;
            //tone(PIN_AUDIO, 600);
            TBEEP = 2;
        }
    }

    for (int j = 0; j < MAX_PLAYERS; j++)
    {
        if(loops[j] > loop_max)
        {
            for (int i = 0; i < NPIXELS; i++)
            {
              if(i%10==0){
                track.setPixelColor(i, COLORS[j]);
              }
            }
            track.show();
            winner_fx();
            for (int k = 0; k < MAX_PLAYERS; k++)
            {
                loops[k] = 0;
                dists[k] = 0;
                speeds[k] = 0;
            }
            start_race();
            break;
        }
    }
    // TODO: use a random function for player display.
    for (int j = 0; j < MAX_PLAYERS; j++)
    {
        (*draw_cars[j])();
    }
  count++;
  //if(count%3==0){
    track.show();
  /*}{
    delay(3);
    }*/
    
    //delay(tdelay);

    if (TBEEP > 0)
    {
        TBEEP -= 1;
        if (TBEEP == 0)
        {
            //noTone(PIN_AUDIO);
        }
    }
}

int display_lcd_laps()
{
    //to do
}

int display_lcd_time()
{
    //to do
}
