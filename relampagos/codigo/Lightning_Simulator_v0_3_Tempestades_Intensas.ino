#include <Adafruit_NeoPixel.h>

/*
  LIGHTNING SIMULATOR v0.1
  Arduino Uno
  Fita A = D7 / 60 LEDs
  Fita B = D8 / 60 LEDs
  Switch = D2 -> GND (INPUT_PULLUP)

  Autonomous storm. No audio.
  Non-blocking millis().
  Logical paths are separated from physical LED mapping.
  Physical animation direction: LED 60 -> LED 01.
  Lightning color: cool blue-white -> neutral white at peak.
*/

#define PIN_A 7
#define PIN_B 8
#define SWITCH_PIN 2
#define NUM_LEDS 60

// Conservative software ceiling for the LM2596S.
// 2400 mA ~= 40 full-white LEDs at 60 mA each.
#define POWER_BUDGET_MA 2400
#define LED_FULL_MA 60

Adafruit_NeoPixel A(NUM_LEDS, PIN_A, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel B(NUM_LEDS, PIN_B, NEO_GRB + NEO_KHZ800);


// ============================================================
// LOGICAL TOPOLOGY — PROVISIONAL
// Change ONLY these values after the physical intersection is fixed.
//
// Each path is an ordered sequence. The engine never needs to know
// whether it is A or B beyond this mapping.
//
// Physical animation direction is now reversed relative to the original POC.
// Each path is defined in the direction in which the visual effect should travel.
// A1: LED 39 -> 0
// A2: LED 40 -> 59  (toward the node)
// B1: LED 39 -> 0
// B2: LED 40 -> 59  (toward the node)
// ============================================================

enum Path : byte { PATH_A1, PATH_A2, PATH_B1, PATH_B2, PATHS };

struct PathMap {
  byte strip;       // 0=A, 1=B
  int start;
  int end;
};

PathMap mapPath[PATHS] = {
  {0, 39,  0},
  {0, 40, 59},
  {1, 39,  0},
  {1, 40, 59}
};

int pathLen(Path p) {
  return abs(mapPath[p].end - mapPath[p].start) + 1;
}

int physicalLED(Path p, int logicalPos) {
  logicalPos = constrain(logicalPos, 0, pathLen(p) - 1);

  if (mapPath[p].start <= mapPath[p].end)
    return mapPath[p].start + logicalPos;

  return mapPath[p].start - logicalPos;
}


enum LightningCurve : byte {
  CURVE_EXPLOSIVE,
  CURVE_GROWING,
  CURVE_PEAKED,
  CURVE_MULTI_PEAK
};

// ============================================================
// FRAME BUFFER
// ============================================================

uint16_t frameAR[NUM_LEDS];
uint16_t frameAG[NUM_LEDS];
uint16_t frameAB[NUM_LEDS];
uint16_t frameBR[NUM_LEDS];
uint16_t frameBG[NUM_LEDS];
uint16_t frameBB[NUM_LEDS];

void clearFrame() {
  for (int i = 0; i < NUM_LEDS; i++) {
    frameAR[i] = 0;
    frameAG[i] = 0;
    frameAB[i] = 0;
    frameBR[i] = 0;
    frameBG[i] = 0;
    frameBB[i] = 0;
  }
}

void addEnergy(Path p, int pos, int brightness, byte r, byte g, byte b) {
  if (brightness <= 0) return;

  int led = physicalLED(p, pos);
  brightness = constrain(brightness, 0, 255);

  if (mapPath[p].strip == 0) {
    frameAR[led] += (uint16_t)r * brightness / 255;
    frameAG[led] += (uint16_t)g * brightness / 255;
    frameAB[led] += (uint16_t)b * brightness / 255;
  }
  else {
    frameBR[led] += (uint16_t)r * brightness / 255;
    frameBG[led] += (uint16_t)g * brightness / 255;
    frameBB[led] += (uint16_t)b * brightness / 255;
  }
}


// ============================================================
// STORM PROFILE
// ============================================================

enum StormState {
  STORM_SILENCE,
  STORM_RISING,
  STORM_DEVELOPING,
  STORM_PEAK,
  STORM_DECAYING,
  STORM_DYING
};

StormState stormState = STORM_SILENCE;

struct StormProfile {
  unsigned long rise;
  unsigned long develop;
  unsigned long peak;
  unsigned long decay;
  unsigned long dying;

  float maxIntensity;
  float density;
  float branchProbability;
  float multiPeakProbability;
  byte baseBrightness;
};

StormProfile storm;
bool stormStrong = false;

bool stormActive = false;
unsigned long stormStart = 0;
unsigned long nextStrike = 0;

float stormIntensity(unsigned long now) {
  if (!stormActive) return 0.0;

  unsigned long t = now - stormStart;

  if (t < storm.rise)
    return storm.maxIntensity * (float)t / storm.rise;

  t -= storm.rise;

  if (t < storm.develop)
    return storm.maxIntensity *
           (0.55 + 0.45 * (float)t / storm.develop);

  t -= storm.develop;

  if (t < storm.peak)
    return storm.maxIntensity;

  t -= storm.peak;

  if (t < storm.decay)
    return storm.maxIntensity *
           (1.0 - 0.75 * (float)t / storm.decay);

  t -= storm.decay;

  if (t < storm.dying)
    return storm.maxIntensity *
           0.25 * (1.0 - (float)t / storm.dying);

  return 0.0;
}

void createStorm(bool strong) {
  stormStrong = strong;

  if (strong) {
    storm.rise = random(5000, 12000);
    storm.develop = random(6000, 18000);
    storm.peak = random(5000, 11000);
    storm.decay = random(7000, 16000);
    storm.dying = random(2500, 7000);

    storm.maxIntensity = random(78, 101) / 100.0;
    storm.density = random(70, 99) / 100.0;
    storm.branchProbability = random(42, 79) / 100.0;
    storm.multiPeakProbability = random(20, 50) / 100.0;
    storm.baseBrightness = random(155, 216);
  }
  else {
    storm.rise = random(7000, 16000);
    storm.develop = random(8000, 22000);
    storm.peak = random(3000, 7000);
    storm.decay = random(9000, 22000);
    storm.dying = random(4000, 9000);

    storm.maxIntensity = random(32, 63) / 100.0;
    storm.density = random(20, 49) / 100.0;
    storm.branchProbability = random(8, 29) / 100.0;
    storm.multiPeakProbability = random(3, 15) / 100.0;
    storm.baseBrightness = random(75, 136);
  }
}

void startStorm(unsigned long now) {
  createStorm(digitalRead(SWITCH_PIN) == LOW);
  stormStart = now;
  stormActive = true;
  stormState = STORM_RISING;

  // Commissioning rule: every new storm MUST produce a first strike.
  // After that, the scheduler becomes probabilistic again.
  nextStrike = now + random(700, 1600);
}

void updateStorm(unsigned long now) {
  if (!stormActive) return;

  unsigned long t = now - stormStart;

  if (t < storm.rise) stormState = STORM_RISING;
  else if (t < storm.rise + storm.develop) stormState = STORM_DEVELOPING;
  else if (t < storm.rise + storm.develop + storm.peak) stormState = STORM_PEAK;
  else if (t < storm.rise + storm.develop + storm.peak + storm.decay)
    stormState = STORM_DECAYING;
  else if (t < storm.rise + storm.develop + storm.peak +
                  storm.decay + storm.dying)
    stormState = STORM_DYING;
  else {
    stormActive = false;
    stormState = STORM_SILENCE;
    nextStrike = now + random(3000, 10000);
  }
}


// ============================================================
// LIGHTNING PROFILE
// ============================================================

struct Lightning {
  bool active;

  Path p1;
  Path p2;

  int pos1;
  int pos2;
  int dir1;
  int dir2;

  unsigned long started;
  unsigned long lastMove;

  unsigned int duration;
  unsigned int propagationDelay;

  byte peakBrightness;
  byte branchBrightness;

  byte curve;
  bool branching;
  bool branchStarted;
};

Lightning bolt;

float envelope(unsigned long age, unsigned long duration, byte c) {
  if (duration == 0 || age >= duration) return 0;

  float x = (float)age / duration;

  if (c == CURVE_EXPLOSIVE) return 1.0 - x;
  if (c == CURVE_GROWING) return x;

  if (c == CURVE_PEAKED) {
    if (x < 0.30) return x / 0.30;
    return 1.0 - (x - 0.30) / 0.70;
  }

  float a = 1.0 - abs(x - 0.28) / 0.28;
  float b = 1.0 - abs(x - 0.63) / 0.24;
  if (a < 0) a = 0;
  if (b < 0) b = 0;
  return max(a, b);
}

bool chance(float p) {
  return random(0, 10000) < (int)(p * 10000.0);
}

void lightningColor(int brightness, byte &r, byte &g, byte &b) {
  // Lightning is predominantly white. At lower energy levels it gets
  // a cool blue tint; as it peaks it approaches neutral white.
  float t = constrain(brightness, 0, 255) / 255.0;
  r = 110 + 145 * t;
  g = 165 + 90 * t;
  b = 255;
}

void createBolt(unsigned long now) {
  bolt.active = true;
  bolt.p1 = (Path)random(0, PATHS);
  bolt.p2 = (Path)random(0, PATHS);

  while (bolt.p2 == bolt.p1)
    bolt.p2 = (Path)random(0, PATHS);

  bolt.pos1 = random(2, pathLen(bolt.p1) - 2);
  bolt.pos2 = 0;

  bolt.dir1 = random(0, 2) ? 1 : -1;
  bolt.dir2 = random(0, 2) ? 1 : -1;

  float si = stormIntensity(now);

  bolt.duration = random(120, 320) + si * 220;
  bolt.propagationDelay = random(0, 11);   // validated range: 0..10 ms

  // Strong discharges are deliberately more common.
  // They are still random, but no longer restricted to the tiny storm peak.
  bool heavyStrike = chance(stormStrong ? 0.45 : 0.16);

  float energy = 0.45 + si * 0.55;
  int normalBoost = stormStrong ? random(45, 91) : random(20, 61);
  int heavyBoost  = stormStrong ? random(80, 131) : random(45, 91);

  int targetBrightness = storm.baseBrightness +
                          energy * (heavyStrike ? heavyBoost : normalBoost) +
                          random(-15, 21);

  bolt.peakBrightness = constrain(targetBrightness,
                                  heavyStrike ? 170 : 80,
                                  255);

  bolt.branchBrightness = bolt.peakBrightness * random(48, 84) / 100;

  int r = random(0, 100);
  if (stormStrong) {
    if (r < 18) bolt.curve = CURVE_EXPLOSIVE;
    else if (r < 32) bolt.curve = CURVE_GROWING;
    else if (r < 76) bolt.curve = CURVE_PEAKED;
    else bolt.curve = CURVE_MULTI_PEAK;
  }
  else {
    if (r < 25) bolt.curve = CURVE_EXPLOSIVE;
    else if (r < 45) bolt.curve = CURVE_GROWING;
    else if (r < 85) bolt.curve = CURVE_PEAKED;
    else bolt.curve = CURVE_MULTI_PEAK;
  }

  bolt.branching =
    chance(storm.branchProbability * (0.45 + si));

  bolt.branchStarted = false;
  bolt.started = now;
  bolt.lastMove = now;
}


// ============================================================
// LIGHTNING UPDATE + RENDER
// ============================================================

void scheduleNext(unsigned long now); // forward declaration for Arduino/C++

void updateBolt(unsigned long now) {
  if (!bolt.active) return;

  unsigned long age = now - bolt.started;

  if (now - bolt.lastMove >= bolt.propagationDelay) {
    bolt.lastMove = now;

    bolt.pos1 += bolt.dir1;

    if (bolt.pos1 <= 0) {
      bolt.pos1 = 1;
      bolt.dir1 = 1;
    }
    if (bolt.pos1 >= pathLen(bolt.p1) - 1) {
      bolt.pos1 = pathLen(bolt.p1) - 2;
      bolt.dir1 = -1;
    }

    if (bolt.branching && age >= bolt.duration / 3) {
      bolt.branchStarted = true;
      bolt.pos2 += bolt.dir2;

      if (bolt.pos2 >= pathLen(bolt.p2) - 1)
        bolt.pos2 = pathLen(bolt.p2) - 2;
    }
  }

  if (age >= bolt.duration) {
    bolt.active = false;
    scheduleNext(now);
  }
}

void renderBolt(unsigned long now) {
  if (!bolt.active) return;

  unsigned long age = now - bolt.started;

  float e = envelope(age, bolt.duration, bolt.curve);
  float si = stormIntensity(now);

  int br = bolt.peakBrightness * e * (0.55 + si * 0.45);
  br = constrain(br, 0, 255);

  // Never let the commissioning flash become too weak to see.
  if (br > 0 && br < 20) br = 20;

  byte cr, cg, cb;
  lightningColor(br, cr, cg, cb);

  addEnergy(bolt.p1, bolt.pos1, br, cr, cg, cb);
  addEnergy(bolt.p1, bolt.pos1 - 1, br * 45 / 100, cr, cg, cb);
  addEnergy(bolt.p1, bolt.pos1 + 1, br * 45 / 100, cr, cg, cb);

  if (bolt.branchStarted) {
    int branch = br * bolt.branchBrightness /
                 max((int)1, (int)bolt.peakBrightness);

    byte brR, brG, brB;
    lightningColor(branch, brR, brG, brB);

    addEnergy(bolt.p2, bolt.pos2, branch, brR, brG, brB);
    addEnergy(bolt.p2, bolt.pos2 - 1, branch * 40 / 100, brR, brG, brB);
    addEnergy(bolt.p2, bolt.pos2 + 1, branch * 40 / 100, brR, brG, brB);
  }

  // Very short secondary flash for multi-peak discharges.
  if (bolt.curve == CURVE_MULTI_PEAK &&
      ((age >= bolt.duration * 0.24 && age < bolt.duration * 0.24 + 12) ||
       (age >= bolt.duration * 0.60 && age < bolt.duration * 0.60 + 12))) {

    int flash = min(255, br + 80);

    for (int i = 0; i < NUM_LEDS; i++) {
      if (random(0, 100) < 60) {
        frameAR[i] += flash;
        frameAG[i] += flash;
        frameAB[i] += flash;
        frameBR[i] += flash;
        frameBG[i] += flash;
        frameBB[i] += flash;
      }
    }
  }
}


// ============================================================
// POWER LIMITER + OUTPUT
// ============================================================

void showFrame() {
  unsigned long requested = 0;

  for (int i = 0; i < NUM_LEDS; i++) {
    int ar = min(frameAR[i], (uint16_t)255);
    int ag = min(frameAG[i], (uint16_t)255);
    int ab = min(frameAB[i], (uint16_t)255);
    int br = min(frameBR[i], (uint16_t)255);
    int bg = min(frameBG[i], (uint16_t)255);
    int bb = min(frameBB[i], (uint16_t)255);

    // Approximate WS2812 current using the brightest channel mix.
    requested += (unsigned long)max(ar, max(ag, ab)) * LED_FULL_MA / 255;
    requested += (unsigned long)max(br, max(bg, bb)) * LED_FULL_MA / 255;
  }

  float scale = 1.0;

  if (requested > POWER_BUDGET_MA)
    scale = (float)POWER_BUDGET_MA / requested;

  for (int i = 0; i < NUM_LEDS; i++) {
    int ar = constrain(frameAR[i] * scale, 0, 255);
    int ag = constrain(frameAG[i] * scale, 0, 255);
    int ab = constrain(frameAB[i] * scale, 0, 255);
    int br = constrain(frameBR[i] * scale, 0, 255);
    int bg = constrain(frameBG[i] * scale, 0, 255);
    int bb = constrain(frameBB[i] * scale, 0, 255);

    A.setPixelColor(i, A.Color(ar, ag, ab));
    B.setPixelColor(i, B.Color(br, bg, bb));
  }

  A.show();
  B.show();
}

// ============================================================
// SCHEDULER
// ============================================================

void scheduleNext(unsigned long now) {
  float si = stormIntensity(now);

  int minGap;
  int maxGap;

  if (stormStrong) {
    // More activity, but still with breathing spaces between discharges.
    minGap = 650 - si * 350;
    maxGap = 2200 - si * 1300;
  }
  else {
    minGap = 1400 - si * 850;
    maxGap = 3600 - si * 2300;
  }

  minGap = max(minGap, 250);
  maxGap = max(maxGap, minGap + 300);

  nextStrike = now + random(minGap, maxGap);
}

void scheduler(unsigned long now) {
  if (!stormActive || bolt.active) return;
  if (now < nextStrike) return;

  // Never let a storm remain visually dead forever.
  // The first strike is guaranteed; later strikes are probabilistic.
  createBolt(now);

  // The next interval is decided only after this strike finishes.
}


// ============================================================
// SETUP / LOOP
// ============================================================

void setup() {
  A.begin();
  B.begin();

  A.clear();
  B.clear();
  A.show();
  B.show();

  pinMode(SWITCH_PIN, INPUT_PULLUP);

  randomSeed(analogRead(A0));

  bolt.active = false;

  // Atmospheric initial silence.
  nextStrike = millis() + random(2500, 6000);
}

void loop() {
  unsigned long now = millis();

  if (!stormActive && !bolt.active && now >= nextStrike)
    startStorm(now);

  updateStorm(now);

  if (bolt.active)
    updateBolt(now);
  else
    scheduler(now);

  clearFrame();

  if (bolt.active)
    renderBolt(now);

  showFrame();
}