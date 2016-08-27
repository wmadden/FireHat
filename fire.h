struct FireData {
  float* pixelHeatArray;
  int   pixelHeatArrayLength;
  float amountCooledPerMs;
  int indexDirection;
};
FireData* fireSetup(int ledArrayLength, int msToCool, int indexDirection);
void randomFire(CRGB* ledArray, FireData* fireData, unsigned long msElapsed, int destOffset, int destArrayLength, int direction);
void fire(CRGB* ledArray, FireData* fireData, unsigned long msElapsed, int heatToInject, int destOffset, int destArrayLength, int direction);
