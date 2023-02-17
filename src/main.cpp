#include <Arduino.h>

#include <WiFi.h>
#include <ezTime.h>

#include <Wire.h>
#include <SH1106Wire.h>

#include <Preferences.h>
#include <esp32-config-lib.hpp>

#define REED_STATUS_LED 25
#define REED_CONTACT 27

#define IR_STATUS_LED 18                                                                                                                                                                                         
#define IR_SENSOR 17  

#define BUTTON 26                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   

SH1106Wire display(0x3c, SDA, SCL);

String getNameOfDay(uint8_t d) {
  switch(d) {
    case SUNDAY: return "Sonntag";
    case MONDAY: return "Montag";
    case TUESDAY: return "Dienstag";
    case WEDNESDAY: return "Mittwoch";
    case THURSDAY: return "Donnerstag";
    case FRIDAY: return "Freitag";
    case SATURDAY: return "Samstag";
    default: return "Unbekannt";
  }
}

String getNameOfDaylightSavingTime(bool dst) {
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
  Timezone tz;
  tz.setLocation("de");
  char dateString[36];
  char timeString[36];
  sprintf(dateString, "%02d.%02d.%04d", tz.day(), tz.month(), tz.year());
  sprintf(timeString, "%02d:%02d:%02d", tz.hour(), tz.minute(), tz.second());
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(0, 15, getNameOfDay(tz.weekday()));
  display.drawString(0, 45, tz.getTimezoneName());
  display.setTextAlignment(TEXT_ALIGN_RIGHT);
  display.drawString(127, 15, dateString);
  display.drawString(127, 45, getNameOfDaylightSavingTime(tz.isDST()));
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

enum ProgramMode {
	RUN,
	CONFIG
};

ProgramMode currentProgramMode = RUN;

esp32config::Configuration config("SHO Config", {
		new esp32config::Namespace("General", "general", {
			new esp32config::Entry("NTP-Hostname", esp32config::TEXT, "ntp-host", false, "pool.ntp.org"),
			new esp32config::Entry("Display-Timeout (ms)", esp32config::INTEGER, "display-timeout", false, "60000")
		}),
		new esp32config::Namespace("WiFi", "wifi", {
			new esp32config::Entry("SSID", esp32config::TEXT, "ssid"),
			new esp32config::Entry("Password", esp32config::PASSWORD, "password")
		}),
		new esp32config::Namespace("MQTT", "mqtt", {
			new esp32config::Entry("Hostname", esp32config::TEXT, "hostname"),
			new esp32config::Entry("Username", esp32config::TEXT, "username"),
			new esp32config::Entry("Password",esp32config:: PASSWORD, "password")
		})}
	);

esp32config::Server configServer(config);
char ntp_hostname[32];
char ssid[32];
char password[64];

void setup_config() {
	configServer.begin("Smart Home Agent", "sha-secret", IPAddress(192, 168,10, 1));
}

void setup_run() {
  // Read preferences
  Preferences wifi, ntp, mqtt;
  wifi.begin("wifi", true);
  ntp.begin("ntp", true);
  mqtt.begin("mqtt", true);

  // Initialize WiFi
  wifi.getString("ssid", ssid, 31);
  wifi.getString("password", password, 63);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

   // Initialize OLED display
  display.init();
  display.flipScreenVertically();

  // Initialize local time from NTP Server
  ezt::waitForSync();
}

void setup() {
  Serial.begin(115200);

  // Initialize pin modes
  pinMode(REED_STATUS_LED, OUTPUT);
  pinMode(IR_STATUS_LED, OUTPUT);
  pinMode(REED_CONTACT, INPUT);
  pinMode(IR_SENSOR, INPUT);
  pinMode(BUTTON, INPUT);

  // Check wich mode to start
  if(digitalRead(BUTTON) == HIGH) {
	currentProgramMode = CONFIG;
  }

  // Setup the startet mode
  if(currentProgramMode == RUN)  {
	setup_run();
  }
  if(currentProgramMode == CONFIG)  {
	setup_config();
  }
}

void loop_config() {
	configServer.loop();
}

void loop_run() {
  digitalWrite(IR_STATUS_LED, !digitalRead(IR_SENSOR));
  digitalWrite(REED_STATUS_LED, digitalRead(REED_CONTACT));
  input_thread();
  output_thread();
}

void loop() {
  // Loop the started mode
  if(currentProgramMode == RUN)  {
	loop_run();
  }
  if(currentProgramMode == CONFIG)  {
	loop_config();
  }
}