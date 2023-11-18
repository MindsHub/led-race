constexpr int PIN_AUDIO = 9;

constexpr int PIN_COUNT = 4;
constexpr int PINS[PIN_COUNT] {4,5,6,7};

unsigned long lastVal[PIN_COUNT];
unsigned long lastTime[PIN_COUNT];
bool thebyte = 0;
uint16_t v = 0;

void setup() {
  Serial.begin(115200);
  pinMode(PIN_AUDIO, OUTPUT);
  for (int i=0;i<PIN_COUNT;++i){
    pinMode(PINS[i], INPUT_PULLUP);
    lastVal[i] = LOW;
    lastTime[i] = 0;
  }
}

void loop() {
  if (Serial.available()) {
    int vr = Serial.read();
    if (vr == 0xff) {
      thebyte = false;
    } else if (vr >= 0) {
      thebyte = !thebyte;
      if (thebyte) {
        v = vr;
      } else {
        v <<= 8;
        v |= vr;
        Serial.println(v);
        if (v == 1) {
          noTone(PIN_AUDIO);
        } else if (v != 0 && v != 0xffff) {
          tone(PIN_AUDIO, v);
        }
      }
    }
  }

  for (int i=0;i<PIN_COUNT;++i){
    bool val = digitalRead(PINS[i]);
    if (!val) {
      if (lastVal[i] && millis()-lastTime[i]>10) {
        Serial.write((char)(i+1));
      }
      lastTime[i] = millis();
    }
    lastVal[i] = val;
  }
}
