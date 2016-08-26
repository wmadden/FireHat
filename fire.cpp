#include <FastLED.h>
#include "fire.h"

// Fire animation variables
const CRGBPalette16 FIRE_PALETTE = CRGBPalette16( CRGB::Black, CRGB::Red, CRGB::Yellow, CRGB::White);

#define MAX_PIXEL_HEAT 240

void injectLinearHeat(FireData* fireData, int heatToInject);

FireData* fireSetup(int pixelHeatArrayLength, int msToCool) {
// FireData* fireSetup(int ledArrayLength, int msToCool, int indexDirection, int indexOffset) {
  FireData* fireData = new FireData();
  fireData->pixelHeatArray = new float[pixelHeatArrayLength]();
  fireData->pixelHeatArrayLength = pixelHeatArrayLength;
  fireData->amountCooledPerMs = (float)255 / msToCool;
  // fireData->indexDirection = indexDirection;
  // fireData->indexOffset = indexOffset;
  return fireData;
}

#define BRIGHTNESS 25.5 // Between 0-255

void applyHeatArrayToPixels(FireData* fireData, CRGB* destLEDArray, int indexDirection, int destArrayOffset, int destArrayLength) {
  float* pixelHeatArray = fireData->pixelHeatArray;
  int pixelHeatArrayLength = fireData->pixelHeatArrayLength;

  for(int i = 0; i < destArrayLength; i++) {
    float colorindex;

    if (indexDirection == 1) {
      colorindex = pixelHeatArray[i];
    } else {
      colorindex = pixelHeatArray[pixelHeatArrayLength - 1 - i];
    }

    CRGB color = ColorFromPalette(FIRE_PALETTE, colorindex);
    // color = color.nscale8(BRIGHTNESS);
    // destLEDArray[(destArrayOffset + i) % destArrayLength] = color;
    destLEDArray[(destArrayOffset + i) % destArrayLength] = color;
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

#define SPARK_CHANCE 40 // Number between 0-255
#define MIN_SPARK_HEAT 100
#define MAX_SPARK_HEAT 180

void spark(FireData* fireData, int sparkHeight, int sparkHeat) {
  fireData->pixelHeatArray[sparkHeight] = qadd8(fireData->pixelHeatArray[sparkHeight], sparkHeat);
}

void randomFire(FireData* fireData, unsigned long msElapsed, CRGB* destLEDArray, int indexDirection, int destArrayOffset, int destArrayLength) {
  random16_add_entropy(random());

  int maximumHeat = fireData->pixelHeatArrayLength * MAX_PIXEL_HEAT / 50;
  int heatToInject = random16(maximumHeat);

  coolPixels(fireData, msElapsed);
  driftUpAndDiffuse(fireData);
  // injectLinearHeat(fireData, heatToInject);

  if (random8() < SPARK_CHANCE) {
    int sparkHeight = random8(1 + fireData->pixelHeatArrayLength / 10);
    int sparkHeat = random8(MIN_SPARK_HEAT, MAX_SPARK_HEAT);
    spark(fireData, sparkHeight, sparkHeat);
  }

  applyHeatArrayToPixels(fireData, destLEDArray, indexDirection, destArrayOffset, destArrayLength);
}
