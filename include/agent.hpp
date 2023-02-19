#ifndef AGENT_HPP
#define AGENT_HPP

#include <Arduino.h>
#include <WiFi.h>
#include <ezTime.h>
#include <Wire.h>
#include <SH1106Wire.h>
#include <Preferences.h>
#include <esp32-config-lib.hpp>

#define VERSION "0.0.0"

#define REED_STATUS_LED 25
#define REED_CONTACT 27
#define IR_STATUS_LED 18                                                                                                                                                                                         
#define IR_SENSOR 17  
#define BUTTON_PIN 26                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   
#define DISPLAY_ADDRESS 0x3c

#define TIMEZONE "de"

class UserInterface;
class AgentConfiguration;
class SignalStabilizer;
class PageRenderer;
class ConnectionManager;

enum Page {	HOME_PAGE, WIFI_PAGE, TIME_PAGE, ELECTRICITY_PAGE, GAS_PAGE };

class ConnectionManager {
	private:
		unsigned long lastConnectionCheck;
		const unsigned long connectionCheckInterval;
	public:
		ConnectionManager(unsigned long connectionCheckInterval = 180000UL);
		void loop(AgentConfiguration& config);
};

class PageRenderer {
	private:
		std::string getNameOfDay(uint8_t d);
		std::string getNameOfDaylightSavingTime(bool dst);
	public:
		PageRenderer();
		void renderHomePage(SH1106Wire& display);
		void renderWifiPage(SH1106Wire& display);
		void renderTimePage(SH1106Wire& display, Timezone& timezone);
		void renderElectricityPage(SH1106Wire& display);
		void renderLoadingPage(SH1106Wire& display);
		void renderConfigPage(SH1106Wire& display, const char* ssid, const char* password, const char* ip);
};

class AgentConfiguration {
	private:
		esp32config::Server* server;
		char wifiSSID[32];
		char wifiPawword[64];
		char mqttURL[64];
		char mqttUsername[64];
		char mqttPassword[64];
		char mqttTopic[64];
		std::string createPassword(int len);
	public:
		AgentConfiguration();
		char* getWiFiSSID();
		char* getWiFiPassword();
		char* getMQTTURL();
		char* getMQTTUsername();
		char* getMQTTPassword();
		char* getMQTTTopic();
		void load();
		void setupConfigMode(UserInterface& ui);
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
		std::function<void(void)> callback;
	public:
		SignalStabilizer(int inputPin, int triggerStatus, unsigned long stabilizationInterval, std::function<void(void)> callback);
		void loop();
};

class UserInterface {
	private:
		SH1106Wire display;
		SignalStabilizer buttonStabilizer;
		Timezone timezone;
		PageRenderer renderer;
		const unsigned long screensaverTimeout;
		const unsigned long displayRefreshInterval;
		Page currentPage;
		bool displayOn;
		unsigned long buttonPressedTime;
		unsigned long lastRenderingTime;
		void buttonPressed();
	public:
		UserInterface(int displayAddress, int buttonPin, unsigned long screensaverTimeout = 60000UL, unsigned long displayRefreshInterval = 100UL, unsigned long buttonStabilizerInterval = 100UL);
		void loop();
		void setup();
		void switchDisplayOff();
		void switchDisplayOn();
		void setTimezone();
		void showInitMessage();
		void showConfigMessage(const char* ssid, const char* password, const char* ip);
};

#endif