//Written by Vince_Prints 2024
//Effects ideas from: https://www.tweaking4all.com/hardware/arduino/adruino-led-strip-effects/
//Adapted to be non-blocking by Vince_Prints

#include <Adafruit_NeoPixel.h>

#define LED_PIN      4
#define BUTTON_PIN   3    
#define PIXEL_PIN    5
#define PIXEL_COUNT 33
#define BRIGHTNESS 150

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, NEO_GRB + NEO_KHZ800);

volatile bool buttonPressed = false;
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 250; // Adjust this value as needed

const int numShows = 15;
int showType = 0;

// Adjust this value as needed for the desired fire effect speed
#define FIRE_DELAY 50
unsigned long lastFireUpdate = 0;
//

void setup() {
  Serial.begin(9600);
  Serial.println("Serial begin");

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);

  pixels.begin();
  pixels.show(); // Initialize all pixels to 'off'
  pixels.setBrightness(BRIGHTNESS); //set brightness

  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), buttonInterrupt, FALLING);
}

void loop() {
  startShow(showType);
  if (buttonPressed) {
    handleButtonPress();
    buttonPressed = false; // Reset buttonPressed flag
  }
}

void buttonInterrupt() {
  unsigned long currentMillis = millis();
  if (currentMillis - lastDebounceTime >= debounceDelay) {
    Serial.println("Button Pressed");
    buttonPressed = true;
    lastDebounceTime = currentMillis;
  }
}

void handleButtonPress() {
  showType = (showType + 1) % numShows; // Increment showType for short press
  pixels.clear(); // Clear pixels
  pixels.show(); // Update pixels
}

void startShow(int i) {
  switch(i){
    case 0: Serial.println("Off");
            colorPulse(pixels.Color(  0,   0,   0), 0);
            break;
    case 1: Serial.println("White");
            colorPulse(pixels.Color(100, 100, 100), 25);
            break;
    case 2: Serial.println("Red");
            colorPulse(pixels.Color(255, 0, 0), 25);
            break;
    case 3: Serial.println("Orange");
            colorPulse(pixels.Color(244, 50, 0), 25);
            break;
    case 4: Serial.println("Yellow");
            colorPulse(pixels.Color(255, 100, 0), 25);
            break;
    case 5: Serial.println("Green");
            colorPulse(pixels.Color(0, 255, 0), 25);
            break;
    case 6: Serial.println("Teal");
            colorPulse(pixels.Color(0, 255, 255), 25);
            break;
    case 7: Serial.println("Violet");
            colorPulse(pixels.Color(150, 50, 255), 25);
            break;
    case 8: Serial.println("Rainbow");
            rainbowCycle(50);
            break;
    case 9: Serial.println("Rainbow Theater");
            theaterChaseRainbow(50);
            break;
    case 10: Serial.println("Strobe"); 
            strobe(100, 100, 100, 10, 50, 500);
            break;
    case 11: Serial.println("Cylon Bounce"); 
            cylonBounce(0xff, 0, 0, 4, 20, 50);
            break;
    case 12: Serial.println("Sparkle");
            sparkle(255, 255, 255, 0);
            break;
    case 13: Serial.println("Fire"); 
            fire(55,120);
            break;
    case 14: Serial.println("Meteor Rain");
            meteorRain(0xff,0xff,0xff,10, 64, true, 30);
            break;
  }
}

void colorPulse(uint32_t c, uint8_t pulseSpeed) {
  static bool increasing = true;
  static uint8_t brightness = 0;
  static unsigned long lastUpdate = 0;

  unsigned long currentMillis = millis();
  
  if (currentMillis - lastUpdate >= pulseSpeed) {
    if (increasing) {
      brightness += 5; // Increase brightness gradually
      if (brightness >= 255) {
        increasing = false; // Start decreasing brightness
      }
    } else {
      brightness -= 5; // Decrease brightness gradually
      if (brightness <= 0) {
        increasing = true; // Start increasing brightness again
      }
    }

    // Set all pixels to the specified color with current brightness
    for (uint16_t i = 0; i < pixels.numPixels(); i++) {
      pixels.setPixelColor(i, pixels.Color(
        (uint8_t)(((c >> 16) & 0xFF) * brightness / 255),
        (uint8_t)(((c >> 8) & 0xFF) * brightness / 255),
        (uint8_t)((c & 0xFF) * brightness / 255)
      ));
    }
    
    pixels.show();
    lastUpdate = millis();
  }
}

