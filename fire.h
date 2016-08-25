struct FireData {
  byte* pixelHeatArray;
  int   pixelHeatArrayLength;
  int   msToCool;
};
FireData* fireSetup(int ledArrayLength, int msToCool);
void randomFire(CRGB* ledArray, FireData* fireData, int msElapsed);
