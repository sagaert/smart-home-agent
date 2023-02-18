#ifndef AGENT_HPP
#define AGENT_HPP

#include <Arduino.h>
#include <WiFi.h>
#include <ezTime.h>
#include <Wire.h>
#include <SH1106Wire.h>
#include <Preferences.h>
#include <esp32-config-lib.hpp>

#define REED_STATUS_LED 25
#define REED_CONTACT 27
#define IR_STATUS_LED 18                                                                                                                                                                                         
#define IR_SENSOR 17  
#define BUTTON 26                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   

class AgentConfiguration {
	private:
		esp32config::Server* server;
		char wifiSSID[32];
		char wifiPawword[64];
		char mqttURL[64];
		char mqttUsername[64];
		char mqttPassword[64];
		char mqttTopic[64];
		void renderConfigPage(SH1106Wire* display, const char* ssid, const char* password, const char* ip);
		void createPassword(char* password, int len);
	public:
		AgentConfiguration();
		char* getWiFiSSID();
		char* getWiFiPassword();
		char* getMQTTURL();
		char* getMQTTUsername();
		char* getMQTTPassword();
		char* getMQTTTopic();
		void load();
		void setupConfigMode(SH1106Wire* display);
		void loopConfigMode();
};

#endif