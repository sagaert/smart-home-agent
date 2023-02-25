#include <agent.hpp>                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               

AgentConfiguration configuration;

ConnectionManager connectionManager;

MeasuringController electricityMeasuringController(connectionManager, configuration, "electricity", IR_SENSOR, LOW, 500UL, IR_STATUS_LED, 750.0);

MeasuringController gasMeasuringController(connectionManager, configuration, "gas", REED_CONTACT, HIGH, 5000UL, REED_STATUS_LED, 0.1);

UserInterface ui(electricityMeasuringController, gasMeasuringController, DISPLAY_ADDRESS, BUTTON_PIN);

enum ProgramMode {
	RUN,
	CONFIG
};

ProgramMode currentProgramMode = RUN;

void setup_run() {
	// Load configuration
	configuration.load();

	// Initialize WiFi and MQTT client
	connectionManager.setup(configuration);

	// Initialize local time from NTP Server and set Timezone
	ui.showInitMessage();
	ezt::waitForSync();
	ui.setTimezone();
	ui.switchDisplayOff();

	// Initialize measuring controller
	electricityMeasuringController.setup();
	gasMeasuringController.setup();
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
	ui.loop();
	connectionManager.loop(configuration);
	electricityMeasuringController.loop();
	gasMeasuringController.loop();
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