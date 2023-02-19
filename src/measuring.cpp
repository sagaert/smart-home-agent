#include <agent.hpp>

SignalStabilizer::SignalStabilizer(int inputPin, int triggerStatus, unsigned long stabilizationInterval, void (*callback)()) :
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
			this->callback();
			this->triggered = true;
		}
	}
}

// Generating an UTC Timestamp in RFC3339 with milliseconds:
void someMethod() {
	UTC.dateTime(RFC3339_EXT);
}
