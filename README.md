# ⚡ Arduino Reaction Time Challenge ⏱️

![Arduino](https://img.shields.io/badge/-Arduino-00979D?style=for-the-badge&logo=Arduino&logoColor=white)
![C++](https://img.shields.io/badge/C++-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white)

A hardware-based reaction time testing game built with Arduino. Test your reflexes, beat your high scores, and see if you have the fastest hands!

## 🚀 Overview
This project is an interactive reaction time game where users wait for an LED/Buzzer signal and trigger an IR Sensor as fast as possible. It features multiple difficulty levels, cheat detection (reacting too fast), and persistent high-score tracking using the Arduino's built-in EEPROM.

### ✨ Key Features (Showcasing Embedded Skills)
- **Non-Volatile Memory (EEPROM):** Saves the Top 3 best times and the "All-Time Best" score. The data persists even when the Arduino is powered off.
- **Difficulty Scaling:** 3 modes (Easy, Medium, Hard) that dynamically adjust delay times and sensory cues (visual vs. audio cues).
- **Anti-Cheat Logic:** Ignores inputs that are faster than humanly possible (< 50ms) to prevent false triggers or button mashing.
- **I2C LCD Integration:** Real-time feedback and menu navigation using an optimized I2C 16x2 LCD interface.
- **Hardware State Management:** Custom game loop logic handling different states (Waiting, Playing, Showing Stats).

## 🛠️ Hardware Requirements
- 1x Arduino Uno (or compatible board)
- 1x IR Proximity Sensor (Pin 7)
- 1x 16x2 LCD with I2C module (Address 0x3F)
- 4x Push Buttons (Start, Stats, Reset, Continue/Difficulty)
- 1x LED (Pin 6)
- 1x Active/Passive Buzzer (Pin 5)
- Jumper wires & Breadboard

## 🎮 How to Play
1. **Select Difficulty:** Press the `Continue` button to cycle through Easy, Medium, and Hard.
2. **Start Game:** Press the `Start` button and wait for the "GO!" signal (LED turns on + Buzzer beeps).
3. **React!** Wave your hand over the IR sensor as fast as you can.
4. **Check Stats:** Press the `Stats` button anytime on the main screen to see the top scores saved in the memory.
5. **Reset:** Press the `Reset` button to wipe the EEPROM clean and start fresh.

## 🧠 Code Highlights
The score-saving mechanism uses a sorting algorithm before committing to EEPROM to minimize write cycles and ensure the leaderboard is always accurate:

```cpp
// Smart saving: Only updates EEPROM if the new time beats an existing high score
if (newTime >= TOO_FAST_LIMIT && newTime <= TIME_LIMIT) {
    // Logic to shift arrays and save to EEPROM...
}
