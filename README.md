# Smart Access Gate System (Arduino)

Automated, sensor-based gate with ultrasonic detection, servo actuation, LEDs, buzzer, and 16x2 I2C LCD. Includes a manual override switch. Written in Arduino (C/C++) for the Arduino Uno.

> This repo is generated directly from the project report and includes the original PDF in `docs/`. See wiring, code, and testing notes below.  

## Features
- Ultrasonic vehicle detection (HC-SR04)
- Smooth servo gate control with staged angles
- Visual (LED) and audible (buzzer) alerts
- 16x2 I2C LCD status messages
- Manual override switch for maintenance/emergencies

## Hardware
| Part | Notes | Pins |
|---|---|---|
| HC-SR04 Ultrasonic | VCC 5V, GND | Trig: D9, Echo: D10 |
| Servo | 5V supply, GND | Signal: D6 |
| Green LED | 220Ω resistor | D7 |
| Red LED | 220Ω resistor | D8 |
| Buzzer | Piezo or active buzzer | D13 |
| 16x2 I2C LCD | Address usually `0x27` | SDA: A4, SCL: A5 |
| Manual Switch | Pull-up enabled | A0 to GND |

## Setup
1. Install Arduino IDE and add the following libraries if missing:
   - `LiquidCrystal_I2C`
   - `Servo`
2. Clone this repo and open `src/SmartAccessGate.ino`.
3. Wire the components per the table above.
4. Upload to an Arduino Uno.

## Behavior
- Opens when object detected within 8 cm, with buzzer + green LED.
- Auto-closes after ~6 seconds of no detection, with buzzer + red LED.
- Manual switch toggles open/close and temporarily disables auto mode.

## How it works

### Distance measurement
`automaticGateControl()` triggers the HC-SR04 by pulling `trigPin` (D9) HIGH for 10 µs, then LOW. `pulseIn(echoPin, HIGH)` measures how long the echo pin stays HIGH — the round-trip travel time of the ultrasonic pulse. The sketch converts that to centimetres:

```cpp
distance = duration * 0.034 / 2;  // µs × speed of sound (cm/µs) ÷ 2 for round-trip
```

If `distance <= distanceThreshold` (8 cm), a vehicle is detected and the gate opens.

### Servo control
`openGate()` and `closeGate()` step the servo in 30° increments (`angleStep`) between `closeAngle` (0°) and `openAngle` (90°), pausing 500 ms per step:

```cpp
for (int angle = currentAngle; angle <= openAngle; angle += angleStep) {
    gateServo.write(angle);
    delay(500);
}
```

### Manual override
The switch on pin A0 uses `INPUT_PULLUP`, so it reads `LOW` when pressed. Each press toggles the `isManualOverrideActive` flag: the first press sets it `true` and opens the gate; the second press clears it and closes the gate. While the flag is `true`, `loop()` bypasses `automaticGateControl()` entirely, leaving the gate under manual control until the switch is pressed again.

## Directory Layout
```
smart-access-gate-arduino/
  ├─ src/
  │   └─ SmartAccessGate.ino
  ├─ docs/
  │   └─ PROJECT_REPORT_ON_SMART_ACCESS_GATE_SYSTEM_USING_ARDUINO_UNO.pdf
  ├─ hardware/
  │   ├─ BOM.md
  │   └─ wiring.md
  ├─ .gitignore
  ├─ LICENSE
  └─ README.md
```

## License
MIT. See `LICENSE`.

## Credits
- Based on the project report "PROJECT REPORT ON SMART ACCESS GATE SYSTEM USING ARDUINO UNO" included in `docs/`.
## Demo Video

[![Smart Access Gate System Demo](https://img.youtube.com/vi/eSR9oMO9okk/0.jpg)](https://youtube.com/shorts/eSR9oMO9okk?feature=share)

Watch the demo here: https://youtube.com/shorts/eSR9oMO9okk?feature=share
