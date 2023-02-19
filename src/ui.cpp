#include <agent.hpp>                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               


SH1106Wire display(0x3c, SDA, SCL);

static boolean displayOn = false;
enum pages { HOME_PAGE, WIFI_PAGE, TIME_PAGE, ELECTRICITY_PAGE };
static pages currentPage = HOME_PAGE;
static unsigned long buttonPressedTime = 0;
static const unsigned long screensaverTimeout = 60000UL; // Switch off display after 1 minute without input
const unsigned long displayRefreshInterval = 100UL; // Refresh the display 10 times a second

SH1106Wire* getDisplay() {
	return &display;
}

void displayInit() {
	display.init();
	display.flipScreenVertically();
}

void showInitMessage() {
	renderLoadingPage(&display);
	display.display();
}

void displayOff() {
	display.clear();
	display.display();
}

void buttonPressed() {
	buttonPressedTime = millis();
	if(displayOn) {
		// Switch to next page
		switch(currentPage) {
			case HOME_PAGE:
				currentPage = WIFI_PAGE;
				break;
			case WIFI_PAGE:
				currentPage = TIME_PAGE;
				break;
			case TIME_PAGE:
				currentPage = ELECTRICITY_PAGE;
				break;
			default:
				currentPage = HOME_PAGE;
				break;
		}
	} else {
		// Activate display and switch to home page
		displayOn = true;
		currentPage = HOME_PAGE;
	}
}

void updateDisplay() {
  static enum { ON, OFF } status = OFF;
  static unsigned long lastRenderingTimestamp = 0UL;
  
  switch(status) {
    case ON:
      // Switch display off, if screensaver should be activated
      displayOn = !(millis() - buttonPressedTime > screensaverTimeout);
      if(displayOn) {
        if(millis() - lastRenderingTimestamp > displayRefreshInterval) {
          // Update display
          display.clear();
          switch(currentPage) {
            case WIFI_PAGE:
              renderWifiPage(&display);
              break;
            case TIME_PAGE:
              renderTimePage(&display);
              break;
            case ELECTRICITY_PAGE:
              renderElectricityPage(&display);
              break;
            default:
              renderHomePage(&display);
              break;
          }
          display.display();
          lastRenderingTimestamp = millis();
        }
      } else {
        // Switch off display
		displayOff();
        status = OFF;
      }
      break;
    case OFF:
      if(displayOn) {
        // Swtich on display
        status = ON;
      }
      break;
  }
  
}