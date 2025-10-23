# Wiring Notes

- **Ultrasonic (HC-SR04)**: VCC→5V, GND→GND, Trig→D9, Echo→D10
- **Servo**: VCC→5V, GND→GND, Signal→D6
- **LEDs**: Anode via 220Ω to D7 (green), D8 (red); Cathode to GND
- **Buzzer**: +→D13, −→GND
- **LCD (I2C)**: VCC→5V, GND→GND, SDA→A4, SCL→A5 (Uno)
- **Manual Switch**: One side→A0, other side→GND (uses INPUT_PULLUP)
