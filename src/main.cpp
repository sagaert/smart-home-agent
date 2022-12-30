#include <Arduino.h>

#include <WiFi.h>
#include <time.h>

#include <Wire.h>
#include <SH1106Wire.h>

#include <Preferences.h>

#define REED_STATUS_LED 25
#define REED_CONTACT 27

#define IR_STATUS_LED 18                                                                                                                                                                                         
#define IR_SENSOR 17  

#define BUTTON 26                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   

#define TIMEZONE PSTR("CET-1CEST,M3.5.0/02,M10.5.0/03")

SH1106Wire display(0x3c, SDA, SCL);

String getNameOfDay(int d) {
  switch(d) {
    case 0: return "Sonntag";
    case 1: return "Montag";
    case 2: return "Dienstag";
    case 3: return "Mittwoch";
    case 4: return "Donnerstag";
    case 5: return "Freitag";
    case 6: return "Samstag";
    default: return "Unbekannt";
  }
}

String getNameOfDaylightSavingTime(int dst) {
  if(dst) {
    return "Sommerzeit";
  } else {
    return "Normalzeit";
  }
}

void renderHomePage() {
  display.setFont(ArialMT_Plain_24);
  display.setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
  display.drawString(64, 32, "Welcome");
}

void renderWifiPage() {
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(0, 0, "WiFi");
  display.drawLine(0, 12, 127, 12);
  switch(WiFi.status()) {
    case WL_CONNECTED:
      char wifiName[32];
      char wifiAddress[32];
      char signalStrength[32];
      char wifiChannel[16];
      sprintf(wifiName, "SSID: %s", WiFi.SSID().c_str());
      sprintf(wifiAddress, "IP: %s", WiFi.localIP().toString().c_str());
      sprintf(signalStrength, "Signal: %d%%", (WiFi.RSSI() + 100)* 2 );
      sprintf(wifiChannel, "Kanal: %d", WiFi.channel());
      display.setFont(ArialMT_Plain_10);
      display.setTextAlignment(TEXT_ALIGN_LEFT);
      display.drawString(0, 15, "Status: Connected");
      display.drawString(0, 27, wifiName);
      display.drawString(0, 39, wifiAddress);
      display.drawString(0, 51, wifiChannel);
      display.setTextAlignment(TEXT_ALIGN_RIGHT);
      display.drawString(127, 51, signalStrength);
      break;
    default:
      display.setFont(ArialMT_Plain_10);
      display.setTextAlignment(TEXT_ALIGN_LEFT);
      display.drawString(0, 15, "Status: Connecting...");
      break;
  }
}

void renderTimePage() {
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(0, 0, "Time");
  display.drawLine(0, 12, 127, 12);
  time_t now_t;
  time(&now_t);
  tm now;
  localtime_r(&now_t, &now);
  char dateString[36];
  char timeString[36];
  sprintf(dateString, "%02d.%02d.%04d", now.tm_mday, now.tm_mon + 1, now.tm_year + 1900);
  sprintf(timeString, "%02d:%02d:%02d", now.tm_hour, now.tm_min, now.tm_sec);
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(0, 15, getNameOfDay(now.tm_wday));
  display.drawString(0, 45, "CET");
  display.setTextAlignment(TEXT_ALIGN_RIGHT);
  display.drawString(127, 15, dateString);
  display.drawString(127, 45, getNameOfDaylightSavingTime(now.tm_isdst));
  display.setFont(ArialMT_Plain_16);
  display.drawString(127, 27, timeString);
}

void renderElectricityPage() {
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(0, 0, "Electricity");
  display.drawLine(0, 12, 127, 12);
}

static boolean displayOn = false;
enum pages { HOME_PAGE, WIFI_PAGE, TIME_PAGE, ELECTRICITY_PAGE };
static pages currentPage = HOME_PAGE;
static unsigned long buttonPressedTime = 0;
static const unsigned long screensaverTimeout = 60000UL; // Switch off display after 1 minute without input
const unsigned long displayRefreshInterval = 100UL; // Refresh the display 10 times a second

