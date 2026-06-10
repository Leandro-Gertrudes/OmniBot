# 🤖 Mecanum Robot — 42Porto Robotics Club

This project started as a **study on mecanum wheels** carried out by the 42Porto robotics club: how they work, why they enable omnidirectional movement, and how to translate that geometry into motor control. As a **hands-on application** of that study, the team built this robot to validate the theory on real hardware.

The result is a **mecanum-wheeled robot** controlled over **Wi-Fi**, built on an **ESP32**. The robot creates its own Wi-Fi network and serves a web page from which any phone or computer can control it in real time, no app, no internet, just open a browser.

Thanks to the mecanum wheels, the robot moves in **any direction without rotating**: forward/backward, lateral strafe, diagonals, and in-place rotation.

> Project developed by a team of **10 people** from the **42Porto Robotics Club**.

---

## 🎥 Demo

[![Mecanum Robot - 42Porto Robotics Club](https://img.youtube.com/vi/aU86q3xjBkY/maxresdefault.jpg)](https://www.youtube.com/shorts/aU86q3xjBkY)

---

## ✨ Features

- **Omnidirectional movement** -- forward, backward, left/right strafe, 4 diagonals, and in-place rotation.
- **Wi-Fi control** -- the ESP32 acts as an Access Point; no router or internet required.
- **Web interface** served directly by the microcontroller.
- **Keyboard and touch control** -- works on mobile (touch) and PC (keyboard).
- **Signal loss detection** -- if the connection to the robot drops, a "SIGNAL LOST" overlay appears on screen.
- **Adjustable speed** in real time while driving.

---

## 🧩 Hardware

| Component | Qty | Role |
|---|---|---|
| ESP32 (DevKit) | 1 | Main controller + Wi-Fi server |
| L298N driver | 2 | Each driver controls 2 DC motors |
| DC motor | 4 | One per wheel |
| Mecanum wheel | 4 | 2 left + 2 right (45° rollers) |
| Battery / power pack | 1 | Powers motors and ESP32 |
| Chassis | 1 | Robot frame |

### Pin map

**L298N #1 -- FRONT motors** (top side of the ESP32)

| Signal | ESP32 Pin |
|---|---|
| FL_ENA (PWM) | D23 |
| FL_IN1 | D22 |
| FL_IN2 | D2 |
| FR_ENB (PWM) | D13 |
| FR_IN3 | D18 |
| FR_IN4 | D4 |

**L298N #2 -- REAR motors** (bottom side of the ESP32)

| Signal | ESP32 Pin |
|---|---|
| BL_ENA (PWM) | D32 |
| BL_IN1 | D25 |
| BL_IN2 | D26 |
| BR_ENB (PWM) | D27 |
| BR_IN3 | D14 |
| BR_IN4 | D12 |

> 💡 Connect the GND of the ESP32 and both L298N boards together. The `ENA`/`ENB` pins receive the **PWM** signal (speed) and the `IN` pins set the **direction** of each motor.

---

## 💻 Software / Building

The code is a single Arduino sketch for the ESP32 and only uses libraries bundled with the ESP32 core:

- `WiFi.h`
- `WebServer.h`

### Requirements

1. **Arduino IDE** (or PlatformIO).
2. **ESP32 Arduino Core 3.0 or later.**
   > The code uses the new `ledcAttach(pin, frequency, resolution)` API introduced in core 3.x. On version 2.x (which uses `ledcSetup` + `ledcAttachPin`) **it will not compile**. Update the core in the Boards Manager if needed.

### Steps

1. Open the `.ino` file in Arduino IDE.
2. Under **Tools > Board**, select your ESP32 board (e.g. *ESP32 Dev Module*).
3. Connect the ESP32 via USB and select the correct port.
4. Upload the sketch.

---

## 🚀 How to use

1. Power on the robot.
2. On your phone or PC, connect to the Wi-Fi network:
   - **SSID:** `42Porto-robotics`
   - **Password:** `robotics42`
3. Open a browser and go to **`http://192.168.4.1`**.
4. Drive! 🏎️

While a key or button is held, the command is resent every 100 ms; on release, the robot receives `stop` and halts automatically.

---

## ⚙️ How it works

### Architecture

```
[ Browser (phone/PC) ]
        |  HTTP (Wi-Fi)
        v
[ ESP32 -- Access Point + WebServer ]
        |  /cmd?action=...   /speed?v=...
        v
[ Movement functions ] -> [ 2x L298N ] -> [ 4 mecanum motors ]
```

The ESP32 creates its own Wi-Fi network and runs a web server on port 80. The web page sends HTTP requests that the ESP32 translates into motor commands.

### Server endpoints

| Route | Description |
|---|---|
| `/` | Returns the control page (HTML + CSS + JS) |
| `/cmd?action=<cmd>` | Executes a movement (`fwd`, `bwd`, `left`, `right`, `ul`, `ur`, `dl`, `dr`, `rot_l`, `rot_r`, `stop`) |
| `/speed?v=<0-255>` | Sets the global speed (`SPEED`) |

### Mecanum wheel logic

Each wheel is driven independently. The combination of rotation directions produces the desired movement:

| Movement | FL | FR | BL | BR |
|---|:---:|:---:|:---:|:---:|
| Forward | + | + | + | + |
| Backward | - | - | - | - |
| Strafe right | + | - | - | + |
| Strafe left | - | + | + | - |
| Diagonal forward-right | + | 0 | 0 | + |
| Diagonal forward-left | 0 | + | + | 0 |
| Diagonal backward-right | 0 | - | - | 0 |
| Diagonal backward-left | - | 0 | 0 | - |
| Rotate right (CW) | + | - | + | - |
| Rotate left (CCW) | - | + | - | + |

*(+ = forward · - = backward · 0 = stopped)*

### Motor control

The `setMotor()` function takes a value from **-255 to 255**:
- **positive** = forward, **negative** = backward, **0** = stopped.
- The sign sets the direction (`IN` pins), the absolute value becomes the **PWM** duty cycle (`ENA`/`ENB` pins).

PWM is configured at **1000 Hz with 8-bit resolution** via `ledcAttach`.

> 🛠️ **Kick-start note:** there is a commented-out kick-start block in the code (`SPEED_MIN`). It gives the wheels a brief power burst when the set speed is too low to overcome inertia. Disabled by default; re-enable it if the motors struggle to start at low speeds.

---

## 📂 Code structure

| Part | Contents |
|---|---|
| **Part 1** | Pin configuration and constants |
| **Part 2** | Motor helper functions (`setMotor`, `motorFL`, ...) |
| **Part 3** | Movement functions (forward, diagonals, rotation, ...) |
| **Part 4** | Wi-Fi, web server, HTML page, `setup()` and `loop()` |

The web page is stored in `PROGMEM` (program memory) and split into two parts (`PAGE_HTML_1` + `PAGE_HTML_2`) to save RAM.

---
