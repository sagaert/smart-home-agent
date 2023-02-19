#include <agent.hpp>

PageRenderer::PageRenderer() {}

std::string PageRenderer::getNameOfDay(uint8_t d) {
	switch(d) {
		case SUNDAY: return "Sonntag";
		case MONDAY: return "Montag";
		case TUESDAY: return "Dienstag";
		case WEDNESDAY: return "Mittwoch";
		case THURSDAY: return "Donnerstag";
		case FRIDAY: return "Freitag";
		case SATURDAY: return "Samstag";
		default: return "Unbekannt";
	}
}

std::string PageRenderer::getNameOfDaylightSavingTime(bool dst) {
	if(dst) {
		return "Sommerzeit";
	} else {
		return "Normalzeit";
	}
}

void PageRenderer::renderLoadingPage(SH1106Wire& display) {
	display.setFont(ArialMT_Plain_10);
	display.setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
	display.drawString(64, 32, "Verbinde WLAN und\nsynchronisiere Zeit...");
}

void PageRenderer::renderHomePage(SH1106Wire& display) {
	display.setFont(ArialMT_Plain_24);
	display.setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
	display.drawString(64, 32, "Welcome");
}

void PageRenderer::renderWifiPage(SH1106Wire& display) {
	display.setFont(ArialMT_Plain_10);
	display.setTextAlignment(TEXT_ALIGN_LEFT);
	display.drawString(0, 0, "WLAN");
	display.drawLine(0, 12, 127, 12);
	switch(WiFi.status()) {
		case WL_CONNECTED:
			char wifiName[32];
			char wifiAddress[32];
			char signalStrength[32];
			char wifiChannel[16];
			sprintf(wifiName, "SSID: %s", WiFi.SSID().c_str());
			sprintf(wifiAddress, "IP: %s", WiFi.localIP().toString().c_str());
			sprintf(signalStrength, "Signal: %d%%", (WiFi.RSSI() + 100)* 2 );
			sprintf(wifiChannel, "Kanal: %d", WiFi.channel());
			display.setFont(ArialMT_Plain_10);
			display.setTextAlignment(TEXT_ALIGN_LEFT);
			display.drawString(0, 15, "Status: Verbunden");
			display.drawString(0, 27, wifiName);
			display.drawString(0, 39, wifiAddress);
			display.drawString(0, 51, wifiChannel);
			display.setTextAlignment(TEXT_ALIGN_RIGHT);
			display.drawString(127, 51, signalStrength);
			break;
		default:
			display.setFont(ArialMT_Plain_10);
			display.setTextAlignment(TEXT_ALIGN_LEFT);
			display.drawString(0, 15, "Status: Verbinden...");
			break;
	}
}

void PageRenderer::renderTimePage(SH1106Wire& display, Timezone& timezone) {
	display.setFont(ArialMT_Plain_10);
	display.setTextAlignment(TEXT_ALIGN_LEFT);
	display.drawString(0, 0, "Zeit");
	display.drawLine(0, 12, 127, 12);
	char dateString[36];
	char timeString[36];
	sprintf(dateString, "%02d.%02d.%04d", timezone.day(), timezone.month(), timezone.year());
	sprintf(timeString, "%02d:%02d:%02d", timezone.hour(), timezone.minute(), timezone.second());
	display.setFont(ArialMT_Plain_10);
	display.setTextAlignment(TEXT_ALIGN_LEFT);
	display.drawString(0, 15, this->getNameOfDay(timezone.weekday()).c_str());
	display.drawString(0, 45, timezone.getTimezoneName());
	display.setTextAlignment(TEXT_ALIGN_RIGHT);
	display.drawString(127, 15, dateString);
	display.drawString(127, 45, this->getNameOfDaylightSavingTime(timezone.isDST()).c_str());
	display.setFont(ArialMT_Plain_16);
	display.drawString(127, 27, timeString);
}

void PageRenderer::renderElectricityPage(SH1106Wire& display) {
	display.setFont(ArialMT_Plain_10);
	display.setTextAlignment(TEXT_ALIGN_LEFT);
	display.drawString(0, 0, "Stromverbrauch");
	display.drawLine(0, 12, 127, 12);
}

void PageRenderer::renderConfigPage(SH1106Wire& display, const char* ssid, const char* password, const char* ip) {
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
}