void output_thread() {
  static enum { ON, OFF } status = OFF;
  static unsigned long lastRenderingTimestamp = 0UL;
  
  switch(status) {
    case ON:
      // Switch display off, if screensaver should be activated
      displayOn = !(millis() - buttonPressedTime > screensaverTimeout);
      if(displayOn) {
        if(millis() - lastRenderingTimestamp > displayRefreshInterval) {
          // Update display
          display.clear();
          switch(currentPage) {
            case WIFI_PAGE:
              renderWifiPage();
              break;
            case TIME_PAGE:
              renderTimePage();
              break;
            case ELECTRICITY_PAGE:
              renderElectricityPage();
              break;
            default:
              renderHomePage();
              break;
          }
          display.display();
          lastRenderingTimestamp = millis();
        }
      } else {
        // Switch off display
        display.clear();
        display.display();
        status = OFF;
      }
      break;
    case OFF:
      if(displayOn) {
        // Swtich on display
        status = ON;
      }
      break;
  }
  
}

void input_thread() {
  static enum { RELEASED, PRESSED } status = RELEASED;
  int button = digitalRead(BUTTON);

  if(status == PRESSED && button == LOW) {
    // If button was pressed and now is released
   
    if(displayOn) {
      // Switch to next page
      switch(currentPage) {
        case HOME_PAGE:
          currentPage = WIFI_PAGE;
          break;
        case WIFI_PAGE:
          currentPage = TIME_PAGE;
          break;
        case TIME_PAGE:
          currentPage = ELECTRICITY_PAGE;
          break;
        default:
          currentPage = HOME_PAGE;
          break;
      }
    } else {
      // Activate display and switch to home page
      displayOn = true;
      currentPage = HOME_PAGE;
    }
    // Reset button pressed timestamp
    buttonPressedTime = millis();
  }
  status = button == HIGH ? PRESSED : RELEASED;
}

void wifi_thread() {
  // TODO: Checking connection and reconnect automatically after some time if neccesary
  /*
  do {
    Serial.print("Connecting WLAN");
    WiFi.begin(wifi.getString("ssid"), wifi.getString("key"));
    for(int i = 0 ; i < 30 && WiFi.status() != WL_CONNECTED; i++) {
      Serial.print(".");
      delay(1000);    
    }
    if(WiFi.status() != WL_CONNECTED) {
      WiFi.disconnect();
    }
  } while(WiFi.status() != WL_CONNECTED); */
}

void measuring_thread() {
  // TODO: Measure current consumption and eliminate flaky signals + send some mqtt messages if nccesary
}

void setup() {
  Serial.begin(115200);

  // Read preferences
  Preferences wifi, ntp, mqtt;
  wifi.begin("wifi", true);
  ntp.begin("ntp", true);
  mqtt.begin("mqtt", true);

  // Initialize pin modes
  pinMode(REED_STATUS_LED, OUTPUT);
  pinMode(IR_STATUS_LED, OUTPUT);
  pinMode(REED_CONTACT, INPUT);
  pinMode(IR_SENSOR, INPUT);
  pinMode(BUTTON, INPUT);

  // Initialize WiFi
  char ssid[32];
  char password[64];
  wifi.getString("ssid", ssid, 31);
  wifi.getString("password", password, 63);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

   // Initialize OLED display
  display.init();
  display.flipScreenVertically();

  // Initialize local time from NTP Server
  char ntp_hostname[32];
  ntp.getString("hostname", ntp_hostname, 31);
//  configTzTime(TIMEZONE, ntp_hostname);
  configTzTime(TIMEZONE, "pool.ntp.org");
}

void loop() {
  digitalWrite(IR_STATUS_LED, !digitalRead(IR_SENSOR));
  digitalWrite(REED_STATUS_LED, digitalRead(REED_CONTACT));
  input_thread();
  output_thread();
}