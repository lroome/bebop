// leds
#define ENABLE_DEBUG

const uint8_t kMatrixWidth = 5;
const uint8_t kMatrixHeight = 3;

#define NUM_LEDS 15
#define DATA_PIN 22
#define LED_TYPE WS2811
#define COLOR_ORDER GRB
#define VOLTS 5
#define MAX_MA 18000

#define BRIGHTNESS 20
#define FRAMES_PER_SECOND 120

CRGB leds[NUM_LEDS];
CRGB currentColor = CRGB::Blue;

int mode = 0;
#define MIN_PATTERN 0

typedef void (*SimplePatternList[])();
SimplePatternList gPatterns = { rainbow, rainbowWithGlitter, confetti, sinelon, juggle, bpm };

uint8_t gCurrentPatternNumber = 0;  // Index number of which pattern is current
uint8_t gHue = 0;                   // rotating "base color" used by many of the patterns

const int ledOrder[] = { 0, 1, 2, 5, 4, 3, 6, 7, 8, 11, 10, 9, 12, 13, 14 };
const int bottomRing[] = { 0, 5, 6, 11, 12 };
const int middleRing[] = { 1, 4, 7, 10, 13 };
const int topRing[] = { 2, 3, 8, 9, 14 };


const int firstCol[] = { 0, 1, 2 };
const int secondCol[] = { 5, 4, 3 };
const int thirdCol[] = { 6, 7, 8 };
const int fourthCol[] = { 11, 10, 9 };
const int fifthCol[] = { 12, 13, 14 };

const int starfish[] = { 0, 1, 2, 9, 10, 11, 10, 9, 3, 4, 5, 4, 3, 14, 13, 12, 13, 14, 8, 7, 6, 7, 8, 2, 1 };


#define NUM_ROWS 3
#define LEDS_PER_ROW 5

const int byRows[][LEDS_PER_ROW] = {
  { 0, 5, 6, 11, 12 },
  { 1, 4, 7, 10, 13 },
  { 2, 3, 8, 9, 14 }
};

#define NUM_COLS 5
#define LEDS_PER_COL 3
const int byColumns[][LEDS_PER_COL] = {
  { 0, 1, 2 },
  { 5, 4, 3 },
  { 6, 7, 8 },
  { 11, 10, 9 },
  { 12, 13, 14 }
};



CRGBPalette16 currentPalette;
TBlendType currentBlending;

#define UPDATES_PER_SECOND 100
extern CRGBPalette16 myRedWhiteBluePalette;
extern const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM;



void lightsSetup() {
  FastLED.setMaxPowerInVoltsAndMilliamps(VOLTS, MAX_MA);
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  currentPalette = RainbowColors_p;
  currentBlending = LINEARBLEND;
  Serial.println("lights configured");
}

void toggleMode() {
  mode = !mode;
}


void setIndividualDot(int dot, CRGB color) {
  leds[dot] = color;
}

void setRing(const int ring[], CRGB color) {
  for (int dot = 0; dot < 5; dot++) {
    setIndividualDot(ring[dot], color);
  }
}

void setCol(const int column[], CRGB color) {
  for (int dot = 0; dot < LEDS_PER_COL; dot++) {
    setIndividualDot(column[dot], color);
  }
}

void setAll() {
  for (int dot = 0; dot < NUM_LEDS; dot++) {
    // set this led
    leds[ledOrder[dot]] = currentColor;
  }
  FastLED.show();
}
void clearAll() {
  for (int dot = 0; dot < NUM_LEDS; dot++) {
    // clear this led
    leds[ledOrder[dot]] = CRGB::Black;
  }
  FastLED.show();
}

void fade() {
  int brightness = 0;
  int fadeAmount = 5;

  for (int steps = 0; steps < 102; steps++) {
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i].setRGB(0, 255, 250);  // Set Color HERE!!!
      leds[i].fadeLightBy(brightness);
    }
    FastLED.show();
    brightness = brightness + fadeAmount;
    // reverse the direction of the fading at the ends of the fade:
    if (brightness == 0 || brightness == 255) {
      fadeAmount = -fadeAmount;
    }
    delay(50);  // This delay sets speed of the fade. I usually do from 5-75 but you can always go higher.
  }
  clearAll();
  delay(50);
}

void setAColumn() {
  switch (col_loops) {
    case 0:
      setCol(firstCol, currentColor);
      break;
    case 1:
      setCol(secondCol, currentColor);
      break;
    case 2:
      setCol(thirdCol, currentColor);
      break;
    case 3:
      setCol(fourthCol, currentColor);
      break;
    case 4:
      setCol(fifthCol, currentColor);
      break;
  }
}

