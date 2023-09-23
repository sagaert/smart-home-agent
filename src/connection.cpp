#include <agent.hpp>

ConnectionManager::ConnectionManager(unsigned long connectionCheckInterval) :
	connectionCheckInterval(connectionCheckInterval) {
		this->lastConnectionCheck = 0UL;
}

bool ConnectionManager::checkConnections(AgentConfiguration& config) {
	if(!WiFi.isConnected()) {
		WiFi.disconnect();
		WiFi.begin(config.getWiFiSSID(), config.getWiFiPassword());
		return false;
	} else {
		return true;
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
	this->influxDBClient.setConnectionParams(config.getInfluxURL(), config.getInfluxOrg(), config.getInfluxBucket(), config.getInfluxToken(), config.getCertRootCA());
	this->checkConnections(config);
}

void ConnectionManager::sendMeasurement(AgentConfiguration& config, double value, const std::string& field) {
	if(this->checkConnections(config)) {
		Point point("consumption");
		point.addField(field.c_str(), value);
		point.addTag("sensor", "main");
		if(!this->influxDBClient.writePoint(point)) {
			Serial.printf("Fehler beim Senden der Daten an die InfluxDB (Status Code %d): %s\n", this->influxDBClient.getLastStatusCode(), this->influxDBClient.getLastErrorMessage().c_str());
		}
	}
}