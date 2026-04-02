//DisplayHandler.h implementation
#include "DisplayHandler.h"


void DisplayHandler :: begin(){
    {
            display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
            display.clearDisplay();
            display.setTextSize(1);
            display.setTextColor(SSD1306_WHITE);
        }
}
void DisplayHandler :: print(String row1){
    display.clearDisplay();
    display.setCursor(0,0);
    display.print(row1);
    display.display();
}
void DisplayHandler :: print(String row1, String row2){
    display.clearDisplay();
    display.setCursor(0,0);
    display.print(row1);
    display.setCursor(0,16);
    display.print(row2);
    display.display();
}