void loopColumns(int direction, int speed) {
  clearAll();
  pickColor();
  setAColumn();

  if (direction == 1) {
    col_loops = (col_loops + 1) % 5;
  } else {

    col_loops = (col_loops - 1);

    if (col_loops == -1) {
      col_loops = 4;
    }
  }
  FastLED.show();
  // insert a delay to keep the framerate modest
  FastLED.delay(speed);
}


void starFish(int direction, int speed) {
  clearAll();
  pickColor();
  setIndividualDot(starfish[curr_dot], currentColor);

  if (direction == 1 ) {
     curr_dot = (curr_dot + 1) % 24;
  } else {
    curr_dot = curr_dot - 1;
    if (curr_dot < 0) { curr_dot = 23; }
  }
  

  FastLED.show();
  // insert a delay to keep the framerate modest
  FastLED.delay(speed);
}


void worm(int direction, int speed) {
  clearAll();
  pickColor();
  
  setIndividualDot(starfish[curr_dot], currentColor);
  
  if (direction == 1 ) {
    if (curr_dot > 0 ) {
      setIndividualDot(starfish[curr_dot - 1], currentColor);
    }
    if (curr_dot > 1) {
      setIndividualDot(starfish[curr_dot - 2], currentColor);      
    }
    
     curr_dot = (curr_dot + 1) % 24;
  } else {
    if (curr_dot < 23)  {
       setIndividualDot(starfish[curr_dot + 1 ], currentColor);
    }
    if (curr_dot < 22)  {
       setIndividualDot(starfish[curr_dot + 2], currentColor);
    }

    curr_dot = curr_dot - 1;
    if (curr_dot < 0) { curr_dot = 23; }
  }
  

  FastLED.show();
  // insert a delay to keep the framerate modest
  FastLED.delay(speed);
}


void pickColor() {
  switch (color_loops) {
    case 0:
      currentColor = CRGB::Blue;
      break;
    case 5:
      currentColor = CRGB::Red;
      break;
    case 7:
      currentColor = CRGB::Green;
      break;
    case 9:
      currentColor = CRGB::Yellow;
      break;
    case 13:
      currentColor = CRGB::Orange;
      break;
    case 15:
      currentColor = CRGB::Purple;
      break;
    case 17:
      currentColor = CRGB::Blue;
      break;
  }
  color_loops = (color_loops + 1) % 17;
}

void ringToss(int direction, int speed) {

  clearAll();
  if (lighting) {
    pickColor();
    switch (row_loops % 3) {
      case 0:
        setRing(topRing, currentColor);
        break;
      case 1:
        setRing(middleRing, currentColor);
        break;
      case 2:
        setRing(bottomRing, currentColor);
        break;
    }
    if (direction == 1) {
      row_loops = (row_loops + 1) % 3;
    } else {
      row_loops--;
      if (row_loops < 0 ) { row_loops = 2;}
    }
  }

  FastLED.show();
  // insert a delay to keep the framerate modest
  FastLED.delay(speed);
}


void predefinedPattern(int patternNum) {
  if (patternNum > 5 || patternNum < 0) { return; }
  gPatterns[patternNum]();

  //       // send the 'leds' array out to the actual LED strip
  FastLED.show();
  //       // insert a delay to keep the framerate modest
  FastLED.delay(1000 / FRAMES_PER_SECOND);

  //       // do some periodic updates
  EVERY_N_MILLISECONDS(20) {
    gHue++;
  }  // slowly cycle the "base color" through the rainbow
}


