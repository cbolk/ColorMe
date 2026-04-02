#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "include/DisaplyHanlder.h"

DisplayHandler display;

void setup() {
  Serial.begin(9600);

  // Inizializzazione con indirizzo 0x3C (comune per SSD1306)
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Blocca tutto se non trova il display
  }

  display.clearDisplay(); // Pulisce il buffer (importantissimo!)
  display.setTextSize(1); // Dimensione testo
  display.setTextColor(SSD1306_WHITE); // Colore (Bianco su Nero)
  display.setCursor(0,0); // Posizione (x,y)
  
  display.println(F("NANO + ADAFRUIT"));
  display.setCursor(0,16);
  display.println(F("128x64 OK!"));
  display.setCursor(0,32);
  display.println(F("3rd row"));
  
  display.display(); // Invia effettivamente i dati al display
}

void loop() {
  // Facciamo un piccolo effetto blink del testo
  display.invertDisplay(false);
  delay(1000);
}