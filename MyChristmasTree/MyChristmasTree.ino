#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif
#define PIN        6
#define NUMPIXELS 17

typedef uint32_t color_t;

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_RGB + NEO_KHZ800);
#define DELAYVAL 700

#define NUM_STEPS 100
#define STEP_DELAY 5
#define CYCLE_DELAY 300


#define FASTDELAYVAL 150

const uint8_t starLedAddr {12};

uint8_t treeLedAddrArray[] {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 14, 15, 16};
const uint8_t NUM_TREE_LEDS {static_cast<uint8_t>(sizeof(treeLedAddrArray))};

color_t starLedColorPrev {};
color_t starLedColorNow {};
color_t starLedColorNext {};
color_t treeLedColorPrev[NUM_TREE_LEDS] {};
color_t treeLedColorNow[NUM_TREE_LEDS] {};
color_t treeLedColorNext[NUM_TREE_LEDS] {};

void colorToRgb(uint8_t* rgb_arr, const color_t color) {
  *(rgb_arr + 2) = ((uint8_t)(color & 0xFF));
  *(rgb_arr + 1) = ((uint8_t)((color >> 8) & 0xFF));
  *(rgb_arr + 0) = ((uint8_t)((color >> 16) & 0xFF));
}

color_t rgbToColor(const uint8_t r, const uint8_t g, const uint8_t b) {
  return ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;
}

const auto TREE_OFF {pixels.Color(0, 0, 0)};
const auto TREE_GREEN {pixels.Color(0, 120, 0)};
const auto TREE_RED {pixels.Color(120, 0, 0)};

const auto STAR_OFF {TREE_OFF};
const auto STAR_GOLD {pixels.Color(255, 255, 0)};
const auto STAR_DIM {pixels.Color(180, 120, 0)};

void paintTree(color_t color) {
  for (uint8_t idx {}; idx < NUM_TREE_LEDS; idx++) {
    treeLedColorNext[idx] = color;
  }
}

void initTreeAnimate() {
for (uint8_t idx {}; idx < NUM_TREE_LEDS; idx++) {
    treeLedColorPrev[idx] = TREE_GREEN;
    treeLedColorNext[idx] = TREE_GREEN;
  }

  starLedColorNext = STAR_GOLD;
  starLedColorPrev = STAR_GOLD;
  renderTree();
}

void shuffleTreeAddrArray() {
  const size_t count {NUM_TREE_LEDS / sizeof(treeLedAddrArray[0])};
  for (uint8_t idx{}; idx < count; idx++) {
    size_t idxToShuffle = random(0, count - idx);
    auto temp {treeLedAddrArray[idx]};
    treeLedAddrArray[idx] = treeLedAddrArray[idxToShuffle];
    treeLedAddrArray[idxToShuffle] = temp;
  }
}

void renderTree() {
  pixels.setPixelColor(starLedAddr, starLedColorNow);          
  for (uint8_t idx{}; idx < NUM_TREE_LEDS; idx++) {
    pixels.setPixelColor(treeLedAddrArray[idx], treeLedColorNow[idx]);      
  }
  pixels.show();
}

static const uint8_t NUM_REDS {4};
uint8_t redIdxsArray[NUM_REDS] {};

void randInitReds() {
  for (uint8_t count {}; count < NUM_REDS; count++) {
    const uint8_t randUpdateLedIdx {static_cast<uint8_t>(random(NUM_TREE_LEDS))};
    redIdxsArray[count] = randUpdateLedIdx;
    treeLedColorNext[randUpdateLedIdx] = TREE_RED;
    treeLedColorPrev[randUpdateLedIdx] = TREE_RED;
  }
}

void setup() {
  pinMode(13, OUTPUT);

#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif

  pixels.begin();
  delay(DELAYVAL);
  pixels.clear();
  pixels.show();
  delay(DELAYVAL);

  shuffleTreeAddrArray();
  initTreeAnimate();
}

// interpolate from one uint8_t to another uint8_t. frac \in [0, 1]
inline uint8_t interpolateU8(uint8_t from_val, uint8_t to_val, float frac) {
  return (uint8_t)((1-frac) * ((float)from_val) + (frac * (float)to_val));
}

// interpolate from one color_t to another color_t. frac \in [0, 1]
color_t interpolateColor(color_t from_color, color_t to_color, float frac) {
  uint8_t from_rgb[3] {0, 120, 0};
  colorToRgb(from_rgb, from_color);

  uint8_t to_rgb[3] {0, 0, 120};
  colorToRgb(to_rgb, to_color);

  uint8_t result_rgb[3];
  for (uint8_t idx{}; idx < 3; idx++) {
    result_rgb[idx] = interpolateU8(from_rgb[idx], to_rgb[idx], frac);
  }
  return rgbToColor(result_rgb[0], result_rgb[1], result_rgb[2]);
}

int step_idx {};

// interpolate from Prev state to Next state. uses step index to compute frac
void interpTree() {
  float step_frac {((float)step_idx)/(NUM_STEPS-1)};
  starLedColorNow = interpolateColor(starLedColorPrev, starLedColorNext, step_frac);
  for (uint8_t idx{}; idx < NUM_TREE_LEDS; idx++) {
    treeLedColorNow[idx] = interpolateColor(treeLedColorPrev[idx], treeLedColorNext[idx], step_frac);
  }
}

void loop() {

  // Transition to "Next" state complete. Update "Next" state
  if (step_idx >= NUM_STEPS) {
    starLedColorNext = (starLedColorPrev == STAR_GOLD) ? STAR_DIM : STAR_GOLD;

    // Select one of the red LEDs to start transitioning to green
    static uint8_t redLedUpdateIdx {};
    redLedUpdateIdx++;
    if (redLedUpdateIdx >= NUM_REDS) {
      redLedUpdateIdx = 0;
    }

    // select an LED on the tree to update
    static uint8_t treeLedUpdateIdx {};
    treeLedUpdateIdx++;
    if (treeLedUpdateIdx >= NUM_TREE_LEDS) {
      treeLedUpdateIdx = 0;
      shuffleTreeAddrArray();
    }

    // 
    treeLedColorNext[treeLedUpdateIdx] = (treeLedColorPrev[treeLedUpdateIdx] == TREE_GREEN) ? TREE_RED : TREE_GREEN;
    treeLedColorNext[redIdxsArray[redLedUpdateIdx]] = (treeLedColorPrev[redIdxsArray[redLedUpdateIdx]] == TREE_GREEN) ? TREE_RED : TREE_GREEN;
    redIdxsArray[redLedUpdateIdx] = treeLedUpdateIdx;
    step_idx = 0;

    // Prev = Now
    starLedColorPrev = starLedColorNow;
    for (uint8_t idx{}; idx < NUM_TREE_LEDS; idx++) {
      treeLedColorPrev[idx] = treeLedColorNow[idx];
    }

    // wait a bit longer at this point before updating to show the final state
    delay(CYCLE_DELAY);
  }

  // interpolate between Prev and Next states
  interpTree();

  // set LEDs the Now state and display
  renderTree(); 
  pixels.show();

  step_idx++;
  delay(STEP_DELAY);
}