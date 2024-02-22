#include <Adafruit_NeoPixel.h>

#define LED_PIN      4
#define BUTTON_PIN   2    
#define PIXEL_PIN    3
#define PIXEL_COUNT 33

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, NEO_GRB + NEO_KHZ800);

const int LONG_PRESS_TIME = 2000;
unsigned long pressTime, releaseTime;
bool pwrState = LOW;
bool longPress = true;
int showType = 0;
int pwrOffType = 0;

int numShows = 9; //Enter the number of shows that are defined in "startShow"

//Interrupt Detection Routine
volatile bool interrupt = false;

void buttonInterrupt() {
  pressTime = millis();
  Serial.println("Press begin: " + pressTime);
  interrupt = true;
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), buttonRelease, FALLING);
}

void buttonRelease() {
  delay(100);
  releaseTime = millis();
  Serial.println("Press release: " + releaseTime);
  long pressDuration = (releaseTime - pressTime);
  Serial.println("Press duration: " + pressDuration);

  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), buttonInterrupt, RISING);

  if ((pressDuration >= LONG_PRESS_TIME) && interrupt) {
    longPress = true;
    if (pwrState == LOW) {
      Serial.println("Power on.");
      !pwrState;
      digitalWrite(LED_PIN, HIGH);
      pixels.clear();
      delay(5);
      startShow(pwrOffType);
    } else {
      Serial.println("Power off.");
      !pwrState;
      digitalWrite(LED_PIN, LOW);
      pixels.clear();
      delay(5);
      pixels.show(); //Turns off LEDs
    }
  } else if ((pressDuration != 0) && (pwrState == HIGH) && interrupt) {
    showType = ((showType + 1) % numShows);
    Serial.println("Show type: " + showType);
    pixels.clear();
    startShow(showType);
  }
  interrupt = false;
}

void setup() {
  Serial.begin(9600);
  Serial.println("Serial begin");
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), buttonInterrupt, RISING);
  pixels.begin();
  startShow(1); // Initialize all pixels to 'off'
}

void loop() {
}

void startShow(int i) {
  switch(i){
    case 0: colorWipe(pixels.Color(255, 255, 255), 50);  // White
            Serial.println("White");
            break;
    case 1: colorWipe(pixels.Color(255, 0, 0), 75);  // Red
            Serial.println("Red");
            break;
    case 2: colorWipe(pixels.Color(200, 150, 0), 75);  // Orange
            Serial.println("Orange");
            break;
    case 3: colorWipe(pixels.Color(255, 255, 0), 75);  // Yellow
            Serial.println("Yellow");
            break;
    case 4: colorWipe(pixels.Color(0, 255, 0), 75);  // Green
            Serial.println("Green");
            break;
    case 5: colorWipe(pixels.Color(0, 255, 255), 75);  // Teal
            Serial.println("Teal");
            break;
    case 6: colorWipe(pixels.Color(0, 0, 255), 75);  // Blue
            Serial.println("Blue");
            break;
    case 7: colorWipe(pixels.Color(150, 50, 255), 75);  // Violet
            Serial.println("Violet");
            break;
    case 8: rainbowCycle(50);
            Serial.println("Rainbow");
            break;
  }
}

void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i = 0; i < pixels.numPixels(); i++) {
    pixels.setPixelColor(i, c);
    pixels.show();
    unsigned long start = millis();
    while (millis() - start < wait) {}
  }
}

void rainbowCycle(uint8_t wait) {
  while (pwrState == HIGH) {
    for (int i = 0; i < 256; i++) { // 5 cycles of all colors on wheel
      for (int j = 0; j < pixels.numPixels(); j++) {
        pixels.setPixelColor(j, Wheel(((j * 256 / pixels.numPixels()) + i) & 255));
      }
      pixels.show();
      unsigned long start = millis();
      while (millis() - start < wait) {}
    }
  }
}

uint32_t Wheel(byte WheelPos) {
  if(WheelPos < 85) {
    return pixels.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return pixels.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return pixels.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
