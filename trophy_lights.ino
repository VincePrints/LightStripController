//Effects pulled from https://www.tweaking4all.com/hardware/arduino/adruino-led-strip-effects/

#include <Neopixel.h>

#define LED_PIN      4
#define BUTTON_PIN   2    
#define PIXEL_PIN    3
#define PIXEL_COUNT 33
#define BRIGHTNESS 150

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, NEO_GRB + NEO_KHZ800);

volatile int showType = 0;
const int numShows = 16;

void setup() {
  Serial.begin(9600);
  Serial.println("Serial begin");
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), buttonInterrupt, FALLING);
  pixels.begin();
  startShow(0); // Initialize all pixels to 'off'
  strip.setBrightness(BRIGHTNESS); //set brightness
}

void loop() {
  startShow(showType);
}

void buttonInterrupt() {
  showType = ((showType + 1) % numShows);
}

void startShow(int i) {
  switch(i){
    case 0: Serial.println("Off");
            colorWipe(strip.Color(  0,   0,   0), 10);
            break;
    case 1: Serial.println("White");
            colorWipe(pixels.Color(0, 0, 0, 255), 50);
            break;
    case 2: Serial.println("Red");
            colorWipe(pixels.Color(255, 0, 0), 75);
            break;
    case 3: Serial.println("Orange");
            colorWipe(pixels.Color(200, 150, 0), 75);
            break;
    case 4: Serial.println("Yellow");
            colorWipe(pixels.Color(255, 255, 0), 75);
            break;
    case 5: Serial.println("Green");
            colorWipe(pixels.Color(0, 255, 0), 75);
            break;
    case 6: Serial.println("Teal");
            colorWipe(pixels.Color(0, 255, 255), 75);
            break;
    case 7: Serial.println("Violet");
            colorWipe(pixels.Color(150, 50, 255), 75);
            break;
    case 8: Serial.println("Rainbow");
            while (showType == 8) {rainbowCycle(50);}
            break;
    case 9: Serial.println("Rainbow Theater");
            while (showType == 9) {theaterChaseRainbow(50);}
            break;
    case 10: Serial.println("Strobe"); 
            while (showType == 10) {strobe(0xff, 0xff, 0xff, 10, 50, 1000);}
            break;
    case 11: Serial.println("Cylon Bounce"); 
            while (showType == 11) {cylonBounce(0xff, 0, 0, 4, 10, 50);}
            break;
    case 12: Serial.println("Sparkle");
            while (showType == 12) {sparkle(0xff, 0xff, 0xff, 0);}
            break;
    case 13: Serial.println("Fire"); 
            while (showType == 13) {fire(55,120,15);}
            break;
    case 14: Serial.println("Bouncing Balls");
            byte colors[3][3] =  { {0xff, 0,0},
                                  {0xff, 0xff, 0xff},
                                  {0, 0, 0xff} };
            while (showType == 14) {bouncingColoredBalls(3, colors);}
            break;
    case 15: Serial.println("Meteor Rain");
            while (showType == 15) {meteorRain(0xff,0xff,0xff,10, 64, true, 30);}
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
  for (int i = 0; i < 256; i++) {
    for (int j = 0; j < pixels.numPixels(); j++) {
      pixels.setPixelColor(j, Wheel(((j * 256 / pixels.numPixels()) + i) & 255));
    }
    pixels.show();
    unsigned long start = millis();
    while (millis() - start < wait) {}
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
  int firstPixelHue = 0;     // First pixel starts at red (hue 0)
  for(int a=0; a<30; a++) {  // Repeat 30 times...
    for(int b=0; b<3; b++) { //  'b' counts from 0 to 2...
      strip.clear();         //   Set all pixels in RAM to 0 (off)
      // 'c' counts up from 'b' to end of strip in increments of 3...
      for(int c=b; c<strip.numPixels(); c += 3) {
        // hue of pixel 'c' is offset by an amount to make one full
        // revolution of the color wheel (range 65536) along the length
        // of the strip (strip.numPixels() steps):
        int      hue   = firstPixelHue + c * 65536L / strip.numPixels();
        uint32_t color = strip.gamma32(strip.ColorHSV(hue)); // hue -> RGB
        strip.setPixelColor(c, color); // Set pixel 'c' to value 'color'
      }
      strip.show();                // Update strip with new contents
      delay(wait);                 // Pause for a moment
      firstPixelHue += 65536 / 90; // One cycle of color wheel over 90 frames
    }
  }
}

void strobe(byte red, byte green, byte blue, int StrobeCount, int FlashDelay, int EndPause){
  for(int j = 0; j < StrobeCount; j++) {
    setAll(red,green,blue);
    showStrip();
    delay(FlashDelay);
    setAll(0,0,0);
    showStrip();
    delay(FlashDelay);
  }
 
 delay(EndPause);
}

void cylonBounce(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay){

  for(int i = 0; i < PIXEL_COUNT-EyeSize-2; i++) {
    setAll(0,0,0);
    setPixel(i, red/10, green/10, blue/10);
    for(int j = 1; j <= EyeSize; j++) {
      setPixel(i+j, red, green, blue);
    }
    setPixel(i+EyeSize+1, red/10, green/10, blue/10);
    showStrip();
    delay(SpeedDelay);
  }

  delay(ReturnDelay);

  for(int i = PIXEL_COUNT-EyeSize-2; i > 0; i--) {
    setAll(0,0,0);
    setPixel(i, red/10, green/10, blue/10);
    for(int j = 1; j <= EyeSize; j++) {
      setPixel(i+j, red, green, blue);
    }
    setPixel(i+EyeSize+1, red/10, green/10, blue/10);
    showStrip();
    delay(SpeedDelay);
  }
 
  delay(ReturnDelay);
}

void sparkle(byte red, byte green, byte blue, int SpeedDelay) {
  int Pixel = random(PIXEL_COUNT);
  setPixel(Pixel,red,green,blue);
  showStrip();
  delay(SpeedDelay);
  setPixel(Pixel,0,0,0);
}

void fire(int Cooling, int Sparking, int SpeedDelay) {
  static byte heat[PIXEL_COUNT];
  int cooldown;
 
  // Step 1.  Cool down every cell a little
  for( int i = 0; i < PIXEL_COUNT; i++) {
    cooldown = random(0, ((Cooling * 10) / PIXEL_COUNT) + 2);
   
    if(cooldown>heat[i]) {
      heat[i]=0;
    } else {
      heat[i]=heat[i]-cooldown;
    }
  }
 
  // Step 2.  Heat from each cell drifts 'up' and diffuses a little
  for( int k= PIXEL_COUNT - 1; k >= 2; k--) {
    heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2]) / 3;
  }
   
  // Step 3.  Randomly ignite new 'sparks' near the bottom
  if( random(255) < Sparking ) {
    int y = random(7);
    heat[y] = heat[y] + random(160,255);
    //heat[y] = random(160,255);
  }

  // Step 4.  Convert heat to LED colors
  for( int j = 0; j < PIXEL_COUNT; j++) {
    setPixelHeatColor(j, heat[j] );
  }

  showStrip();
  delay(SpeedDelay);
}

