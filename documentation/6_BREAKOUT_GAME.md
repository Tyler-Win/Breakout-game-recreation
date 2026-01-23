<br>

<div style="display: flex; align-items: center; gap: 20px;">
  <div style="font-size: 2em; font-weight: bold;">
  Using the SDK for BOOSTXL-EDUMKII
  </div>
  <div style="position: absolute; top: 20px; right: 50px;">
  <a href="../Readme.md" title="Go back" style="text-decoration: none; font-size: 32px;">↩</a>
  </div>  
</div>
<br>
<hr style="border: none; border-bottom: 4px solid #FFF; width: 100%; margin-top: 0;">


> Version 1.3  
> Date: Fall 2025

---

## ⌨️ Final Project by Tyler Huynh and Adnan Heganovic

The goal of this project was to create a recreation of the classic arcade game Breakout for the **BOOSTXL-EDUMKII BoosterPack**, designed for use with the **LP-MSPM0G3507 LaunchPad**. The creation of this software aims to show future students the possibility of creating games on this hardware and give inspiration to future final projects through implementation of this idea.

---

<br>

## 👾 What is Breakout?

Originally released May 13th, 1976 for arcade cabinets. Breakout features rows of bricks along the top of the screen, with the goal to crush them using a ball that is in constant motion across the screen. The player will need to utilize a paddle to hit the ball at the correct angle to get rid of all the bricks and get the highest score before losing too many lives.

### Key Features:
- Ball in constant motion
- Ball responds to hitting paddle, bricks, wall
- Ball speeds up and slows down based on where it hit the paddle
- Paddle moves left and right in response to Joystick
- PB1 and PB2 allow for pausing and playing
- Buzzer makes a chime when ball destroys brick 
- Player loses a life when ball touches the bottom of the screen.
- RGB LED signals players remaining lives
- Game over screen when player loses all three lives

Implementing this software onto the MSPM0G3507 allows for future student projects to follow in it's lead.

🔗 [More details on Wikipedia](https://en.wikipedia.org/wiki/Breakout_(video_game))

---

<br>

## 🙏 Acknowledgments

- **Ryanne Geiger** - Use of their SDK for the BOOSTXL-EDUMKII was invaluable
- **Eli (T.A.)** - Helped to find realistic ways to implement some key features
- **Professor Bruce Link** – Support and help throughout the entire process

------

<br>

##  🧱 Module Overview

### 🧩 Accelerometer  
3-axis accelerometer connected via ADC:

| Axis | ADC Module | Channel |
|------|------------|---------|
| X    | ADC1       | CH6     |
| Y    | ADC0       | CH7     |
| Z    | ADC1       | CH5     |

---

### 🧩 Microphone  

- Connected to **ADC0**, **Channel 5**

---

### 🧩 Joystick  

| Function  | ADC Module | Channel |
|-----------|------------|---------|
| X-Axis    | ADC0       | CH2     |
| Y-Axis    | ADC1       | CH1     |
| Select    | GPIO       | PA26 (Pin 59) |

> Joystick Select is handled via the push button interface (see below)

---

### 🧩 Push Buttons  

> **Important Jumper Configuration:**  
> - **Remove Jumper J18**  
> - **Move Jumper J22 down by 1 pin**  
> 🧩 This configuration enables the BoosterPack push buttons but disables UART functionality.

| Button          | GPIO Pin       |
|-----------------|----------------|
| Button 1        | PA11 (Pin 22)  |
| Button 2        | PA12 (Pin 34)  |
| Joystick Select | PA26 (Pin 59)  |

---

### 🧩 LCD Screen  

- The display is driven by the **ST7735S** LCD controller  
- Graphics support provided by `gfx.c` for basic shapes such as rectangles  
- Original display interface code contributed by **Andrew Deford**

---

### 🧩 Buzzer  

- Audio output driven using PWM  
- Support for tones and simple melodies  
- Open-source melody examples include:
  - Harry Potter Theme  
  - Super Mario Theme  
  - It's a Small World

> PWM and initialization code improved with help from Andrew Deford

---

### 🧩 PWM Timer Mapping

| Peripheral | GPIO Pin | Timer Channel |
|------------|----------|----------------|
| Buzzer     | PB4      | TIMA0_C2       |
| RGB Red    | PB1      | TIMA1_C1       |
| RGB Green  | PA28     | TIMG7_C0       |
| RGB Blue   | PA31     | TIMG7_C1       |

---

## Modules in Development

### 🚧 Light Sensor  
> **Status:** To Be Developed  
Connected pins: PB2 (Pins 14/15)  
Shared functionality: UART3_TX, I2C1_SCL, TIMA0_C3, TIMG6_C0

---

### 🚧 Temperature Sensor  
> **Status:** To Be Developed  
Connected pins: PB16 (Pins 32/33)  
Shared functionality: UART2_RX, SPI1_SCK, TIMG8_C1, TIMG7_C1

---

### 🚧 Servo Motor Connector  
> **Status:** To Be Developed  
Connected pins: PB12 (Pins 28/29)  
Shared functionality: UART3_TX, TIMA0_C2, TIMA0_C1

---

### 🚧 UART Serial Console  
> **Status:** To Be Developed  
May require alternate jumper settings if used with push buttons

---

## Notes

- This SDK provides an extensible foundation for BoosterPack-based development using TI’s **MSPM0G3507** platform.  
- Future improvements may include: I²C sensor support, interrupt-based button handling, and a full serial debug console.


