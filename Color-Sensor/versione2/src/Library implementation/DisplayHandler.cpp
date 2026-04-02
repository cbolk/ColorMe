//DisplayHandler.h implementation
#include "DisplayHandler.h"

#define ROWGAP  16

void DisplayHandler :: begin(){
    {
            theDisplay.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
            theDisplay.clearDisplay();
            theDisplay.setTextSize(1);
            theDisplay.setTextColor(SSD1306_WHITE);
        }
}
void DisplayHandler :: print(String row1){
    theDisplay.clearDisplay();
    theDisplay.setCursor(0,0);
    theDisplay.print(row1);
    theDisplay.theDisplay();
}
void DisplayHandler :: print(String row1, String row2){
    theDisplay.clearDisplay();
    theDisplay.setCursor(0,0);
    theDisplay.print(row1);
    theDisplay.setCursor(0,ROWGAP);
    theDisplay.print(row2);
    theDisplay.theDisplay();
}

void DisplayHandler :: displayColor(RGB rgb) {
    char strRGB[SCREEN_WIDTH];
    char strHEX[8];

    theDisplay.clearDisplay();
    theDisplay.setCursor(0, 0);
    theDisplay.println("Colore rilevato");
    theDisplay.setCursor(0, ROWGAP);
    sprintf(strRGB, "R: %03d G: %03d B: %03d", rgb.r, rgb.g, rgb.b);
    theDisplay.println(strRGB);
    theDisplay.setCursor(0, ROWGAP*2);
    sprintf(strHEX, "#%02x%02x%02x", rgb.r, rgb.g, rgb.b);
    theDisplay.println(strHEX);
    theDisplay.theDisplay();
    
    return COLOR_UPDATED;
}