# Arduino Auto Bell

![Auto Bell](https://github.com/mugisha-eric/Arduino-auto-bell/blob/main/R09A0044.JPG?raw=true)

This project is an automatic bell system using an Arduino Uno, Real Time Clock (RTC), LCD display, two push buttons, and a breadboard. The system allows you to set specific times for the bell to ring automatically.

## Features
- **Automatic Bell Ringing:** Set specific times for the bell to ring automatically.
- **Manual Override:** Use push buttons to set and disable the alarm manually.
- **LCD Display:** Display current time and alarm status on a 16x2 LCD.
- **Adjustable Buzzer Duration:** Customize the duration of the buzzer sound.
  
🕒 RTC Module

🖥️ LCD Display

🔔 Buzzer/Bell
## Components Needed
- Arduino Uno
- RTC module (e.g., DS3231)
- LCD display (16x2)
- 2 Push Buttons
- Breadboard
- Jumper wires
- 10k Ohm resistors (for push buttons)
- Buzzer or Bell

## Circuit Diagram
![Circuit Diagram](https://github.com/mugisha-eric/Arduino-auto-bell/blob/main/AutoBell.png?raw=true)

## Setup Instructions

1. **Wiring the Components:**
    - Connect the RTC module to the Arduino using I2C (SDA to A4, SCL to A5).
    - Connect the LCD display to the Arduino using digital pins.
        - RS to Pin 7
        - E to Pin 6
        - D4 to Pin 5
        - D5 to Pin 4
        - D6 to Pin 3
        - D7 to Pin 2
    - Connect one push button to pin 8 and ground with a 10k Ohm resistor.
    - Connect the other push button to pin 9 and ground with a 10k Ohm resistor.
    - Connect the Buzzer to Pin 10 and GND.

2. **Programming the Arduino:**
    - Install the necessary libraries for the RTC and LCD.
    - Upload the following code to your Arduino:

```cpp
#include <Wire.h>
#include <RTClib.h>
#include <LiquidCrystal.h>

RTC_DS3231 rtc;
LiquidCrystal lcd(7, 6, 5, 4, 3, 2);

const int buzzer = 10;
const int button1 = 8;
const int button2 = 9;
DateTime alarmTime;

void setup() {
  Wire.begin();
  rtc.begin();
  lcd.begin(16, 2);
  pinMode(buzzer, OUTPUT);
  pinMode(button1, INPUT_PULLUP);
  pinMode(button2, INPUT_PULLUP);
  alarmTime = rtc.now();
  lcd.print("Auto Bell System");
}

void loop() {
  DateTime now = rtc.now();
  lcd.setCursor(0, 1);
  lcd.print(now.hour());
  lcd.print(":");
  lcd.print(now.minute());
  lcd.print(":");
  lcd.print(now.second());

  if (digitalRead(button1) == LOW) {
    alarmTime = now + TimeSpan(0, 0, 1, 0);
    lcd.setCursor(0, 0);
    lcd.print("Alarm Set: ");
    lcd.print(alarmTime.hour());
    lcd.print(":");
    lcd.print(alarmTime.minute());
  }

  if (digitalRead(button2) == LOW) {
    lcd.clear();
    lcd.print("Alarm Disabled");
    alarmTime = rtc.now();
  }

  if (now.hour() == alarmTime.hour() && now.minute() == alarmTime.minute() && now.second() == alarmTime.second()) {
    digitalWrite(buzzer, HIGH);
    delay(1000);
    digitalWrite(buzzer, LOW);
  }

  delay(1000);
}
