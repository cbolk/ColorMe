//DisplayHandler.h implementation
#include "DisplayHandler.h"

void DisplayHandler::begin() {
    myDisplay.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
    myDisplay.clearDisplay();
    myDisplay.setTextSize(1);
    myDisplay.setTextColor(SSD1306_WHITE);
}

void DisplayHandler::print(String row1) {
    myDisplay.clearDisplay();
    myDisplay.setCursor(0, 0);
    myDisplay.print(row1);
    myDisplay.display();
}

void DisplayHandler::print(String row1, String row2) {
    myDisplay.clearDisplay();
    myDisplay.setCursor(0, 0);
    myDisplay.print(row1);
    myDisplay.setCursor(0, 16);
    myDisplay.print(row2);
    myDisplay.display();
}

void DisplayHandler::displayColor(int r, int g, int b) {
    char strRGB[MYSCREEN_WIDTH + 1];
    char strHEX[8];

    myDisplay.clearDisplay();
    myDisplay.setCursor(0, 0);
    myDisplay.println("Colore rilevato");
    myDisplay.setCursor(0, ROWGAP);
    sprintf(strRGB, "R: %03d G: %03d B: %03d", r, g, b);
    myDisplay.println(strRGB);
    myDisplay.setCursor(0, ROWGAP * 2);
    sprintf(strHEX, "#%02x%02x%02x", r, g, b);
    myDisplay.println(strHEX);
    myDisplay.display();
}