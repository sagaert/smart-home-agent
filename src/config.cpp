#include <agent.hpp>

AgentConfiguration::AgentConfiguration() :
	server(nullptr), wifiSSID(""), wifiPawword(""), influxURL(""), influxToken(""), influxOrg(""), influxBucket("") {
}
	
char* AgentConfiguration::getWiFiSSID() {
	return this->wifiSSID;
}

char* AgentConfiguration::getWiFiPassword() {
	return this->wifiPawword;
}

char* AgentConfiguration::getInfluxURL() {
	return this->influxURL;
}

char* AgentConfiguration::getInfluxToken() {
	return this->influxToken;
}

char* AgentConfiguration::getInfluxOrg() {
	return this->influxOrg;
}

char* AgentConfiguration::getInfluxBucket() {
	return this->influxBucket;
}

void AgentConfiguration::load() {
	// Read preferences
	Preferences wifi, influx;
	wifi.begin("wifi", true);
	influx.begin("influx", true);

	// Initialize WiFi
	wifi.getString("ssid", this->wifiSSID, 31);
	wifi.getString("password", this->wifiPawword, 63);

	// Initialize Influx
	influx.getString("url", this->influxURL, 63);
	influx.getString("token", this->influxToken, 127);
	influx.getString("org", this->influxOrg, 63);
	influx.getString("bucket", this->influxBucket, 63);
}

void AgentConfiguration::setupConfigMode(UserInterface& ui) {
	esp32config::Configuration config("SHO Config", {
			new esp32config::Namespace("WiFi", "wifi", {
				new esp32config::Entry("SSID", esp32config::TEXT, "ssid"),
				new esp32config::Entry("Password", esp32config::PASSWORD, "password")
			}),
			new esp32config::Namespace("InfluxDB", "influx", {
				new esp32config::Entry("URL", esp32config::TEXT, "url"),
				new esp32config::Entry("Token", esp32config::PASSWORD, "token"),
				new esp32config::Entry("Org", esp32config::PASSWORD, "org"),
				new esp32config::Entry("Bucket", esp32config::PASSWORD, "bucket")
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
