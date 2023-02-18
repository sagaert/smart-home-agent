#include <agent.hpp>                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               

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

void renderLoadingPage() {
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
  display.drawString(64, 32, "Verbinde WLAN und\nsynchronisiere Zeit...");
}

void renderHomePage() {
  display.setFont(ArialMT_Plain_24);
  display.setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
  display.drawString(64, 32, "Welcome");
}

void renderWifiPage() {
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(0, 0, "WLAN");
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
      display.drawString(0, 15, "Status: Verbunden");
      display.drawString(0, 27, wifiName);
      display.drawString(0, 39, wifiAddress);
      display.drawString(0, 51, wifiChannel);
      display.setTextAlignment(TEXT_ALIGN_RIGHT);
      display.drawString(127, 51, signalStrength);
      break;
    default:
      display.setFont(ArialMT_Plain_10);
      display.setTextAlignment(TEXT_ALIGN_LEFT);
      display.drawString(0, 15, "Status: Verbinden...");
      break;
  }
}

void renderTimePage() {
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(0, 0, "Zeit");

  // Super hack for generating an extended RFC3339 string based on UTC
/*  display.setTextAlignment(TEXT_ALIGN_RIGHT);
  display.drawString(127, 0, UTC.dateTime(RFC3339_EXT));
  display.setTextAlignment(TEXT_ALIGN_LEFT); */
  
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
  display.drawString(0, 0, "Stromverbrauch");
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

void measuring_thread() {
  // TODO: Measure current consumption and eliminate flaky signals + send some mqtt messages if nccesary
}

enum ProgramMode {
	RUN,
	CONFIG
};

ProgramMode currentProgramMode = RUN;

AgentConfiguration configuration;

void setup_run() {
	// Load configuration
	configuration.load();

	// Initialize WiFi
	WiFi.mode(WIFI_STA);
	WiFi.begin(configuration.getWiFiSSID(), configuration.getWiFiPassword());
	Serial.printf("\nStarting WiFi with SSID '%s'.\n\n", configuration.getWiFiSSID());

	// Initialize local time from NTP Server
	renderLoadingPage();
	display.display();
	Serial.printf("\nStarting time synchronisation...");
	ezt::waitForSync();
	Serial.printf("finished\n\n");
	display.clear();
	display.display();
}

void setup() {
	Serial.begin(115200);

	// Initialize pin modes
	pinMode(REED_STATUS_LED, OUTPUT);
	pinMode(IR_STATUS_LED, OUTPUT);
	pinMode(REED_CONTACT, INPUT);
	pinMode(IR_SENSOR, INPUT);
	pinMode(BUTTON, INPUT);

	// Initialize OLED display
	display.init();
	display.flipScreenVertically();

	// Check wich mode to start
	if(digitalRead(BUTTON) == HIGH) {
		currentProgramMode = CONFIG;
	}

	// Setup the startet mode
	if(currentProgramMode == RUN)  {
		setup_run();
	}
	if(currentProgramMode == CONFIG)  {
		configuration.setupConfigMode(&display);
	}
}

void loop_run() {
  digitalWrite(IR_STATUS_LED, !digitalRead(IR_SENSOR));
  digitalWrite(REED_STATUS_LED, digitalRead(REED_CONTACT));
  input_thread();
  output_thread();
  wifi_thread(&configuration);
}

void loop() {
  // Loop the started mode
  if(currentProgramMode == RUN)  {
	loop_run();
  }
  if(currentProgramMode == CONFIG)  {
	configuration.loopConfigMode();
  }
}