#define RAINBOW_CYCLE_DELAY 10  // Delay between each step in the rainbow cycle

uint16_t rainbowCycleIndex = 0;  // Current position in the rainbow cycle

void rainbowCycle(uint8_t wait) {
  static unsigned long previousMillis = 0;
  
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= RAINBOW_CYCLE_DELAY) {
    previousMillis = currentMillis;

    // Increment the rainbow cycle index
    rainbowCycleIndex++;

    // Update the colors of each pixel based on the current rainbow cycle index
    for (int j = 0; j < pixels.numPixels(); j++) {
      pixels.setPixelColor(j, Wheel(((j * 256 / pixels.numPixels()) + rainbowCycleIndex) & 255));
    }
    pixels.show();
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

void theaterChaseRainbow(int wait) {
  static int firstPixelHue = 0;
  static int chaseIndex = 0;
  static unsigned long lastUpdate = 0;
  
  if (millis() - lastUpdate >= wait) {
    pixels.clear();
    for (int c = chaseIndex; c < PIXEL_COUNT; c += 3) {
      int hue = firstPixelHue + c * 65536L / PIXEL_COUNT;
      uint32_t color = pixels.gamma32(pixels.ColorHSV(hue));
      pixels.setPixelColor(c, color);
    }
    pixels.show();
    
    // Increment chaseIndex for the next update
    chaseIndex = (chaseIndex + 1) % 3;
    
    // Increment firstPixelHue for color wheel rotation
    firstPixelHue += 65536 / 90;
    
    lastUpdate = millis();
  }
}

void strobe(byte red, byte green, byte blue, int StrobeCount, int FlashDelay, int EndPause) {
  for (int j = 0; j < StrobeCount; j++) {
    // Turn on the pixels with the specified color
    for (int i = 0; i < pixels.numPixels(); i++) {
      pixels.setPixelColor(i, pixels.Color(red, green, blue));
    }
    pixels.show(); // Show the updated pixel colors
    delay(FlashDelay);

    // Turn off all pixels (set them to black)
    for (int i = 0; i < pixels.numPixels(); i++) {
      pixels.setPixelColor(i, pixels.Color(0, 0, 0));
    }
    pixels.show(); // Show the updated pixel colors
    delay(FlashDelay);
  }
 
  delay(EndPause);
}

void cylonBounce(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay) {
  static int i = 0;
  static bool movingRight = true;
  static unsigned long lastUpdate = 0;
  
  if (millis() - lastUpdate >= SpeedDelay) {
    pixels.clear();

    // Draw the "eye"
    for (int j = 0; j <= EyeSize; j++) {
      pixels.setPixelColor(i + j, red, green, blue);
    }

    pixels.show();
    
    // Move the "eye" left or right
    if (movingRight) {
      i++;
      // If the "eye" reaches the end, change direction
      if (i >= PIXEL_COUNT - EyeSize) {
        movingRight = false;
      }
    } else {
      i--;
      // If the "eye" reaches the beginning, change direction
      if (i <= 0) {
        movingRight = true;
      }
    }
    
    lastUpdate = millis();
  }

  // Introduce a delay before reversing direction
  if ((movingRight && i >= PIXEL_COUNT - EyeSize) || (!movingRight && i <= 0)) {
    delay(ReturnDelay);
  }
}

void sparkle(byte red, byte green, byte blue, int SpeedDelay) {
  static unsigned long lastUpdate = 0;
  static int Pixel = 0;
  static bool active = false;

  if (!active) {
    Pixel = random(PIXEL_COUNT);
    pixels.setPixelColor(Pixel, red, green, blue);
    pixels.show();
    active = true;
    lastUpdate = millis();
  } else {
    if (millis() - lastUpdate >= SpeedDelay) {
      pixels.setPixelColor(Pixel, 0, 0, 0);
      pixels.show();
      active = false;
    }
  }
}

void fire(int Cooling, int Sparking) {
  static byte heat[PIXEL_COUNT];
  int cooldown;
 
  unsigned long currentMillis = millis();

  // Check if it's time to update the fire effect
  if (currentMillis - lastFireUpdate >= FIRE_DELAY) {
    lastFireUpdate = currentMillis;

    // Step 1. Cool down every cell a little
    for (int i = 0; i < PIXEL_COUNT; i++) {
      cooldown = random(0, ((Cooling * 10) / PIXEL_COUNT) + 2);
     
      if (cooldown > heat[i]) {
        heat[i] = 0;
      } else {
        heat[i] = heat[i] - cooldown;
      }
    }
   
    // Step 2. Heat from each cell drifts 'up' and diffuses a little
    for (int k = PIXEL_COUNT - 1; k >= 2; k--) {
      heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2]) / 3;
    }
     
    // Step 3. Randomly ignite new 'sparks' near the bottom
    if (random(255) < Sparking) {
      int y = random(7);
      heat[y] = heat[y] + random(160, 255);
    }

    // Step 4. Convert heat to LED colors
    for (int j = 0; j < PIXEL_COUNT; j++) {
      setPixelHeatColor(j, heat[j]);
    }

    pixels.show();
  }
}

