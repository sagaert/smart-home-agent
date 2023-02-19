#include <agent.hpp>                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               

UserInterface::UserInterface(int displayAddress, int buttonPin, unsigned long screensaverTimeout, unsigned long displayRefreshInterval, unsigned long buttonStabilizerInterval) :
	buttonStabilizer(SignalStabilizer(buttonPin, LOW, buttonStabilizerInterval, std::bind(&UserInterface::buttonPressed, this))),
	display(SH1106Wire(displayAddress, SDA, SCL)),
	screensaverTimeout(screensaverTimeout),
	displayRefreshInterval(displayRefreshInterval) {
	this->currentPage = HOME_PAGE;
	this->displayOn = false;
	this->buttonPressedTime = 0UL;
	this->lastRenderingTime = 0UL;
}

void UserInterface::setup() {
	this->display.init();
	this->display.flipScreenVertically();
}

void UserInterface::showInitMessage() {
	renderLoadingPage(this->display);
	this->display.display();
}

void UserInterface::switchDisplayOff() {
	this->displayOn = false;
}

void UserInterface::switchDisplayOn() {
	this->displayOn = true;
}

void UserInterface::setTimezone() {
	this->timezone.setLocation(TIMEZONE);
}

void UserInterface::buttonPressed() {
	this->buttonPressedTime = millis();
	if(this->displayOn) {
		// Switch to next page
		switch(this->currentPage) {
			case HOME_PAGE:
				this->currentPage = WIFI_PAGE;
				break;
			case WIFI_PAGE:
				this->currentPage = TIME_PAGE;
				break;
			case TIME_PAGE:
				this->currentPage = ELECTRICITY_PAGE;
				break;
			default:
				this->currentPage = HOME_PAGE;
				break;
		}
	} else {
		// Activate display and switch to home page
		this->displayOn = true;
		this->currentPage = HOME_PAGE;
	}
}

void UserInterface::loop() {
	unsigned long now = millis();

	// Loop the input thread
	this->buttonStabilizer.loop();

	// Switch display off, if screensaver should be activated
    this->displayOn = !(now - this->buttonPressedTime > this->screensaverTimeout);

	// Update display only when refresh interval is reached
    if(now - this->lastRenderingTime > this->displayRefreshInterval) {
		if(this->displayOn) {
			// Update display
			this->display.clear();
			switch(this->currentPage) {
				case WIFI_PAGE:
					renderWifiPage(this->display);
					break;
				case TIME_PAGE:
					renderTimePage(this->display, this->timezone);
					break;
				case ELECTRICITY_PAGE:
					renderElectricityPage(this->display);
					break;
				default:
					renderHomePage(this->display);
					break;
			}
			this->display.display();
		} else {
			this->display.clear();
			this->display.display();
		}
	}
}

void UserInterface::renderConfigPage(const char* ssid, const char* password, const char* ip) {
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