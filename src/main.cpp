#include <agent.hpp>                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               

AgentConfiguration configuration;

UserInterface ui(DISPLAY_ADDRESS, BUTTON_PIN);

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
	ui.showInitMessage();
	Serial.printf("\nStarting time synchronisation...");
	ezt::waitForSync();
	initTimezone();
	Serial.printf("finished\n\n");
	ui.switchDisplayOff();
}

void setup() {
	Serial.begin(115200);

	// Initialize pin modes
	pinMode(REED_STATUS_LED, OUTPUT);
	pinMode(IR_STATUS_LED, OUTPUT);
	pinMode(REED_CONTACT, INPUT);
	pinMode(IR_SENSOR, INPUT);
	pinMode(BUTTON_PIN, INPUT);

	// Initialize UI
	ui.setup();

	// Check wich mode to start
	if(digitalRead(BUTTON_PIN) == HIGH) {
		currentProgramMode = CONFIG;
	}

	// Setup the startet mode
	if(currentProgramMode == RUN)  {
		setup_run();
	}
	if(currentProgramMode == CONFIG)  {
		configuration.setupConfigMode(ui);
	}
}

void loop_run() {
  digitalWrite(IR_STATUS_LED, !digitalRead(IR_SENSOR));
  digitalWrite(REED_STATUS_LED, digitalRead(REED_CONTACT));
  ui.loop();
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