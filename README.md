# 🎃 Special Effects for Halloween Parties

Electronic projects developed and tested to create Halloween special effects using accessible hardware, procedural software, and plenty of experimentation.

This repository documents part of the work carried out for **Arthuween 2026**, including two main experiences:

- ⚡ **Lightning Engine** — a lightning storm using Arduino Uno and WS2812B LED strips.
- 💀 **Maria Joaquina** — a scenic skull with animated eyes using an ESP32-S3 and GC9A01 displays.

The goal is not simply to publish code. The documentation also records ideas, decisions, tests, problems encountered, and versions that were considered valid.

## ⚡ Lightning Engine

The lightning machine was designed as an **autonomous procedural storm**, rather than a simple blinking-light effect.

It works with:

- 2 Arduino Uno boards;
- 2 WS2812B strips with 60 LEDs each;
- 120 LEDs in total;
- D7 for strip A and D8 for strip B;
- 12 V / 3.33 A power supply;
- LM2596S adjusted to approximately 5.02 V;
- logical paths separated from the physical LED positions;
- intensity, duration, propagation, branching, and multiple peaks;
- storm lifecycle: `SILENCE → RISING → DEVELOPING → PEAK → DECAYING → DYING`.

An important detail is that **silence is also part of the effect**. A convincing storm should not produce lightning continuously.

During Arthuween 2026, the system was used inside a scenic cloud and exceeded **25,000 CPM (Caralhos Por Minuto)**, according to the report after the event.

➡️ [Lightning Engine documentation](relampagos/README.md)

## 💀 Maria Joaquina — Skull Eyes

The skull received two approximately 1.28" circular GC9A01 displays controlled by an ESP32-S3.

The eyes are built **procedurally**. They are not animations made from a sequence of images.

The visual composition consists of:

- sclera;
- outer ring;
- iris;
- pupil.

During movement, the unit that moves is the **entire iris** — ring + iris + pupil. The pupil remains centered within the iris.

The functional reference version is:

**POC-KAME-EYE-002C — IRIS GAZE + NUMB / LIFELESS**

This version is kept as the frozen baseline because it was the last version proven to work with both eyes.

➡️ [Skull eyes documentation](olhos-caveira/README.md)

## 🧪 How the project was developed

The process was deliberately experimental:

`POC → visual test → technical validation → assembly → real-world event → human reaction → experience validation`

Validated versions should not be modified directly. Any change should result in a new version.

## 🏠 Arthuween 2026

In addition to the two electronic systems, the scenery used looping projections of ghosts, zombies, and other supernatural content in a window facing the side corridor. The corridor lights were turned off to improve contrast.

The result combined:

**lightning + Maria Joaquina + projections + darkness + surprise + procedural behavior.**

## 📚 Structure

```
relampagos/
  README.md
  codigo/
    Lightning_Simulator_v0_3_Tempestades_Intensas.ino

olhos-caveira/
  README.md
  codigo/
    POC-KAME-EYE-002C_IrisGaze_Numb_Lifeless.ino

docs/
  ARTHUWEEN-2026.md
```

## ⚠️ Notes

The code files are records of development and validation versions of a scenic project. Components, power supply, current requirements, and physical construction must be properly sized and verified before reproducing any part of the project.

## 🎃 Result

Arthuween evolved from a collection of POCs into a complete experiment involving **engineering, scenery, software, hardware, procedural behavior, and human experience**.

---

### 🤖 Human Designed. AI Assisted.

This project was designed and developed by humans with the assistance of artificial intelligence. Technical decisions, experimentation, validation and final implementation remained under human direction.
