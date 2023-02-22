#include <agent.hpp>

ConnectionManager::ConnectionManager(unsigned long connectionCheckInterval) :
	connectionCheckInterval(connectionCheckInterval), mqttClient(PubSubClient(this->wifiClient)) {
		this->lastConnectionCheck = 0UL;
}

void ConnectionManager::checkConnections(AgentConfiguration& config) {
	Serial.printf("Checking WiFi connection...");
	if(!WiFi.isConnected()) {
		Serial.println("not connected! Start reconnect.");
		WiFi.disconnect();
		WiFi.begin(config.getWiFiSSID(), config.getWiFiPassword());
	} else {
		Serial.println("connected");
		Serial.printf("Checking MQTT connection...");
		if(!this->mqttClient.connected()) {
			Serial.println("not connected! Start reconnect.");
			this->mqttClient.connect("smart-home-agent", config.getMQTTUsername(), config.getMQTTPassword());
		} else {
			Serial.println("connected");
		}
	}
}

bool ConnectionManager::isConnected() {
	return WiFi.isConnected() && this->mqttClient.connected();
}

void ConnectionManager::loop(AgentConfiguration& config) {
	unsigned long now = millis();
	if(now - this->lastConnectionCheck > this->connectionCheckInterval) {
		this->checkConnections(config);
		this->lastConnectionCheck = now;
	}
}

void ConnectionManager::setup(AgentConfiguration& config) {
	WiFi.mode(WIFI_STA);
	this->mqttClient.setServer(config.getMQTTHost(), config.getMQTTPort());
	this->checkConnections(config);
}

void ConnectionManager::sendMeasurement(AgentConfiguration& config, long value) {
	std::string json_t = "{\"time\":\"%s\",\"value\":%d,\"sensor\":\"main\",\"measurement\":\"consumption\",\"field\":\"electricity\"}";
	char json[128];
	sprintf(json, json_t.c_str(), UTC.dateTime(RFC3339_EXT).c_str(), value);
	this->mqttClient.publish(config.getMQTTTopic(), json);
	Serial.println("Sending measurement:");
	Serial.println(json);
}