void demos() {
  fade();
  /*
    for (int dot = 0; dot < NUM_LEDS; dot++) {
      leds[ledOrder[dot]] = currentColor;
      FastLED.show();
      // clear this led for the next time around the loop
      leds[ledOrder[dot]] = CRGB::Black;


      delay(300);
    }
    */

  for (int i = 0; i < 10; i++) {
    setRing(bottomRing, CRGB::Blue);
    setRing(middleRing, CRGB::Green);
    setRing(topRing, CRGB::Red);
    FastLED.show();
    delay(500);

    setRing(topRing, CRGB::Blue);
    setRing(bottomRing, CRGB::Green);
    setRing(middleRing, CRGB::Red);
    FastLED.show();
    delay(500);

    setRing(middleRing, CRGB::Blue);
    setRing(topRing, CRGB::Green);
    setRing(bottomRing, CRGB::Red);
    FastLED.show();
    delay(500);
  }
  clearAll();


  for (int i = 0; i < 5; i++) {
    for (int j = 0; j < 5; j++) {
      setIndividualDot(bottomRing[j], CRGB::Yellow);
      FastLED.show();
      // clear this led for the next time around the loop
      setIndividualDot(bottomRing[j], CRGB::Black);
      delay(100);
    }
  }

  for (int i = 0; i < 5; i++) {
    for (int j = 0; j < 5; j++) {
      setIndividualDot(bottomRing[j], CRGB::Green);
      setIndividualDot(middleRing[j], CRGB::Yellow);
      FastLED.show();
      // clear this led for the next time around the loop
      setIndividualDot(bottomRing[j], CRGB::Black);
      setIndividualDot(middleRing[j], CRGB::Black);
      delay(100);
    }
  }

  for (int i = 0; i < 5; i++) {
    for (int j = 0; j < 5; j++) {
      setIndividualDot(bottomRing[j], CRGB::Orange);
      setIndividualDot(middleRing[j], CRGB::Green);
      setIndividualDot(topRing[j], CRGB::Yellow);
      FastLED.show();
      // clear this led for the next time around the loop
      setIndividualDot(bottomRing[j], CRGB::Black);
      setIndividualDot(middleRing[j], CRGB::Black);
      setIndividualDot(topRing[j], CRGB::Black);
      delay(100);
    }
  }

  for (int i = 0; i < 5; i++) {
    for (int j = 4; j >= 0; j--) {
      setIndividualDot(bottomRing[j], CRGB::Purple);
      setIndividualDot(middleRing[j], CRGB::Blue);
      setIndividualDot(topRing[j], CRGB::Magenta);
      FastLED.show();
      // clear this led for the next time around the loop
      setIndividualDot(bottomRing[j], CRGB::Black);
      setIndividualDot(middleRing[j], CRGB::Black);
      setIndividualDot(topRing[j], CRGB::Black);
      delay(100);
    }
  }
}

void followSong() {
  switch (lighting) {
      case 1:
        predefinedPattern(5);
        break;
      case 2:
        predefinedPattern(1);
        break;
      case 3:
        predefinedPattern(0);
        break;
      case 4:
        loopColumns(1,250);
        break;
      case 5:
        ringToss(1, 250);
        break;
      case 6:
        starFish(1,250);
        break;
      case 7:
        starFish(0,250);
        break;
      case 8:
        loopColumns(0,250);
        break;
      case 9:
        predefinedPattern(2);
        break;
      case 10:
        predefinedPattern(3);
        break;
      case 11:
        predefinedPattern(4);
        break;
      case 12:
        ringToss(1, 250);
        break;
      case 13:
        fade();
        break;
      case 14:
        worm(1, 250);
        break;
      case 15:
        worm(0, 250);
        break; 
      default:
        clearAll();
        delay(50);
    }
}

void followNoise() {
  clearAll();
  if (lighting != 0) {
    pickColor();
    setAll();
    FastLED.show();
    FastLED.delay(500);
    clearAll();
    FastLED.delay(500);
  }
}

void LEDController(void* pvParameters) {
  for (;;) {
    if (audio_mode == 0 ) { 
      followSong();
    } else {
      followNoise();
    }

    
  }
}

// void LEDController(void* pvParameters) {
//   for (;;) {
//     if (mode == 0) {
//       ChangePalettePeriodically();

//       static uint8_t startIndex = 0;
//       startIndex = startIndex + 1; /* motion speed */

//       FillLEDsFromPaletteColors(startIndex);

//       FastLED.show();
//       FastLED.delay(1000 / UPDATES_PER_SECOND);
//     } else {
//       // Call the current pattern function once, updating the 'leds' array
//       gPatterns[gCurrentPatternNumber]();

//       // send the 'leds' array out to the actual LED strip
//       FastLED.show();
//       // insert a delay to keep the framerate modest
//       FastLED.delay(1000 / FRAMES_PER_SECOND);

//       // do some periodic updates
//       EVERY_N_MILLISECONDS(20) {
//         gHue++;
//       }  // slowly cycle the "base color" through the rainbow
//       EVERY_N_SECONDS(10) {
//         nextPattern();
//       }  // change patterns periodically
//     }
//   }
// }



void FillLEDsFromPaletteColors(uint8_t colorIndex) {
  uint8_t brightness = 255;

  for (int i = 0; i < NUM_LEDS; ++i) {
    leds[i] = ColorFromPalette(currentPalette, colorIndex, BRIGHTNESS, currentBlending);
    colorIndex += 3;
  }
}

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

void nextPattern() {
  // add one to the current pattern number, and wrap around at the end
  gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE(gPatterns);
}


void rainbow() {
  // FastLED's built-in rainbow generator
  fill_rainbow(leds, NUM_LEDS, gHue, 7);
}

void rainbowWithGlitter() {
  // built-in FastLED rainbow, plus some random sparkly glitter
  rainbow();
  addGlitter(80);
}

void addGlitter(fract8 chanceOfGlitter) {
  if (random8() < chanceOfGlitter) {
    leds[random16(NUM_LEDS)] += CRGB::White;
  }
}

