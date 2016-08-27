#include <FastLED.h>
#include "fire.h"
#include "transformIndex.h"

// Fire animation variables
const CRGBPalette16 FIRE_PALETTE = CRGBPalette16( CRGB::Black, CRGB::Red, CRGB::Yellow, CRGB::White);

#define MAX_PIXEL_HEAT 240

void injectLinearHeat(FireData* fireData, int heatToInject);

FireData* fireSetup(int ledArrayLength, int msToCool, int indexDirection) {
  FireData* fireData = new FireData();
  fireData->pixelHeatArray = new float[ledArrayLength]();
  fireData->pixelHeatArrayLength = ledArrayLength;
  fireData->amountCooledPerMs = (float)255 / msToCool;
  fireData->indexDirection = indexDirection;
  return fireData;
}

#define BRIGHTNESS 25.5 // Between 0-255

void applyHeatArrayToPixels(FireData* fireData, CRGB* ledArray, int destOffset, int destArrayLength, int direction) {
  float* pixelHeatArray = fireData->pixelHeatArray;
  int ledArrayLength = fireData->pixelHeatArrayLength;

  for(int i = 0; i < ledArrayLength; i++) {
    float colorindex;
    if (fireData->indexDirection == 1) {
      colorindex = pixelHeatArray[i];
    } else {
      colorindex = pixelHeatArray[fireData->pixelHeatArrayLength - 1 - i];
    }

    CRGB color = ColorFromPalette(FIRE_PALETTE, colorindex);
    // color = color.nscale8(BRIGHTNESS);
    int destIndex = transformIndex(i, destOffset, destArrayLength, direction);
    ledArray[destIndex] = color;
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

void driftUpAndDiffuse(FireData* fireData) {
  float* pixels = fireData->pixelHeatArray;
  for (int i = fireData->pixelHeatArrayLength - 1; i >= 2; i--) {
    pixels[i] = (pixels[i - 1] + pixels[i - 2]) / 2.0;
  }
  // pixels[2] = (pixels[1] + pixels[0]) / 3.0;
  pixels[1] = pixels[0] / 2.0;
}

#define SPARK_CHANCE 40 // Number between 0-255
#define MIN_SPARK_HEAT 100
#define MAX_SPARK_HEAT 180

void spark(FireData* fireData, int sparkHeight, int sparkHeat) {
  fireData->pixelHeatArray[sparkHeight] = qadd8(fireData->pixelHeatArray[sparkHeight], sparkHeat);
}

void randomFire(CRGB* ledArray, FireData* fireData, unsigned long msElapsed, int destOffset, int destArrayLength, int direction) {
  random16_add_entropy(random());

  coolPixels(fireData, msElapsed);
  driftUpAndDiffuse(fireData);

  if (random8() < SPARK_CHANCE) {
    int sparkHeight = 0;
    int sparkHeat = random8(MIN_SPARK_HEAT, MAX_SPARK_HEAT);
    spark(fireData, sparkHeight, sparkHeat);
  }

  applyHeatArrayToPixels(fireData, ledArray, destOffset, destArrayLength, direction);
}

void fire(CRGB* ledArray, FireData* fireData, unsigned long msElapsed, int heatToInject, int destOffset, int destArrayLength, int direction) {
  random16_add_entropy(random());

  coolPixels(fireData, msElapsed);
  driftUpAndDiffuse(fireData);

  spark(fireData, 0, heatToInject);

  applyHeatArrayToPixels(fireData, ledArray, destOffset, destArrayLength, direction);
}