void setPixelHeatColor (int Pixel, byte temperature) {
  // Scale 'heat' down from 0-255 to 0-191
  byte t192 = round((temperature/255.0)*191);
 
  // calculate ramp up from
  byte heatramp = t192 & 0x3F; // 0..63
  heatramp <<= 2; // scale up to 0..252
 
  // figure out which third of the spectrum we're in:
  if( t192 > 0x80) {                     // hottest
    pixels.setPixelColor(Pixel, 255, 255, heatramp);
  } else if( t192 > 0x40 ) {             // middle
    pixels.setPixelColor(Pixel, 255, heatramp, 0);
  } else {                               // coolest
    pixels.setPixelColor(Pixel, heatramp, 0, 0);
  }
}

//void loop() {
 // meteorRain(0xff,0xff,0xff,10, 64, true, 30);
//}

unsigned long lastUpdateTime = 0;

void meteorRain(byte red, byte green, byte blue, byte meteorSize, byte meteorTrailDecay, boolean meteorRandomDecay, int SpeedDelay) {
  static int i = 0; // Declare and initialize i as a static variable
  
  if (millis() - lastUpdateTime >= SpeedDelay) {
    lastUpdateTime = millis();

    pixels.Color(0, 0, 0);

    // Move the meteor
    for (int j = 0; j < PIXEL_COUNT; j++) {
      if ((!meteorRandomDecay) || (random(10) > 5)) {
        fadeToBlack(j, meteorTrailDecay);
      }
    }

    // Draw meteor
    for (int j = 0; j < meteorSize; j++) {
      if ((i - j < PIXEL_COUNT) && (i - j >= 0)) {
        pixels.setPixelColor(i - j, red, green, blue);
      }
    }

    pixels.show();

    i++; // Increment i for next iteration
    if (i >= PIXEL_COUNT + PIXEL_COUNT) { // Reset i when it exceeds the pixel count
      i = 0;
    }
  }
}

void fadeToBlack(int ledNo, byte fadeValue) {
  uint32_t oldColor;
  uint8_t r, g, b;
  int value;

  oldColor = pixels.getPixelColor(ledNo);
  r = (oldColor & 0x00ff0000UL) >> 16;
  g = (oldColor & 0x0000ff00UL) >> 8;
  b = (oldColor & 0x000000ffUL);

  r = (r <= 10) ? 0 : (int)r - (r * fadeValue / 256);
  g = (g <= 10) ? 0 : (int)g - (g * fadeValue / 256);
  b = (b <= 10) ? 0 : (int)b - (b * fadeValue / 256);

  pixels.setPixelColor(ledNo, r, g, b);
}