void setPixelHeatColor (int Pixel, byte temperature) {
  // Scale 'heat' down from 0-255 to 0-191
  byte t192 = round((temperature/255.0)*191);
 
  // calculate ramp up from
  byte heatramp = t192 & 0x3F; // 0..63
  heatramp <<= 2; // scale up to 0..252
 
  // figure out which third of the spectrum we're in:
  if( t192 > 0x80) {                     // hottest
    setPixel(Pixel, 255, 255, heatramp);
  } else if( t192 > 0x40 ) {             // middle
    setPixel(Pixel, 255, heatramp, 0);
  } else {                               // coolest
    setPixel(Pixel, heatramp, 0, 0);
  }
}

void bouncingColoredBalls(int BallCount, byte colors[][3]) {
  float Gravity = -9.81;
  int StartHeight = 1;
 
  float Height[BallCount];
  float ImpactVelocityStart = sqrt( -2 * Gravity * StartHeight );
  float ImpactVelocity[BallCount];
  float TimeSinceLastBounce[BallCount];
  int   Position[BallCount];
  long  ClockTimeSinceLastBounce[BallCount];
  float Dampening[BallCount];
 
  for (int i = 0 ; i < BallCount ; i++) {  
    ClockTimeSinceLastBounce[i] = millis();
    Height[i] = StartHeight;
    Position[i] = 0;
    ImpactVelocity[i] = ImpactVelocityStart;
    TimeSinceLastBounce[i] = 0;
    Dampening[i] = 0.90 - float(i)/pow(BallCount,2);
  }

  while (true) {
    for (int i = 0 ; i < BallCount ; i++) {
      TimeSinceLastBounce[i] =  millis() - ClockTimeSinceLastBounce[i];
      Height[i] = 0.5 * Gravity * pow( TimeSinceLastBounce[i]/1000 , 2.0 ) + ImpactVelocity[i] * TimeSinceLastBounce[i]/1000;
 
      if ( Height[i] < 0 ) {                      
        Height[i] = 0;
        ImpactVelocity[i] = Dampening[i] * ImpactVelocity[i];
        ClockTimeSinceLastBounce[i] = millis();
 
        if ( ImpactVelocity[i] < 0.01 ) {
          ImpactVelocity[i] = ImpactVelocityStart;
        }
      }
      Position[i] = round( Height[i] * (PIXEL_COUNT - 1) / StartHeight);
    }
 
    for (int i = 0 ; i < BallCount ; i++) {
      setPixel(Position[i],colors[i][0],colors[i][1],colors[i][2]);
    }
   
    showStrip();
    setAll(0,0,0);
  }
}

void meteorRain(byte red, byte green, byte blue, byte meteorSize, byte meteorTrailDecay, boolean meteorRandomDecay, int SpeedDelay) {  
  setAll(0,0,0);
 
  for(int i = 0; i < PIXEL_COUNT+PIXEL_COUNT; i++) {
   
   
    // fade brightness all LEDs one step
    for(int j=0; j<PIXEL_COUNT; j++) {
      if( (!meteorRandomDecay) || (random(10)>5) ) {
        fadeToBlack(j, meteorTrailDecay );        
      }
    }
   
    // draw meteor
    for(int j = 0; j < meteorSize; j++) {
      if( ( i-j <PIXEL_COUNT) && (i-j>=0) ) {
        setPixel(i-j, red, green, blue);
      }
    }
   
    showStrip();
    delay(SpeedDelay);
  }
}

void fadeToBlack(int ledNo, byte fadeValue) {
    uint32_t oldColor;
    uint8_t r, g, b;
    int value;
   
    oldColor = strip.getPixelColor(ledNo);
    r = (oldColor & 0x00ff0000UL) >> 16;
    g = (oldColor & 0x0000ff00UL) >> 8;
    b = (oldColor & 0x000000ffUL);

    r=(r<=10)? 0 : (int) r-(r*fadeValue/256);
    g=(g<=10)? 0 : (int) g-(g*fadeValue/256);
    b=(b<=10)? 0 : (int) b-(b*fadeValue/256);
   
    strip.setPixelColor(ledNo, r,g,b); 
}