void confetti() {
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy(leds, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  leds[pos] += CHSV(gHue + random8(64), 200, 255);
}

void sinelon() {
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy(leds, NUM_LEDS, 20);
  int pos = beatsin16(13, 0, NUM_LEDS - 1);
  leds[pos] += CHSV(gHue, 255, 192);
}

void bpm() {
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 120;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8(BeatsPerMinute, 64, 255);
  for (int i = 0; i < NUM_LEDS; i++) {  //9948
    leds[i] = ColorFromPalette(palette, gHue + (i * 2), beat - gHue + (i * 10));
  }
}

void juggle() {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy(leds, NUM_LEDS, 20);
  uint8_t dothue = 0;
  for (int i = 0; i < 15; i++) {
    leds[beatsin16(i + 7, 0, NUM_LEDS - 1)] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}


// There are several different palettes of colors demonstrated here.
//
// FastLED provides several 'preset' palettes: RainbowColors_p, RainbowStripeColors_p,
// OceanColors_p, CloudColors_p, LavaColors_p, ForestColors_p, and PartyColors_p.
//
// Additionally, you can manually define your own color palettes, or you can write
// code that creates color palettes on the fly.  All are shown here.

void ChangePalettePeriodically() {
  uint8_t secondHand = (millis() / 1000) % 60;
  static uint8_t lastSecond = 99;

  if (lastSecond != secondHand) {
    lastSecond = secondHand;
    if (secondHand == 0) {
      currentPalette = RainbowColors_p;
      currentBlending = LINEARBLEND;
    }
    if (secondHand == 10) {
      currentPalette = RainbowStripeColors_p;
      currentBlending = NOBLEND;
    }
    if (secondHand == 15) {
      currentPalette = RainbowStripeColors_p;
      currentBlending = LINEARBLEND;
    }
    if (secondHand == 20) {
      SetupPurpleAndGreenPalette();
      currentBlending = LINEARBLEND;
    }
    if (secondHand == 25) {
      SetupTotallyRandomPalette();
      currentBlending = LINEARBLEND;
    }
    if (secondHand == 30) {
      SetupBlackAndWhiteStripedPalette();
      currentBlending = NOBLEND;
    }
    if (secondHand == 35) {
      SetupBlackAndWhiteStripedPalette();
      currentBlending = LINEARBLEND;
    }
    if (secondHand == 40) {
      currentPalette = CloudColors_p;
      currentBlending = LINEARBLEND;
    }
    if (secondHand == 45) {
      currentPalette = PartyColors_p;
      currentBlending = LINEARBLEND;
    }
    if (secondHand == 50) {
      currentPalette = myRedWhiteBluePalette_p;
      currentBlending = NOBLEND;
    }
    if (secondHand == 55) {
      currentPalette = myRedWhiteBluePalette_p;
      currentBlending = LINEARBLEND;
    }
  }
}

// This function fills the palette with totally random colors.
void SetupTotallyRandomPalette() {
  for (int i = 0; i < 16; ++i) {
    currentPalette[i] = CHSV(random8(), 255, random8());
  }
}

// This function sets up a palette of black and white stripes,
// using code.  Since the palette is effectively an array of
// sixteen CRGB colors, the various fill_* functions can be used
// to set them up.
void SetupBlackAndWhiteStripedPalette() {
  // 'black out' all 16 palette entries...
  fill_solid(currentPalette, 16, CRGB::Black);
  // and set every fourth one to white.
  currentPalette[0] = CRGB::White;
  currentPalette[4] = CRGB::White;
  currentPalette[8] = CRGB::White;
  currentPalette[12] = CRGB::White;
}

// This function sets up a palette of purple and green stripes.
void SetupPurpleAndGreenPalette() {
  CRGB purple = CHSV(HUE_PURPLE, 255, 255);
  CRGB green = CHSV(HUE_GREEN, 255, 255);
  CRGB black = CRGB::Black;

  currentPalette = CRGBPalette16(
    green, green, black, black,
    purple, purple, black, black,
    green, green, black, black,
    purple, purple, black, black);
}


// This example shows how to set up a static color palette
// which is stored in PROGMEM (flash), which is almost always more
// plentiful than RAM.  A static PROGMEM palette like this
// takes up 64 bytes of flash.
const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM = {
  CRGB::Red,
  CRGB::Gray,  // 'white' is too bright compared to red and blue
  CRGB::Blue,
  CRGB::Black,

  CRGB::Red,
  CRGB::Gray,
  CRGB::Blue,
  CRGB::Black,

  CRGB::Red,
  CRGB::Red,
  CRGB::Gray,
  CRGB::Gray,
  CRGB::Blue,
  CRGB::Blue,
  CRGB::Black,
  CRGB::Black
};