# SnakeRobot

This repository contains Arduino/ESP32 sketches for a servo‑driven snake robot and related examples.

It includes:

- `snakeservo.ino` — Multi‑segment snake robot using a PCA9685 servo driver and an ultrasonic sensor for basic obstacle handling.
- `servoMotorControl.ino` — Minimal single‑servo sweep example on ESP32 GPIO 27.
- `esp32camTest.ino` — ESP32‑CAM web server example for streaming video over Wi‑Fi (AI‑Thinker board by default).

## Hardware

- ESP32 development board
  - For `esp32camTest.ino`: AI‑Thinker ESP32‑CAM module (PSRAM required)
- Servos: up to 7 micro servos (default in `snakeservo.ino`)
- PCA9685 16‑channel PWM/Servo driver (I²C)
- Ultrasonic distance sensor (HC‑SR04 or similar)
- External 5V supply for servos (do not power servos from ESP32 5V pin)
- Common ground between ESP32, PCA9685, ultrasonic sensor, and servo power

## Libraries

- `Adafruit_PWMServoDriver` (for PCA9685) — Library Manager: “Adafruit PWM Servo Driver Library”
- `Wire` (I²C) — built‑in
- `ESP32Servo` (single servo example)
- ESP32 Arduino core (Board Manager)
- `esp_camera` (installed with ESP32 board support; used by ESP32‑CAM example)

## Wiring

Snake robot (`snakeservo.ino`):

- PCA9685 → ESP32:
  - `SDA` → ESP32 `GPIO 21`
  - `SCL` → ESP32 `GPIO 22`
  - `VCC` → ESP32 `3.3V` (logic)
  - `GND` → ESP32 `GND`
- Servos → PCA9685 channels `0..6` (signal on channel pins, power from external 5V to PCA9685 `V+` and GND)
- Ultrasonic sensor:
  - `Trig` → ESP32 `GPIO 12`
  - `Echo` → ESP32 `GPIO 13` (use a voltage divider or 3.3V‑compatible sensor for Echo)
  - `VCC` → 5V (module dependent)
  - `GND` → GND

Single‑servo example (`servoMotorControl.ino`):

- Servo signal → ESP32 `GPIO 27`
- Servo power → external 5V and GND (common ground with ESP32)

ESP32‑CAM (`esp32camTest.ino`):

- Use AI‑Thinker ESP32‑CAM board with onboard OV2640 camera
- Ensure PSRAM is present and enabled

## Sketch Details

### snakeservo.ino

Multi‑segment snake gait generator using a PCA9685. Key constants and functions:

- `TotalNumberofServos` — number of segments (default: `7`).
- `SERVOMIN`/`SERVOMAX` — PCA9685 pulse bounds; calibrate for your servos.
- `slither(offset, Amplitude, Speed, Wavelengths)` — sinusoidal gait along the body.
- `staticWave(offset, Amplitude, Wavelengths)` — static pose with alternating phase.
- `ring()` — approximate ring configuration using polygon interior angle.
- Ultrasonic distance check (`Trig=GPIO 12`, `Echo=GPIO 13`):
  - If `distance < 20` cm → `straightline()` (basic obstacle reaction)
  - Else → `slither(0, 35, 3, 2)`

Tuning tips:

- Start with small `Amplitude` and ensure `abs(offset) + Amplitude ≤ 90` to avoid over‑rotation.
- Calibrate `SERVOMIN`/`SERVOMAX` per servo to avoid binding.
- Provide a dedicated 5V supply sized for total servo stall current. Keep grounds common.

### servoMotorControl.ino

Minimal sweep of one servo on `GPIO 27` using `ESP32Servo`.

- Use to verify servo wiring and power before running the full snake.
- Note: The second `for` loop in the current sketch uses a comma instead of a semicolon in the condition; if you see a compile error, change `for (int i = 90; i >= 0, i--)` to `for (int i = 90; i >= 0; i--)`.

### esp32camTest.ino

Standard ESP32‑CAM streaming server (AI‑Thinker pins) using `esp_camera`:

- Select board: “AI Thinker ESP32‑CAM”
- Partition scheme: at least 3MB APP space
- Update Wi‑Fi credentials in the sketch variables `ssid` and `password` before flashing
- After connecting, the Serial Monitor prints: `Camera Ready! Use 'http://<IP>' to connect`

Security note: Do not commit real Wi‑Fi credentials in version control. Prefer a `secrets.h` that defines `ssid/password` and is excluded via `.gitignore`.

## Build and Upload

1) Install Arduino IDE (or PlatformIO) and add ESP32 boards via Board Manager.
2) Install libraries listed above via Library Manager.
3) Per sketch:
   - `snakeservo.ino`: Board “ESP32 Dev Module” (or your ESP32), upload to the main ESP32 board.
   - `servoMotorControl.ino`: Same as above.
   - `esp32camTest.ino`: Board “AI Thinker ESP32‑CAM”; enable PSRAM; set correct COM/baud.
4) Connect Serial Monitor (115200 baud for these sketches) for debug output.

## Repository Layout

- `snakeservo.ino` — snake gait + obstacle reaction (PCA9685 + ultrasonic)
- `servoMotorControl.ino` — single‑servo sweep on GPIO 27
- `esp32camTest.ino` — ESP32‑CAM video streaming server example

## Troubleshooting

- Servos jitter or reset the ESP32: use a separate, adequately sized 5V supply for servos; keep grounds common.
- No I²C response from PCA9685: check SDA/SCL wiring (GPIO 21/22), 3.3V logic, and I²C address (default 0x40).
- Ultrasonic readings unstable: add small delays, ensure 5V to sensor, and reduce noise; use level shifting for Echo if needed.
- ESP32‑CAM fails to init: confirm PSRAM, board selection, and camera model define (`CAMERA_MODEL_AI_THINKER`).

