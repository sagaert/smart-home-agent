#include <agent.hpp>                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               

SignalStabilizer buttonStabilizer(BUTTON, LOW, 100UL, buttonPressed);

AgentConfiguration configuration;

enum ProgramMode {
	RUN,
	CONFIG
};

ProgramMode currentProgramMode = RUN;

void setup_run() {
	// Load configuration
	configuration.load();

	// Initialize WiFi
	WiFi.mode(WIFI_STA);
	WiFi.begin(configuration.getWiFiSSID(), configuration.getWiFiPassword());
	Serial.printf("\nStarting WiFi with SSID '%s'.\n\n", configuration.getWiFiSSID());

	// Initialize local time from NTP Server and set Timezone
	showInitMessage();
	Serial.printf("\nStarting time synchronisation...");
	ezt::waitForSync();
	initTimezone();
	Serial.printf("finished\n\n");
	displayOff();
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
	displayInit();

	// Check wich mode to start
	if(digitalRead(BUTTON) == HIGH) {
		currentProgramMode = CONFIG;
	}

	// Setup the startet mode
	if(currentProgramMode == RUN)  {
		setup_run();
	}
	if(currentProgramMode == CONFIG)  {
		configuration.setupConfigMode();
	}
}

void loop_run() {
  digitalWrite(IR_STATUS_LED, !digitalRead(IR_SENSOR));
  digitalWrite(REED_STATUS_LED, digitalRead(REED_CONTACT));
  buttonStabilizer.loop();
  updateDisplay();
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