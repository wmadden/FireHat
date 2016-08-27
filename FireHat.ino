#include <FastLED.h>

#define COLOR_ORDER GRB

#define RIM_LED_PIN 12   // which pin your pixels are connected to
#define TOP_LED_PIN 6   // which pin your pixels are connected to

#define BRIGHTNESS 50   // 0-255, higher number is brighter.
#define SATURATION 255   // 0-255, 0 is pure white, 255 is fully saturated color

#define RIM_LED_COUNT 93 // Number of LEDs around rim of hat
#define TOP_LED_COUNT 36 // Number of LEDs on top of hat (i.e. around fan / goggles)
CRGB rimLeds[RIM_LED_COUNT];
CRGB topLeds[TOP_LED_COUNT];

// #include "audio.h"
#include "fire.h"

// Memory for fire animations
FireData* rimFireDataLeft;
FireData* rimFireDataRight;
FireData* topFireDataLeft;
FireData* topFireDataRight;
#define MS_TO_COOL 1000

unsigned long lastFrameAt;

void setup() {
  FastLED.setMaxPowerInVoltsAndMilliamps(3, 500);
  delay( 2000 ); // power-up safety delay

  // Set up LEDs
  FastLED.addLeds<WS2812B, RIM_LED_PIN, COLOR_ORDER>(rimLeds, RIM_LED_COUNT).setCorrection( TypicalLEDStrip );
  FastLED.addLeds<WS2812B, TOP_LED_PIN, COLOR_ORDER>(topLeds, TOP_LED_COUNT).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness( BRIGHTNESS );

  rimFireDataLeft = fireSetup(RIM_LED_COUNT / 2, MS_TO_COOL, 1);
  rimFireDataRight = fireSetup(RIM_LED_COUNT / 2, MS_TO_COOL, 1);
  topFireDataLeft = fireSetup(TOP_LED_COUNT / 2, MS_TO_COOL, 1);
  topFireDataRight = fireSetup(TOP_LED_COUNT / 2, MS_TO_COOL, 1);

  lastFrameAt = millis();
}

void loop() {
  unsigned long currentTime = millis();
  unsigned long timeElapsed = currentTime - lastFrameAt;
  lastFrameAt = currentTime;

  randomFire(rimLeds, rimFireDataLeft, timeElapsed, 62, RIM_LED_COUNT, 1);
  randomFire(rimLeds, rimFireDataRight, timeElapsed, 61, RIM_LED_COUNT, -1);
  randomFire(topLeds, topFireDataLeft, timeElapsed, 0, TOP_LED_COUNT, 1);
  randomFire(topLeds, topFireDataRight, timeElapsed, TOP_LED_COUNT, TOP_LED_COUNT, -1);

  FastLED.show();
}
