#include <FastLED.h>

byte prevKeyState = HIGH; // button is active low
#define BUTTON_PIN  10 // Pin button is connected to

void buttonSetup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
}

int readButtonState() {
  byte currKeyState = digitalRead(BUTTON_PIN);
  int buttonPressed = 0;
  if ((prevKeyState == LOW) && (currKeyState == HIGH)) {
    buttonPressed = 1;
  }
  prevKeyState = currKeyState;
  return buttonPressed;
}
