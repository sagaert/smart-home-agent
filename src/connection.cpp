#include <agent.hpp>

const unsigned long connectionCheckInterval = 180000UL; // Check WiFi-Connection every 3 minutes

void wifi_thread(AgentConfiguration* config) {
	static enum { ON, OFF } status = OFF;
	static unsigned long lastConnectionCheck = 0UL;
	if(millis() - lastConnectionCheck > connectionCheckInterval) {
		Serial.printf("Checking WiFi connection...");
		if(!WiFi.isConnected()) {
			Serial.println("not connected! Start reconnect.");
			WiFi.disconnect();
			WiFi.begin(config->getWiFiSSID(), config->getWiFiPassword());
		} else {
			Serial.println("connected");
		}
		lastConnectionCheck = millis();
	}
}

