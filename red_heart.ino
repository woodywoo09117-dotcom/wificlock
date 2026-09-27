#include "ArduinoGraphics.h"
#include "Arduino_LED_Matrix.h"
#include "RTC.h"
#include "WiFiS3.h"

char ssid[] = "YOUR_WIFI_NAME";
char pass[] = "YOUR_WIFI_PASSWORD";

const int GMT_OFFSET_HOURS = -4;  // Change for your timezone

ArduinoLEDMatrix matrix;
const char* dayNames[7] = {
  "SUNDAY", "MONDAY", "TUESDAY", "WEDNESDAY",
  "THURSDAY", "FRIDAY", "SATURDAY"
};

bool syncClock() {
  unsigned long connectionStart = millis();
  Serial.print("Wi-Fi firmware: ");
  Serial.println(WiFi.firmwareVersion());

  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("Connecting to Wi-Fi...");
    WiFi.begin(ssid, pass);
    delay(5000);
    if (millis() - connectionStart >= 30000) {
      Serial.println("Wi-Fi connection timed out.");
      return false;
    }
  }

  unsigned long epochTime = 0;
  unsigned long timeStart = millis();

  while (epochTime == 0) {
    epochTime = WiFi.getTime();
    delay(1000);
    if (millis() - timeStart >= 60000) {
      Serial.println("NTP time sync timed out.");
      return false;
    }
  }

  epochTime += GMT_OFFSET_HOURS * 3600;
  RTCTime syncedTime(epochTime);
  RTC.setTime(syncedTime);
  Serial.println("Clock synchronized.");
  return true;
}

void setup() {
  Serial.begin(115200);
  matrix.begin();
  RTC.begin();

  syncClock();
}

void loop() {
  RTCTime currentTime;
  RTC.getTime(currentTime);

  int dayIndex = static_cast<int>(currentTime.getDayOfWeek());
  String message = "            ";
  message += dayNames[dayIndex];
  message += " ";
  message += currentTime.toString();
  message += "            ";

  matrix.beginDraw();
  matrix.stroke(0xFFFFFFFF);
  matrix.textScrollSpeed(50);
  matrix.textFont(Font_5x7);

  matrix.beginText(0, 1, 0xFFFFFF);
  matrix.print(message);
  matrix.endText(SCROLL_LEFT);

  matrix.endDraw();
}