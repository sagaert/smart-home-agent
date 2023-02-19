#include <agent.hpp>

AgentConfiguration::AgentConfiguration() :
	server(nullptr), wifiSSID(""), wifiPawword(""), mqttURL(""), mqttUsername(""), mqttPassword(""), mqttTopic("") {
}
	
char* AgentConfiguration::getWiFiSSID() {
	return this->wifiSSID;
}

char* AgentConfiguration::getWiFiPassword() {
	return this->wifiPawword;
}

char* AgentConfiguration::getMQTTURL() {
	return this->mqttURL;
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
	mqtt.getString("url", this->mqttURL, 63);
	mqtt.getString("username", this->mqttUsername, 63);
	mqtt.getString("password", this->mqttPassword, 63);
	mqtt.getString("topic", this->mqttURL, 63);
}

void AgentConfiguration::setupConfigMode(UserInterface& ui) {
	esp32config::Configuration config("SHO Config", {
			new esp32config::Namespace("WiFi", "wifi", {
				new esp32config::Entry("SSID", esp32config::TEXT, "ssid"),
				new esp32config::Entry("Password", esp32config::PASSWORD, "password")
			}),
			new esp32config::Namespace("MQTT", "mqtt", {
				new esp32config::Entry("URL", esp32config::TEXT, "url"),
				new esp32config::Entry("Username", esp32config::TEXT, "username"),
				new esp32config::Entry("Password",esp32config::PASSWORD, "password"),
				new esp32config::Entry("Topic",esp32config::TEXT, "topic", false, "smart-home")
			})}
		);
	this->server = new esp32config::Server(config);
	const char* ssid = "Smart Home Agent";
	char password[9];
	createPassword(&password[0], 8);
	IPAddress ip(192, 168, 10, 1);
	this->server->begin(ssid, password ,ip);
	renderConfigPage(ui.getDisplay(), ssid, password, ip.toString().c_str());
}

const char alphanum[] = "0123456789";

void AgentConfiguration::createPassword(char* password, int len) {
	for(int i = 0 ; i < len ; i++) {
		password[i] = alphanum[random(sizeof(alphanum) - 1)];
	}
	password[len] = '\0';
}

void AgentConfiguration::loopConfigMode() {
	this->server->loop();
}

void AgentConfiguration::renderConfigPage(SH1106Wire& display, const char* ssid, const char* password, const char* ip) {
	display.clear();

	display.setFont(ArialMT_Plain_10);
	display.setTextAlignment(TEXT_ALIGN_LEFT);
	display.drawString(0, 0, "Konfigurations-Modus");
	display.drawLine(0, 12, 127, 12);

	char configSSID[32];
	char configPassword[32];
	char configIP[32];
    sprintf(configSSID, "SSID: %s", ssid);
    sprintf(configPassword, "Password: %s", password);
    sprintf(configIP, "http://%s", ip);

	display.drawString(0, 15, "Verbindungsdaten:");
	display.drawString(0, 27, configSSID);
	display.drawString(0, 39, configPassword);
	display.drawString(0, 51, configIP);

	display.display();
}