#include <FastLED.h>
#include "fire.h"

// Fire animation variables
const CRGBPalette16 FIRE_PALETTE = CRGBPalette16( CRGB::Black, CRGB::Red, CRGB::Yellow, CRGB::White);

#define MAX_PIXEL_HEAT 240

void injectLinearHeat(FireData* fireData, int heatToInject);

FireData* fireSetup(int ledArrayLength, int msToCool) {
  FireData* fireData = new FireData();
  fireData->pixelHeatArray = new float[ledArrayLength]();
  fireData->pixelHeatArrayLength = ledArrayLength;
  fireData->amountCooledPerMs = (float)255 / msToCool;

  // injectLinearHeat(fireData, 255 * ledArrayLength / 4);

  return fireData;
}

#define BRIGHTNESS 25.5 // Between 0-255

void applyHeatArrayToPixels(FireData* fireData, CRGB* ledArray) {
  float* pixelHeatArray = fireData->pixelHeatArray;
  int ledArrayLength = fireData->pixelHeatArrayLength;

  for(int i = 0; i < ledArrayLength; i++) {
    float colorindex = pixelHeatArray[i];
    CRGB color = ColorFromPalette(FIRE_PALETTE, colorindex);
    // color = color.nscale8(BRIGHTNESS);
    ledArray[i] = color;
  }
}

void coolPixels(FireData* fireData, unsigned long msElapsed) {
  float amountCooledSinceLastFrame = fireData->amountCooledPerMs * (float)msElapsed;

  for (int i = 0; i < fireData->pixelHeatArrayLength; i++) {
    float pixelHeat = fireData->pixelHeatArray[i];
    if (pixelHeat == 0) continue;

    if (pixelHeat - amountCooledSinceLastFrame < 0) {
      fireData->pixelHeatArray[i] = 0;
    } else {
      fireData->pixelHeatArray[i] = pixelHeat - amountCooledSinceLastFrame;
    }
  }
}

// Heat is injected at the bottom and flows upwards. Each pixel holds one byte (255) of
// heat, which overflows to the next pixel.
void injectLinearHeat(FireData* fireData, int heatToInject) {
  float* pixelHeatArray = fireData->pixelHeatArray;
  int pixelHeatArrayLength = fireData->pixelHeatArrayLength;

  for (int i = 0; i < pixelHeatArrayLength; i++) {
    int pixelHeatCapacity = MAX_PIXEL_HEAT - pixelHeatArray[i];

    if (pixelHeatCapacity > heatToInject) pixelHeatCapacity = heatToInject;

    pixelHeatArray[i] += pixelHeatCapacity;
    heatToInject -= pixelHeatCapacity;

    if (heatToInject == 0) break;
  }
}

// const int RISING_SPEED = 10; // pixels per second
// const float RISING_SPEED_PIXELS_PER_MS = RISING_SPEED / 1000.0;

void driftUpAndDiffuse(FireData* fireData) {
  float* pixels = fireData->pixelHeatArray;
  for (int i = fireData->pixelHeatArrayLength - 1; i >= 3; i--) {
    pixels[i] = (pixels[i] + pixels[i - 1] + pixels[i - 2]) / 3.0;
  }
}

// void spark(FireData* fireData, int height) {
//
// }

void randomFire(CRGB* ledArray, FireData* fireData, unsigned long msElapsed) {
  random16_add_entropy(random());

  int maximumHeat = fireData->pixelHeatArrayLength * MAX_PIXEL_HEAT / 50;
  int heatToInject = random16(maximumHeat);

  coolPixels(fireData, msElapsed);
  driftUpAndDiffuse(fireData);
  // injectLinearHeat(fireData, heatToInject);

  if (random8() < 40) {
    int y = random8(7);
    fireData->pixelHeatArray[y] = qadd8( fireData->pixelHeatArray[y], random8(100, 180) );
  }

  applyHeatArrayToPixels(fireData, ledArray);
}
