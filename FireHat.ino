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

#include "audio.h"
#include "button.h"
#include "fire.h"
#include "soundReactiveDot.h"

// Memory for fire animations
FireData* rimFireDataLeft;
FireData* rimFireDataRight;
FireData* topFireDataLeft;
FireData* topFireDataRight;
#define MS_TO_COOL 750

SoundReactiveDotData* rimDotDataLeft;
SoundReactiveDotData* rimDotDataRight;
SoundReactiveDotData* topDotDataLeft;
SoundReactiveDotData* topDotDataRight;

unsigned long lastFrameAt;

void setup() {
  FastLED.setMaxPowerInVoltsAndMilliamps(3, 500);
  delay( 2000 ); // power-up safety delay

  buttonSetup();

  // Set up LEDs
  FastLED.addLeds<WS2812B, RIM_LED_PIN, COLOR_ORDER>(rimLeds, RIM_LED_COUNT).setCorrection( TypicalLEDStrip );
  FastLED.addLeds<WS2812B, TOP_LED_PIN, COLOR_ORDER>(topLeds, TOP_LED_COUNT).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness( BRIGHTNESS );

  rimFireDataLeft = fireSetup(RIM_LED_COUNT / 2, 0.5 * MS_TO_COOL, 1);
  rimFireDataRight = fireSetup(RIM_LED_COUNT / 2, 0.5 * MS_TO_COOL, 1);
  topFireDataLeft = fireSetup(TOP_LED_COUNT / 2, 0.3 * MS_TO_COOL, 1);
  topFireDataRight = fireSetup(TOP_LED_COUNT / 2, 0.3 * MS_TO_COOL, 1);

  rimDotDataLeft = soundReactiveDotSetup();
  rimDotDataRight = soundReactiveDotSetup();
  topDotDataLeft = soundReactiveDotSetup();
  topDotDataRight = soundReactiveDotSetup();

  lastFrameAt = millis();
}

#define ANIMATION_COUNT 5

#define FIRE_ANIMATION 0
#define AUDIO_FIRE_ANIMATION 1
#define WHITE_ANIMATION 2
#define BLACK_ANIMATION 3
#define SOUND_REACTIVE_DOT_ANIMATION 4

int currentAnimation = SOUND_REACTIVE_DOT_ANIMATION;

void loop() {
  unsigned long currentTime = millis();
  unsigned long timeElapsed = currentTime - lastFrameAt;
  lastFrameAt = currentTime;

  if (readButtonState()) {
    currentAnimation = (currentAnimation + 1) % ANIMATION_COUNT;
  }

  switch (currentAnimation) {
  case AUDIO_FIRE_ANIMATION:
    audioUpdate();
    fire(rimLeds, rimFireDataLeft, timeElapsed, audioLevel(), 62, RIM_LED_COUNT, 1);
    fire(rimLeds, rimFireDataRight, timeElapsed, audioLevel(), 61, RIM_LED_COUNT, -1);
    fire(topLeds, topFireDataLeft, timeElapsed, audioLevel(), 0, TOP_LED_COUNT, 1);
    fire(topLeds, topFireDataRight, timeElapsed, audioLevel(), TOP_LED_COUNT - 1, TOP_LED_COUNT, -1);
    break;

  case FIRE_ANIMATION:
    randomFire(rimLeds, rimFireDataLeft, timeElapsed, 62, RIM_LED_COUNT, 1);
    randomFire(rimLeds, rimFireDataRight, timeElapsed, 61, RIM_LED_COUNT, -1);
    randomFire(topLeds, topFireDataLeft, timeElapsed, 0, TOP_LED_COUNT, 1);
    randomFire(topLeds, topFireDataRight, timeElapsed, TOP_LED_COUNT - 1, TOP_LED_COUNT, -1);
    break;

  case WHITE_ANIMATION:
    for(int i = 0; i < RIM_LED_COUNT; i++) {
      rimLeds[i] = CRGB::White;
    }
    for(int i = 0; i < TOP_LED_COUNT; i++) {
      topLeds[i] = CRGB::White;
    }
    break;

  case BLACK_ANIMATION:
    for(int i = 0; i < RIM_LED_COUNT; i++) {
      rimLeds[i] = CRGB::Black;
    }
    for(int i = 0; i < TOP_LED_COUNT; i++) {
      topLeds[i] = CRGB::Black;
    }
    break;

  case SOUND_REACTIVE_DOT_ANIMATION:
    audioUpdate();
    soundReactiveDot(rimDotDataLeft, rimLeds, RIM_LED_COUNT / 2, timeElapsed, audioLevel(), 62, RIM_LED_COUNT, 1);
    soundReactiveDot(rimDotDataRight, rimLeds, RIM_LED_COUNT / 2, timeElapsed, audioLevel(), 61, RIM_LED_COUNT, -1);
    soundReactiveDot(topDotDataLeft, topLeds, TOP_LED_COUNT / 2, timeElapsed, audioLevel(), 0, TOP_LED_COUNT, 1);
    soundReactiveDot(topDotDataRight, topLeds, TOP_LED_COUNT / 2, timeElapsed, audioLevel(), TOP_LED_COUNT - 1, TOP_LED_COUNT, -1);
    break;
  }

  FastLED.show();
}
