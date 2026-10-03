# ⚡ Lightning Engine

## What is it?

The Lightning Engine is a storm simulator for WS2812B addressable LED strips.

The central idea was simple:

> We don't want to make LEDs blink. We want to make a storm feel alive.

For that reason, the system models **time**, **intensity**, **storm development**, and the **personality of each strike**.

## Validated hardware

- Arduino Uno
- 2 × WS2812B, 60 LEDs each
- 120 LEDs total
- D7 → strip A
- D8 → strip B
- 12 V / 3.33 A power supply
- LM2596S adjusted to approximately 5.02 V
- Strips installed approximately 2.8 m above the ground
- Cotton/fiber filling used as a diffuser to form the cloud

The two strips are treated as independent entities. This allows branches to be created and a discharge to move from one logical region to another.

## How the storm works

The storm has states:

```
SILENCE
   ↓
RISING
   ↓
DEVELOPING
   ↓
PEAK
   ↓
DECAYING
   ↓
DYING
   ↓
SILENCE
```

The **SILENCE** state is intentional.

A storm that flashes continuously stops looking like a storm and starts looking like a lighting effect.

## Every lightning strike has a personality

A discharge can vary in:

- intensity;
- duration;
- propagation time;
- direction;
- branching;
- persistence;
- intensity curve;
- number of peaks.

The curves used include:

- **Explosive** — starts strong and fades;
- **Growing** — progressively increases;
- **Peaked** — grows, reaches a peak, and decays;
- **Multi-peak** — contains more than one peak.

For strong storms, the distribution used was:

- 18% explosive;
- 14% growing;
- 44% peaked;
- 24% multi-peak.

## Propagation

Several timings between successive points of a discharge were tested:

| Propagation | Observed result |
|---:|---|
| 35/25 ms | subtle |
| 10 ms | flash |
| 5 ms | very good |
| 0 ms | intense |
| random 0–10 ms | validated |

Random selection between 0 and 10 ms helped prevent every discharge from behaving exactly the same way.

## Power

The theoretical limit for 120 LEDs at 60 mA in full white would be:

`120 × 60 mA = 7.2 A`

This exceeds the capacity of the power system used.

Therefore, the firmware uses:

- sparse effects;
- variable brightness;
- a power limit;
- localized flashes.

The software limit used by the firmware is approximately **2400 mA**, equivalent to about 40 LEDs at full white.

### Power supply

```
12 V / 3.33 A
       │
       ▼
    LM2596S
       │
     5.02 V
       │
       ├── Arduino Uno 5V
       │
       └── VCC of both WS2812B strips

Common GND between Arduino and strips
```

**Important:** the regulated 5 V is applied to the Uno's 5V pin, not to the barrel/DC jack.

## Real-world result

The system was installed inside a scenic cloud and operated during Arthuween 2026.

The test was no longer limited to the bench: it was validated in a real environment, in front of an audience.

Reported result:

**more than 25,000 CPM.**

## What we learned

### 1. The diffuser matters

The distance between the LEDs and the diffuser strongly affects how the emitted light overlaps.

Cotton/fiber filling worked very well.

### 2. Randomness needs structure

Pure randomness does not necessarily produce a convincing phenomenon.

The system uses randomness within rules: storm state, intensity, curves, intervals, and branching probability.

### 3. Silence is part of the effect

This was perhaps one of the most important discoveries.

The absence of lightning also communicates that a storm is happening.

## Code

The version registered in this repository is:

`Lightning_Simulator_v0_3_Tempestades_Intensas.ino`

It preserves the version containing the strong-storm behavior used during the final tests.

---

### 🤖 Human Designed. AI Assisted.

This project was designed and developed by humans with the assistance of artificial intelligence. Technical decisions, experimentation, validation and final implementation remained under human direction.
