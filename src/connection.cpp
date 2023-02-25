#include <agent.hpp>

ConnectionManager::ConnectionManager(unsigned long connectionCheckInterval) :
	connectionCheckInterval(connectionCheckInterval), mqttClient(MQTTClient(1024)) {
		this->lastConnectionCheck = 0UL;
}

bool ConnectionManager::checkConnections(AgentConfiguration& config) {
	if(!WiFi.isConnected()) {
		WiFi.disconnect();
		WiFi.begin(config.getWiFiSSID(), config.getWiFiPassword());
		return false;
	} else {
		if(!this->mqttClient.connected()) {
			return this->mqttClient.connect("smart-home-agent", config.getMQTTUsername(), config.getMQTTPassword());
		} else {
			return true;
		}
	}
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
	this->mqttClient.begin(config.getMQTTHost(), config.getMQTTPort(), this->wifiClient);
	this->checkConnections(config);
}

void ConnectionManager::sendMeasurement(AgentConfiguration& config, double value, const std::string& field) {
	if(this->checkConnections(config)) {
		std::string json_t = "{\"time\":\"%s\",\"value\":%16.2f,\"sensor\":\"main\",\"measurement\":\"consumption\",\"field\":\"%s\"}";
		char json[256];
		sprintf(json, json_t.c_str(), UTC.dateTime(RFC3339_EXT).c_str(), value, field.c_str());
		this->mqttClient.publish(config.getMQTTTopic(), json);
	}
}