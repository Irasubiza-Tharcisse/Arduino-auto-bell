#include <Wire.h>
#include <RTClib.h>
#include <LiquidCrystal_I2C.h>

RTC_DS3231 rtc;
LiquidCrystal_I2C lcd(0x26, 20, 4);

// Pin definitions
const int buzzerPin = 8;   // Buzzer pin
const int modeBtnPin = 2;  // Mode button pin
const int adjustPin = 9;   // Adjust button pin

// Constants
const char* daysOfTheWeek[7] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
const unsigned long debounceDelay = 50; // Debounce delay in milliseconds

// Variables for button handling
int modeClicks = 1;        // Tracks mode button clicks
int adjustClicks = 1;
int lastModeButtonState = HIGH;      // Previous state of the mode button
int currentModeButtonState = HIGH;   // Current state of the mode button
unsigned long lastModeDebounceTime = 0; // Last time the mode button was toggled

int lastAdjustButtonState = HIGH;    // Previous state of the adjust button
int currentAdjustButtonState = HIGH; // Current state of the adjust button
unsigned long lastAdjustDebounceTime = 0; // Last time the adjust button was toggled

bool toggleState = false; // Variable to store the toggle state
bool toggleDay = false;

// Variables for bell scheduling
int upcomingHr = -1;       // Upcoming bell hour
int upcomingMin = -1;      // Upcoming bell minute

// Day type
String DayType = "Normal Day";

void setup() {
  // Initialize pins
  pinMode(buzzerPin, OUTPUT);
  pinMode(modeBtnPin, INPUT_PULLUP);
  pinMode(adjustPin, INPUT_PULLUP);

  // Initialize Serial and LCD
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();

  // Initialize RTC
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  //rtc.adjust(DateTime(2025, 2, 24, 9, 10, 0));
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  // Display initial page
  Page1();
}

void loop() {
  // Update current time and display
  DateTime now = rtc.now();
  lcd.setCursor(0, 3);
  lcd.print(currentDate());
  lcd.print(' ');
  lcd.print(currentTime());

  // Check for upcoming bells
  checkBellType();

  // Handle mode button
  handleButton(modeBtnPin, lastModeButtonState, currentModeButtonState, lastModeDebounceTime, handleModeButtonClick);

  // Handle adjust button
  handleButton(adjustPin, lastAdjustButtonState, currentAdjustButtonState, lastAdjustDebounceTime, handleAdjustButtonClick);

  delay(100); // Small delay to reduce CPU usage
}

// Function to handle button debounce and clicks
void handleButton(int buttonPin, int &lastButtonState, int &currentButtonState, unsigned long &lastDebounceTime, void (*clickHandler)()) {
  int buttonReading = digitalRead(buttonPin);

  // Check if the button state has changed
  if (buttonReading != lastButtonState) {
    lastDebounceTime = millis(); // Reset the debounce timer
  }

  // If the button state has been stable for the debounce delay, process it
  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (buttonReading != currentButtonState) {
      currentButtonState = buttonReading;

      // If the button is pressed (LOW), trigger the click handler
      if (currentButtonState == LOW) {
        clickHandler();
      }
    }
  }

  // Save the current button state for the next loop
  lastButtonState = buttonReading;
}

// Function to handle mode button clicks
void handleModeButtonClick() {
  modeClicks++;
  if (modeClicks > 4) {
    modeClicks = 1;
  }

  switch (modeClicks) {
    case 1:
      Page1();
      Serial.println("Mode Button: Page 1");
      break;
    case 2:
      Page2();
      Serial.println("Mode Button: Page 2");
      break;
    case 3:
      Page3();
      Serial.println("Mode Button: Page 3");
      break;
    case 4:
      Page4();
      Serial.println("Mode Button: Page 4");
      break;
  }
}

// Function to handle adjust button clicks
void handleAdjustButtonClick() {
  switch (modeClicks) {
    case 1:
      triggerBuzzer();
      Serial.println("Adjust Button: Buzzer Triggered");
      break;
    case 2:
      toggleBell();
      break;
    case 3:
      adjustDay();
      break;

    default:
      break;
  }
}

// Function to format and return the current time as a string
String currentTime() {
  DateTime now = rtc.now();
  char formattedTime[9]; // Buffer to store the formatted time (HH:MM:SS)
  sprintf(formattedTime, "%02d:%02d:%02d", now.hour(), now.minute(), now.second());
  return formattedTime;
}

// Function to format and return the current date as a string
String currentDate() {
  DateTime now = rtc.now();
  char buffer[11]; // Buffer to store the formatted date (YYYY/MM/DD)
  sprintf(buffer, "%04d/%02d/%02d", now.year(), now.month(), now.day());
  return buffer;
}

