#include <FastLED.h>
#include "config.h"
#include "transformIndex.h";
#include "soundReactiveDot.h";

#define PEAK_FALL_RATE 100 // Milliseconds before peak drops

SoundReactiveDotData* soundReactiveDotSetup() {
  SoundReactiveDotData* data = new SoundReactiveDotData();
  return data;
}

void soundReactiveDot(SoundReactiveDotData* data, CRGBPalette16 palette, CRGB* ledArray, int pixelCount, unsigned long msElapsed, int audioLevel, int destOffset, int destArrayLength, int direction) {
  if(audioLevel < 0L) audioLevel = 0; // Clip output
  else if(audioLevel > MAX_AUDIO_LEVEL) audioLevel = MAX_AUDIO_LEVEL;

  int scaledAudioLevel = map(audioLevel, 0, MAX_AUDIO_LEVEL, 0, pixelCount - 1);
  if(scaledAudioLevel > data->peak) data->peak = scaledAudioLevel; // Keep 'peak' dot at top
  if (data->peak > pixelCount - 1) data->peak = pixelCount - 1;

  // Color pixels up to peak
  for(int i = 0; i < pixelCount; i++) {
    int destIndex = transformIndex(i, destOffset, destArrayLength, direction);
    if(i >= scaledAudioLevel) {
      ledArray[destIndex] = CRGB::Black;
    } else {
      ledArray[destIndex] = ColorFromPalette(palette, map(i, 0, pixelCount - 1, 0, 255));
    }
  }

  // Draw dot at peak
  if (data->peak > 0) {
    int destIndex = transformIndex(data->peak, destOffset, destArrayLength, direction);
    ledArray[destIndex] = CRGB::White;
  }

  // Every PEAK_FALL_RATE milliseconds, make the peak pixel drop by 1:
  data->peakDropTimer += msElapsed;
  if(data->peakDropTimer > PEAK_FALL_RATE) {
    if(data->peak > 0) data->peak--;
    data->peakDropTimer = 0;
  }
}
