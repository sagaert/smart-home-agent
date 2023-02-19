#include <agent.hpp>

Timezone tz;
void initTimezone() {
	tz.setLocation("de");
}

String getNameOfDay(uint8_t d) {
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

String getNameOfDaylightSavingTime(bool dst) {
	if(dst) {
		return "Sommerzeit";
	} else {
		return "Normalzeit";
	}
}

void renderLoadingPage(SH1106Wire* display) {
	display->setFont(ArialMT_Plain_10);
	display->setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
	display->drawString(64, 32, "Verbinde WLAN und\nsynchronisiere Zeit...");
}

void renderHomePage(SH1106Wire* display) {
	display->setFont(ArialMT_Plain_24);
	display->setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
	display->drawString(64, 32, "Welcome");
}

void renderWifiPage(SH1106Wire* display) {
	display->setFont(ArialMT_Plain_10);
	display->setTextAlignment(TEXT_ALIGN_LEFT);
	display->drawString(0, 0, "WLAN");
	display->drawLine(0, 12, 127, 12);
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
			display->setFont(ArialMT_Plain_10);
			display->setTextAlignment(TEXT_ALIGN_LEFT);
			display->drawString(0, 15, "Status: Verbunden");
			display->drawString(0, 27, wifiName);
			display->drawString(0, 39, wifiAddress);
			display->drawString(0, 51, wifiChannel);
			display->setTextAlignment(TEXT_ALIGN_RIGHT);
			display->drawString(127, 51, signalStrength);
			break;
		default:
			display->setFont(ArialMT_Plain_10);
			display->setTextAlignment(TEXT_ALIGN_LEFT);
			display->drawString(0, 15, "Status: Verbinden...");
			break;
	}
}

void renderTimePage(SH1106Wire* display) {
	display->setFont(ArialMT_Plain_10);
	display->setTextAlignment(TEXT_ALIGN_LEFT);
	display->drawString(0, 0, "Zeit");
	display->drawLine(0, 12, 127, 12);
	char dateString[36];
	char timeString[36];
	sprintf(dateString, "%02d.%02d.%04d", tz.day(), tz.month(), tz.year());
	sprintf(timeString, "%02d:%02d:%02d", tz.hour(), tz.minute(), tz.second());
	display->setFont(ArialMT_Plain_10);
	display->setTextAlignment(TEXT_ALIGN_LEFT);
	display->drawString(0, 15, getNameOfDay(tz.weekday()));
	display->drawString(0, 45, tz.getTimezoneName());
	display->setTextAlignment(TEXT_ALIGN_RIGHT);
	display->drawString(127, 15, dateString);
	display->drawString(127, 45, getNameOfDaylightSavingTime(tz.isDST()));
	display->setFont(ArialMT_Plain_16);
	display->drawString(127, 27, timeString);
}

void renderElectricityPage(SH1106Wire* display) {
	display->setFont(ArialMT_Plain_10);
	display->setTextAlignment(TEXT_ALIGN_LEFT);
	display->drawString(0, 0, "Stromverbrauch");
	display->drawLine(0, 12, 127, 12);
}

