#include <FastLED.h>
#include "fire.h"

// Fire animation variables
const CRGBPalette16 FIRE_PALETTE = CRGBPalette16( CRGB::Black, CRGB::Red, CRGB::Yellow, CRGB::White);

#define MAX_PIXEL_HEAT 255

FireData* fireSetup(int ledArrayLength, int msToCool) {
  FireData* fireData = new FireData();
  fireData->pixelHeatArray = new byte[ledArrayLength]();
  fireData->pixelHeatArrayLength = ledArrayLength;
  fireData->msToCool = msToCool;

  int initialValue = 0;
  for (int i = 0; i < ledArrayLength; i++) {
    fireData->pixelHeatArray[i] = initialValue;
    initialValue += 255 / fireData->pixelHeatArrayLength;
  }

  return fireData;
}

void applyHeatArrayToPixels(byte* pixelHeatArray, CRGB* ledArray, int ledArrayLength) {
  for(int i = 0; i < ledArrayLength; i++) {
    byte colorindex = pixelHeatArray[i];
    CRGB color = ColorFromPalette(FIRE_PALETTE, colorindex).nscale8(255 / 10);
    ledArray[i] = color;
  }
}

void coolPixels(byte* pixelHeatArray, int pixelHeatArrayLength, int msToCool, int msElapsed) {
  int amountCooledPerMs = msToCool / 255;
  int amountCooledSinceLastFrame = amountCooledPerMs * msElapsed;

  for (int i = 0; i < pixelHeatArrayLength; i++) {
    pixelHeatArray[i] -= amountCooledSinceLastFrame;
    if (pixelHeatArray[i] < 0) pixelHeatArray[i] = 0;
  }
}

// Applies fire animation to heat array.
// msToCool: number of milliseconds required for a pixel to cool completely
void applyFireToHeatArray(byte* pixelHeatArray, int pixelHeatArrayLength, int msToCool, int msElapsed, int heatToInject) {
  // Every pixel has a certain amount of heat which cools every frame.
  // Heat is injected at the bottom and flows upwards. Each pixel holds one byte (255) of
  // heat, which overflows to the next pixel.

  // coolPixels(pixelHeatArray, pixelHeatArrayLength, msToCool, msElapsed);

  // Inject heat
  // for (int i = 0; i < pixelHeatArrayLength; i++) {
  //   int pixelHeatCapacity = MAX_PIXEL_HEAT - pixelHeatArray[i];
  //
  //   if (pixelHeatCapacity > heatToInject) pixelHeatCapacity = heatToInject;
  //
  //   heatToInject -= pixelHeatCapacity;
  //   pixelHeatArray[i] += pixelHeatCapacity;
  //
  //   if (heatToInject == 0) break;
  // }
}

void randomFire(CRGB* ledArray, FireData* fireData, int msElapsed) {
  random16_add_entropy(random());

  int heatToInject = 0;
  // if (random8() / 255 > percentageChanceToSpark) {
  //   int maximumHeat = ledArrayLength * MAX_PIXEL_HEAT;
  //   heatToInject = random8() * maximumHeat;
  // }

  applyFireToHeatArray(fireData->pixelHeatArray, fireData->pixelHeatArrayLength, fireData->msToCool, msElapsed, heatToInject);
  applyHeatArrayToPixels(fireData->pixelHeatArray, ledArray, fireData->pixelHeatArrayLength);
}