// Function to display Page 1
void Page1() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("AUTOMATED BELL PROJ");
  lcd.setCursor(0, 1);
  lcd.print("MADE BY 'DEV SQUAD'");
}

// Function to display Page 2
void Page2() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("UPCOMING BELL  -->");
  lcd.setCursor(5, 1);
  String upHr = (upcomingHr < 10) ? '0' + String(upcomingHr) : String(upcomingHr);
  String upMin = (upcomingMin < 10) ? '0' + String(upcomingMin) : String(upcomingMin);
  lcd.print(upHr + ":" + upMin);

  lcd.setCursor(5, 2);
  lcd.print(toggleState ? "ON" : "OFF");
}

// Function to display Page 3
void Page3() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("ADJUST DAY TYPE");
  lcd.setCursor(0, 1);
  lcd.print("Today is: ");
  lcd.print(DayType);
}

// Function to display Page 4
void Page4() {
  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.print("ABOUT THE BELL");
  lcd.setCursor(0, 1);
  lcd.print("IT'S AUTOMATIC BELL!");
}

// Function to trigger the buzzer
void triggerBuzzer() {
  for (int i = 0; i < 3; i++) {
    tone(buzzerPin, 1000); // Generate a 1kHz tone
    delay(500); // Ring for 0.5 seconds
    noTone(buzzerPin); // Turn off the tone
    delay(250); // Wait for 0.25 seconds
  }
  delay(500);
  for (int i = 0; i < 3; i++) {
    tone(buzzerPin, 1000); // Generate a 1kHz tone
    delay(500); // Ring for 0.5 seconds
    noTone(buzzerPin); // Turn off the tone
    delay(250); // Wait for 0.25 seconds
  }
}

// Function to check and update the bell schedule
void checkBellType() {
  if (toggleDay)
  {
    DayOff();
  }
  else
  {
    NormalDays();
  }

}

// Function to handle normal days
void NormalDays() {
  DateTime now = rtc.now();

  DayType = "Normal Day";

  int bellHours[12] = {9,9, 9, 9, 10, 10, 10, 10, 10, 10, 10, 10};
  int bellMinutes[12] = {45, 50, 55, 59, 5, 10, 15, 20, 25, 30, 35, 40, };

  int currentHr = now.hour();
  int currentMin = now.minute();
  int currentSec = now.second();

  int numBells = 12;
  int nextHr, nextMin;
  findUpcomingBell(bellHours, bellMinutes, currentHr, currentMin, numBells, nextHr, nextMin);

  if (nextHr == currentHr && nextMin == currentMin && toggleState && currentSec < 15) {
    triggerBuzzer();
    checkBellType();
    findUpcomingBell(bellHours, bellMinutes, currentHr, currentMin, numBells, nextHr, nextMin);
  }
}


// Function to handle days off
void DayOff() {
  DateTime now = rtc.now();

  DayType = "Day-Off";

  int belHours[8] = {7, 12, 13, 16, 18, 18, 19, 21};
  int belMinutes[8] = {30, 40, 20, 30, 0, 40, 20, 0};

  int curHr = now.hour();
  int curMin = now.minute();
  int curSec = now.second();

  int numBels = 8;
  int nextHrs, nextMins;
  findUpcomingBell(belHours, belMinutes, curHr, curMin, numBels, nextHrs, nextMins);

  if (nextHrs == curHr && nextMins == curMin && toggleState && curSec < 15) {
    triggerBuzzer();
    checkBellType();
    findUpcomingBell(belHours, belMinutes, curHr, curMin, numBels, nextHrs, nextMins);
  }
}

// Function to find the upcoming bell time
void findUpcomingBell(int bellHours[], int bellMinutes[], int currentHr, int currentMin, int numBells, int &nextHr, int &nextMin) {
  int minDiff = 24 * 60;
  nextHr = 23;
  nextMin = 59;

  for (int i = 0; i < numBells; i++) {
    int bellHr = bellHours[i];
    int bellMin = bellMinutes[i];
    int totalCurrentMin = currentHr * 60 + currentMin;
    int totalBellMin = bellHr * 60 + bellMin;
    int diff = totalBellMin - totalCurrentMin;

    if (diff >= 0 && diff < minDiff) {
      minDiff = diff;
      nextHr = bellHr;
      nextMin = bellMin;
    }
    upcomingHr = nextHr;
    upcomingMin = nextMin;
  }
}

void toggleBell()
{
  toggleState = !toggleState;
  Page2();

  Serial.print("Adjust Button: Bell Toggled - ");
  Serial.println(toggleState ? "ON" : "OFF");
}

void adjustDay() {
  
  // Toggle DayType
  toggleDay = !toggleDay;

  Serial.println(toggleDay);

  toggleDay ? (DayType = "Day-Off") : (DayType = "Normal Day");

  Serial.println(DayType);

  Page3(); // Call Page3 function
}