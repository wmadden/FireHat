struct FireData {
  float* pixelHeatArray;
  int   pixelHeatArrayLength;
  // int   msToCool;
  float amountCooledPerMs;
};
FireData* fireSetup(int ledArrayLength, int msToCool);
void randomFire(CRGB* ledArray, FireData* fireData, unsigned long msElapsed);
