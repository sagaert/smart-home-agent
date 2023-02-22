#include <agent.hpp>

AgentConfiguration::AgentConfiguration() :
	server(nullptr), wifiSSID(""), wifiPawword(""), mqttHost(""), mqttPort(0), mqttUsername(""), mqttPassword(""), mqttTopic("") {
}
	
char* AgentConfiguration::getWiFiSSID() {
	return this->wifiSSID;
}

char* AgentConfiguration::getWiFiPassword() {
	return this->wifiPawword;
}

char* AgentConfiguration::getMQTTHost() {
	return this->mqttHost;
}

int AgentConfiguration::getMQTTPort() {
	return this->mqttPort;
}

char* AgentConfiguration::getMQTTUsername() {
	return this->mqttUsername;
}

char* AgentConfiguration::getMQTTPassword() {
	return this->mqttPassword;
}

char* AgentConfiguration::getMQTTTopic() {
	return this->mqttTopic;
}

void AgentConfiguration::load() {
	// Read preferences
	Preferences wifi, mqtt;
	wifi.begin("wifi", true);
	mqtt.begin("mqtt", true);

	// Initialize WiFi
	wifi.getString("ssid", this->wifiSSID, 31);
	wifi.getString("password", this->wifiPawword, 63);

	// Initialize MQTT
	mqtt.getString("host", this->mqttHost, 63);
	this->mqttPort = mqtt.getInt("port");
	mqtt.getString("username", this->mqttUsername, 63);
	mqtt.getString("password", this->mqttPassword, 63);
	mqtt.getString("topic", this->mqttTopic, 63);

}

void AgentConfiguration::setupConfigMode(UserInterface& ui) {
	esp32config::Configuration config("SHO Config", {
			new esp32config::Namespace("WiFi", "wifi", {
				new esp32config::Entry("SSID", esp32config::TEXT, "ssid"),
				new esp32config::Entry("Password", esp32config::PASSWORD, "password")
			}),
			new esp32config::Namespace("MQTT", "mqtt", {
				new esp32config::Entry("Host", esp32config::TEXT, "host"),
				new esp32config::Entry("Port", esp32config::INTEGER, "port"),
				new esp32config::Entry("Username", esp32config::TEXT, "username"),
				new esp32config::Entry("Password",esp32config::PASSWORD, "password"),
				new esp32config::Entry("Topic",esp32config::TEXT, "topic", false, "smart-home")
			})}
		);
	this->server = new esp32config::Server(config);
	std::string ssid = "Smart Home Agent";
	std::string password = createPassword(8);
	IPAddress ip(192, 168, 10, 1);
	this->server->begin(ssid, password ,ip);
	ui.showConfigMessage(ssid.c_str(), password.c_str(), ip.toString().c_str());
}

const char alphanum[] = "0123456789";

std::string AgentConfiguration::createPassword(int len) {
	char *password = new char[len+1];
	for(int i = 0 ; i < len ; i++) {
		password[i] = alphanum[random(sizeof(alphanum) - 1)];
	}
	password[len] = '\0';
	return std::string(password);
}

void AgentConfiguration::loopConfigMode() {
	this->server->loop();
}
