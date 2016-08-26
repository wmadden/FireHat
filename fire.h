struct FireData {
  float* pixelHeatArray;
  int   pixelHeatArrayLength;
  // int   msToCool;
  float amountCooledPerMs;
  int indexDirection;
  int indexOffset;
  int ledArrayLength;
};
FireData* fireSetup(int pixelHeatArrayLength, int msToCool);
void randomFire(FireData* fireData, unsigned long msElapsed, CRGB* destLEDArray, int indexDirection, int destArrayOffset, int destArrayLength);
