# 💀 Maria Joaquina — Skull Eyes

## What is it?

Maria Joaquina is the scenic skull from Arthuween 2026.

Its eyes were built with:

- ESP32-S3 DevKit;
- 2 × circular GC9A01 displays;
- approximately 1.28";
- 240 × 240 resolution;
- displays installed behind dark glasses.

The glasses hide the boards and wires, so what the audience sees is simply a pair of eyes inside the skull.

## The main idea

The eyes are not videos.

They are **generated in real time**.

Each eye is composed of circles:

```
┌─────────────────────────┐
│                         │
│       SCLERA            │
│          ●              │
│       IRIS +            │
│       PUPIL             │
│                         │
└─────────────────────────┘
```

The moving unit is the **entire iris**:

**outer ring + iris + pupil**

The pupil remains centered relative to the iris.

This produces a much simpler and more coherent visual movement than trying to move each element independently.

## Frozen baseline

The functional reference is:

**POC-KAME-EYE-002C — IRIS GAZE + NUMB / LIFELESS**

Validated parameters:

| Parameter | Value |
|---|---:|
| Center X | 120 |
| Center Y | 120 |
| Sclera | 105 |
| Outer ring | 65 |
| Iris | 55 |
| Pupil | 25 |
| Buffer | 201 × 201 |
| Buffer X | 20 |
| Buffer Y | 20 |

Gaze positions:

- left = 90
- center = 120
- right = 150
- up = 90
- down = 150

The framebuffer uses RGB565 and is sent to both displays with `pushImage()`.

## How the movement works

The movement does not jump from one position to another.

The code uses a smoothing curve:

```
t² × (3 - 2t)
```

This produces smooth ease-in/ease-out movement.

The normal sequence includes:

- center → up → center;
- center → down → center;
- center → left → center;
- center → right → center;
- center → upper-left → center;
- center → lower-right → center.

## NUMB / LIFELESS

From time to time, the eye enters a different state.

In this state:

- the iris remains visible;
- the gaze becomes slow/minimal;
- the pupil becomes light gray;
- after a few seconds, the eye returns to normal behavior.

In the 002C baseline:

- chance: 3%;
- duration: approximately 2.5–5 seconds.

During testing, 15% was also experimented with, but the frozen baseline remains at 3%.

## An important decision: autonomy

HC-SR04 sensors were considered to detect people and change the eyes' behavior.

That idea was removed from the Arthuween scope.

Maria Joaquina works **autonomously**.

The sensors remain available for future experiments.

## Approaches that were discarded

### Person tracking

Removed from the final project.

### Blink

Not used because the skull has no eyelids.

### Image-based animation

Rejected. The eye behavior should remain procedural.

### LGFX_Sprite

It was tested and caused `StoreProhibited`. It is not part of the validated solution.

### readRect()

It was tested, but the driver was configured with `readable=false`. It was not used as a framebuffer strategy.

### Redrawing the entire screen

This caused flicker during movement.

The validated solution uses a local framebuffer and `pushImage()`.

## A critical hardware detail

The two GC9A01 displays use shared CS on the SPI bus.

The validated 002C initialization sequence must be preserved.

The sequence is:

1. left CS LOW;
2. initialize through `rightDisplay.init()`;
3. left rotation = 0;
4. right rotation = 2;
5. draw on both displays;
6. set both CS pins HIGH.

**Do not change this sequence inside the frozen version.**

## The R8 resistor case — an important discovery

During the investigation of individual control of the two GC9A01 displays, we came very close to removing resistor **R8** from one of the boards to find out whether that would allow CS to be used conventionally, making each display completely independent on the SPI bus.

That was when an important piece of information emerged for anyone who encounters the same type of module.

R8 was identified on the board as:

- **R8-CS**;
- marking **513** (approximately 51 kΩ);
- a Chinese marking indicating a **pull-down resistor**;
- an indication that the module can operate without an external CS/RST connection.

In practice, this pull-down keeps **CS LOW** when the external pin is not being driven. Since GC9A01 CS is **active LOW**, the display remains selected by default.

One test was particularly revealing: with the physical CS wires disconnected, **both displays continued to receive initialization and appeared blue**. This showed that the behavior did not simply depend on the CS wire being connected to the ESP32 — the module itself was keeping CS LOW through R8.

The conclusion was important:

> **Removing R8 was not necessary to obtain individual display control.**

The validated approach was to keep the original hardware and control CS from the ESP32:

- CS LOW → display selected;
- CS HIGH → display deselected;
- during initialization, preserve the CS combination used by 002C;
- after initialization, control the displays individually.

Therefore, **R8 should not be removed as part of the validated solution**.

### Why document this?

Because removing the resistor is a very natural idea when two GC9A01 modules exhibit apparently strange CS behavior.

In this project, however, removing the component would have meant modifying the hardware before fully understanding the circuit.

The investigation showed that the pull-down was part of the module's behavior and that it was possible to work with it intact.

**Practical lesson:** before modifying the board, investigate the module's CS circuit and test its behavior with the GPIO explicitly driven LOW and HIGH.

## Result

Maria Joaquina was installed at Arthuween 2026 and was a big hit with the audience.

Validation was no longer limited to the technical side: the behavior was observed in a real scenic installation.

## Code

`POC-KAME-EYE-002C_IrisGaze_Numb_Lifeless.ino` is the versioned copy of the functional baseline code.

---

### 🤖 Human Designed. AI Assisted.

This project was designed and developed by humans with the assistance of artificial intelligence. Technical decisions, experimentation, validation and final implementation remained under human direction.
