#include <agent.hpp>

ConnectionManager::ConnectionManager(unsigned long connectionCheckInterval) :
	connectionCheckInterval(connectionCheckInterval) {
		this->lastConnectionCheck = 0UL;
}

void ConnectionManager::loop(AgentConfiguration& config) {
	unsigned long now = millis();
	if(now - this->lastConnectionCheck > this->connectionCheckInterval) {
		Serial.printf("Checking WiFi connection...");
		if(!WiFi.isConnected()) {
			Serial.println("not connected! Start reconnect.");
			WiFi.disconnect();
			WiFi.begin(config.getWiFiSSID(), config.getWiFiPassword());
		} else {
			Serial.println("connected");
		}
		this->lastConnectionCheck = now;
	}
}

