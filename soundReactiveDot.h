struct SoundReactiveDotData {
  byte peak; // Used for falling dot
  float peakDropTimer;
};

SoundReactiveDotData* soundReactiveDotSetup();

void soundReactiveDot(SoundReactiveDotData* data, CRGB* ledArray, int pixelCount, unsigned long msElapsed, int audioLevel, int destOffset, int destArrayLength, int direction);
