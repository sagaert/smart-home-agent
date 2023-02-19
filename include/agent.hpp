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
		void setupConfigMode();
		void loopConfigMode();
};

class SignalStabilizer {
	private:
		unsigned long lastChange;
		int  lastStatus;
		bool triggered;
		unsigned long stabilizationInterval;
		int inputPin;
		int triggerStatus;
		void (*callback)();
	public:
		SignalStabilizer(int inputPin, int triggerStatus, unsigned long stabilizationInterval, void (*callback)());
		void loop();
};

void wifi_thread(AgentConfiguration* config);

SH1106Wire* getDisplay();
void displayInit();
void displayOff();
void showInitMessage();
void updateDisplay();
void buttonPressed();

void initTimezone();
void renderLoadingPage(SH1106Wire* display);
void renderHomePage(SH1106Wire* display);
void renderWifiPage(SH1106Wire* display);
void renderTimePage(SH1106Wire* display);
void renderElectricityPage(SH1106Wire* display);

#endif