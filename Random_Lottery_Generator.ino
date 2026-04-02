#include <WiFi.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "time.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define BUTTON_PIN 0

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// WiFi
const char* ssid = "HUAWEI-B535-A157";
const char* password = "7G14LT1GQB3";

// State
bool showingLottery = false;
int step = 0;
unsigned long stepStart = 0;

// Timing
const unsigned long introDuration = 2000;
const unsigned long generatingDuration = 2000;
const unsigned long numbersDuration = 15000;
const unsigned long messageDuration = 4000;

// Button
bool lastButtonState = HIGH;

// Messages
const char* messages[] = {
  "Good luck",
  "All the best",
  "Try again",
  "Fingers crossed",
  "Lets see"
};
int msgCount = 5;

// Stored results
int savedNums1[5];
int savedStars1[2];
int savedNums2[5];
int savedStars2[2];

// Selected message
const char* selectedMessage = "";

// ------------------ SETUP ------------------

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();

  connectWiFi();
  setupTime();
}

// ------------------ LOOP ------------------

void loop() {
  handleButton();

  if (showingLottery) {
    runLotterySequence();
  } else {
    showClock();
    delay(1000);
  }
}

// ------------------ WIFI + TIME ------------------

void connectWiFi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}

void setupTime() {
  configTime(0, 0, "pool.ntp.org");
  setenv("TZ", "GMT0BST,M3.5.0/1,M10.5.0/2", 1);
  tzset();
}

// ------------------ BUTTON ------------------

void handleButton() {
  bool state = digitalRead(BUTTON_PIN);

  if (lastButtonState == HIGH && state == LOW) {
    showingLottery = true;
    step = 0;
    stepStart = millis();
  }

  lastButtonState = state;
}

// ------------------ CLOCK ------------------

void showClock() {
  struct tm timeinfo;

  if (!getLocalTime(&timeinfo)) return;

  const char* days[] = {"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};
  const char* months[] = {"Jan","Feb","Mar","Apr","May","Jun",
                          "Jul","Aug","Sep","Oct","Nov","Dec"};

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(1);
  display.setCursor(0, 0);
  display.printf("%s %02d %s",
    days[timeinfo.tm_wday],
    timeinfo.tm_mday,
    months[timeinfo.tm_mon]);

  display.setTextSize(2);
  display.setCursor(10, 20);
  display.printf("%02d:%02d",
    timeinfo.tm_hour,
    timeinfo.tm_min);

  display.setTextSize(1);
  display.setCursor(95, 35);
  display.printf("%02d", timeinfo.tm_sec);

  display.setTextSize(1);
  display.setCursor(35, 55);
  display.printf("%04d", timeinfo.tm_year + 1900);

  display.display();
}

// ------------------ RNG ------------------

uint32_t rng() {
  return esp_random();
}

void shuffle(int *array, int size) {
  for (int i = size - 1; i > 0; i--) {
    int j = rng() % (i + 1);
    int t = array[i];
    array[i] = array[j];
    array[j] = t;
  }
}

// ------------------ GENERATION ------------------

void generateLotteryData() {

  int nums[50];
  int stars[12];

  for (int i = 0; i < 50; i++) nums[i] = i + 1;
  for (int i = 0; i < 12; i++) stars[i] = i + 1;

  // Line 1
  shuffle(nums, 50);
  shuffle(stars, 12);

  for (int i = 0; i < 5; i++) savedNums1[i] = nums[i];
  savedStars1[0] = stars[0];
  savedStars1[1] = stars[1];

  // Line 2
  shuffle(nums, 50);
  shuffle(stars, 12);

  for (int i = 0; i < 5; i++) savedNums2[i] = nums[i];
  savedStars2[0] = stars[0];
  savedStars2[1] = stars[1];
}

// ------------------ MESSAGE ------------------

void selectMessage() {
  selectedMessage = messages[rng() % msgCount];
}

// ------------------ SEQUENCE ------------------

void runLotterySequence() {

  unsigned long now = millis();

  switch (step) {

    case 0:
      display.clearDisplay();
      display.setTextSize(2);
      display.setCursor(0, 20);
      display.println("EuroMillions");
      display.display();

      if (now - stepStart > introDuration) {
        step = 1;
        stepStart = now;
      }
      break;

    case 1:
      display.clearDisplay();
      display.setTextSize(1);
      display.setCursor(0, 20);
      display.println("Generating...");
      display.println("Lottery numbers");
      display.display();

      if (now - stepStart > generatingDuration) {
        generateLotteryData();
        selectMessage();
        step = 2;
        stepStart = now;
      }
      break;

    case 2:
      display.clearDisplay();
      display.setTextSize(1);
      display.setCursor(0, 0);
      display.println("EuroMillions");
      display.println();

      display.print("1) ");
      for (int i = 0; i < 5; i++) {
        display.print(savedNums1[i]);
        display.print(" ");
      }
      display.print("| ");
      display.print(savedStars1[0]);
      display.print(" ");
      display.println(savedStars1[1]);

      display.println();

      display.print("2) ");
      for (int i = 0; i < 5; i++) {
        display.print(savedNums2[i]);
        display.print(" ");
      }
      display.print("| ");
      display.print(savedStars2[0]);
      display.print(" ");
      display.println(savedStars2[1]);

      display.display();

      if (now - stepStart > numbersDuration) {
        step = 3;
        stepStart = now;
      }
      break;

    case 3:
      display.clearDisplay();
      display.setTextSize(2);
      display.setCursor(10, 20);
      display.println(selectedMessage);
      display.display();

      if (now - stepStart > messageDuration) {
        showingLottery = false;
      }
      break;
  }
}