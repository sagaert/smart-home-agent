#include <agent.hpp>

SignalStabilizer::SignalStabilizer(int inputPin, int triggerStatus, unsigned long stabilizationInterval, std::function<void(void)> callback) :
	inputPin(inputPin), triggerStatus(triggerStatus), stabilizationInterval(stabilizationInterval), callback(callback) {
	this->lastStatus = digitalRead(inputPin);
	this->lastChange = millis();
	this->triggered = false;
}

void SignalStabilizer::loop() {
	unsigned long now = millis();
	int currentStatus = digitalRead(this->inputPin);
	if(currentStatus != this->lastStatus) {
		this->lastStatus = currentStatus;
		this->lastChange = now;
		this->triggered = false;
	} else {
		bool shouldTrigger = currentStatus == this->triggerStatus && now - this->stabilizationInterval > this->lastChange;
		if(!triggered && shouldTrigger) {
			if(this->callback != 0) {
				this->callback();
			}
			this->triggered = true;
		}
	}
}

MeasuringController::MeasuringController(ConnectionManager& connManager, AgentConfiguration& config, const std::string& field, const int inputPin, const int triggerStatus, const unsigned long sensorStabilizerInterval, const int outputLED, const double consumptionPerTrigger) :
	config(config), connManager(connManager), field(field), inputPin(inputPin), triggerStatus(triggerStatus), outputLED(outputLED), consumptionPerTrigger(consumptionPerTrigger),
	sensorStabilizer(SignalStabilizer(inputPin, triggerStatus, sensorStabilizerInterval, std::bind(&MeasuringController::consumptionMeasured, this))) {
}

void MeasuringController::setup() {
	this->lastMeasured = 0UL;
	this->currentConsumption = 0.0;
}

void MeasuringController::loop() {
	// Update the status LED
	digitalWrite(this->outputLED, digitalRead(this->inputPin) == this->triggerStatus);

	// Loop the sensor stabilizer
	this->sensorStabilizer.loop();
}

void MeasuringController::consumptionMeasured() {
	unsigned long now = millis();
	if(this->lastMeasured > 0UL) {
		unsigned long period = now - this->lastMeasured;
		this->currentConsumption = 3600000UL / (double)(period) * this->consumptionPerTrigger;
		this->connManager.sendMeasurement(this->config, this->currentConsumption, this->field);
	}
	this->lastMeasured = now;
}

double MeasuringController::getCurrentConsumption() {
	return this->currentConsumption;
}