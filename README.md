ESP32 Lottery Generator + Environmental Display

A compact standalone device using an ESP32 that:

Displays real-time temperature and pressure
Syncs time and date via WiFi (NTP)
Generates valid lottery number sets
Outputs multiple lines per draw
Adds simple randomized messages

No cloud dependency. No external RTC required.
Features
Idle Mode
Time & date (via WiFi)
Temperature + pressure (BME280)
Continuous display
Generator Mode
Triggered by button
Generates 3 independent lines
Supports:
EuroMillions (5 + 2)
Thunderball (5 + 1)
HotPicks (1–5 numbers)
Displays a short random message
Interaction
Single button:
Short press → generate numbers
Auto return to idle mode after timeout
Hardware
ESP32 DevKitC
SSD1306 OLED Display
BME280
1x push button

modes =  SENSOR_MODE
  → time (NTP)
  → temperature / pressure

GENERATOR_MODE
  → generate lottery lines
  → display results + message


  Core Logic
Stateless RNG using ESP32 hardware randomness
Each generated line is independent
No cross-line constraints or bias
All outputs follow valid lottery rules

Example Output
Idle Mode  =  12:43:18
02 Apr 2026

Temp: 21.4 C
Press: 1012 hPa

Generator Mode  =   EuroMillions

1) 4 18 23 37 49 | 2 11
2) 7 12 19 33 45 | 1 8
3) 3 9 21 34 50 | 5 10

Good luck

File Structure
main.ino → main loop and state machine
generator.h → lottery logic
sensor.h → BME280 handling
time_sync.h → WiFi + NTP setup
display.h → OLED rendering
config.h → constants and settings


Design Principles
Separation of concerns:
RNG, UI, sensors, and time are independent
No unnecessary complexity
No pseudo-intelligent logic (no “prediction”)
Deterministic, repeatable structure


Future Improvements (optional)
Multi-ticket generation (configurable count)
OLED animations during generation
Battery-powered enclosure
Mode persistence in flash memory

License

MIT 

.gitignore (basic) = *.bin
*.elf
*.map
*.log
.vscode/
build/

Optional docs/wiring.md (short example)
OLED (I2C)
VCC → 3.3V
GND → GND
SDA → GPIO21
SCL → GPIO22

BME280 (I2C)
Same SDA/SCL as OLED

Button
GPIO0 → Button → GND
(use INPUT_PULLUP)


