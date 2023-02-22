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

void MeasuringController::setup() {
	this->lastMeasured = millis();
	this->currentInterval = random(5000L, 10000L);
}

void MeasuringController::loop(AgentConfiguration& config, ConnectionManager& connManager) {
	digitalWrite(IR_STATUS_LED, !digitalRead(IR_SENSOR));
	digitalWrite(REED_STATUS_LED, digitalRead(REED_CONTACT));
	unsigned long now = millis();
	if(now - this->lastMeasured > this->currentInterval) {
		long value = random(500L, 3000L); // Fake measurement :-)
		connManager.sendMeasurement(config, value);
		this->lastMeasured = now;
		this->currentInterval = random(5000L, 10000L);
	}
}

// Generating an UTC Timestamp in RFC3339 with milliseconds:
void someMethod() {
	UTC.dateTime(RFC3339_EXT);
}
