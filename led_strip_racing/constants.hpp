#pragma once

/// set this to a value higher than one to make all interactions faster, for debugging purposes.
constexpr int DEBUG_SPEED_SCALE = 1;
/// Led strip pin. A 500 <= R <= 1K in series should be used.
constexpr int PIN_LED = 3;
/// An additional LED strip or some other lights that turn on after the game finishes.
constexpr int PIN_FINAL_LIGHTS = 2;
/// The analog pin to read random seed
constexpr int RANDOM_SEED_PIN = A0;

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
/// All of the colors, in order.
constexpr uint32_t COLORS[] = {COLOR1, COLOR2, COLOR3, COLOR4, COLOR5, COLOR6, COLOR7, COLOR8};


/// A 19-color rainbow to draw as halo in some place of the led strip
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
  color(0, 53, 255),
};
