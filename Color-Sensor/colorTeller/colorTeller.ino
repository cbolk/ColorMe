#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "Adafruit_TCS34725.h"

// Impostazioni Display
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define ROWGAP 16
#define OLED_RESET    -1
#define SCREEN_ADDRESS  0x3C 
#define SENSOR_ADDRESS  0x29

#define GET_COLOR_CYCLES 10

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Impostazioni Sensore Colore
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);

// Pin Tasti
const int pinGiallo = 2;
const int pinVerde  = 3;
const int pinLED = 4;

// some magic numbers for this device from the DN40 application note
#define TCS34725_R_Coef 0.136
#define TCS34725_G_Coef 1.000
#define TCS34725_B_Coef -0.444
#define TCS34725_GA 1.0
#define TCS34725_DF 310.0
#define TCS34725_CT_Coef 3810.0
#define TCS34725_CT_Offset 1391.0

// Autorange class for TCS34725
class tcs34725 {
private:
  struct tcs_agc {
    tcs34725Gain_t ag;
    uint8_t at;
    uint16_t mincnt;
    uint16_t maxcnt;
  };
  static const tcs_agc agc_lst[];
  uint16_t agc_cur;

  void setGainTime(void);
  Adafruit_TCS34725 tcs;

public:
  tcs34725(void);

  boolean begin(void);
  void getData(void);

  boolean isAvailable, isSaturated;
  uint16_t againx, atime, atime_ms;
  uint16_t r, g, b, c;
  uint16_t ir;
  uint16_t r_comp, g_comp, b_comp, c_comp;
  uint16_t saturation, saturation75;
  float cratio, cpl, ct, lux, maxlux;
};
//
// Gain/time combinations to use and the min/max limits for hysteresis
// that avoid saturation. They should be in order from dim to bright.
//
// Also set the first min count and the last max count to 0 to indicate
// the start and end of the list.
//
const tcs34725::tcs_agc tcs34725::agc_lst[] = {
  { TCS34725_GAIN_60X, TCS34725_INTEGRATIONTIME_614MS,     0, 20000 },
  { TCS34725_GAIN_60X, TCS34725_INTEGRATIONTIME_154MS,  4990, 63000 },
  { TCS34725_GAIN_16X, TCS34725_INTEGRATIONTIME_154MS, 16790, 63000 },
  { TCS34725_GAIN_4X,  TCS34725_INTEGRATIONTIME_154MS, 15740, 63000 },
  { TCS34725_GAIN_1X,  TCS34725_INTEGRATIONTIME_154MS, 15740, 0 }
};
tcs34725::tcs34725() : agc_cur(0), isAvailable(0), isSaturated(0) {
}

// initialize the sensor
boolean tcs34725::begin(void) {
  tcs = Adafruit_TCS34725(agc_lst[agc_cur].at, agc_lst[agc_cur].ag);
  if ((isAvailable = tcs.begin()))
    setGainTime();
  return(isAvailable);
}

// Set the gain and integration time
void tcs34725::setGainTime(void) {
  tcs.setGain(agc_lst[agc_cur].ag);
  tcs.setIntegrationTime(agc_lst[agc_cur].at);
  atime = int(agc_lst[agc_cur].at);
  atime_ms = ((256 - atime) * 2.4);
  switch(agc_lst[agc_cur].ag) {
  case TCS34725_GAIN_1X:
    againx = 1;
    break;
  case TCS34725_GAIN_4X:
    againx = 4;
    break;
  case TCS34725_GAIN_16X:
    againx = 16;
    break;
  case TCS34725_GAIN_60X:
    againx = 60;
    break;
  }
}

// Retrieve data from the sensor and do the calculations
void tcs34725::getData(void) {
  // read the sensor and autorange if necessary
  tcs.getRawData(&r, &g, &b, &c);
  while(1) {
    if (agc_lst[agc_cur].maxcnt && c > agc_lst[agc_cur].maxcnt)
      agc_cur++;
    else if (agc_lst[agc_cur].mincnt && c < agc_lst[agc_cur].mincnt)
      agc_cur--;
    else break;

    setGainTime();
    delay((256 - atime) * 2.4 * 2); // shock absorber
    tcs.getRawData(&r, &g, &b, &c);
    break;
  }

  // DN40 calculations
  ir = (r + g + b > c) ? (r + g + b - c) / 2 : 0;
  r_comp = r - ir;
  g_comp = g - ir;
  b_comp = b - ir;
  c_comp = c - ir;
  cratio = float(ir) / float(c);

  saturation = ((256 - atime) > 63) ? 65535 : 1024 * (256 - atime);
  saturation75 = (atime_ms < 150) ? (saturation - saturation / 4) : saturation;
  isSaturated = (atime_ms < 150 && c > saturation75) ? 1 : 0;
  cpl = (atime_ms * againx) / (TCS34725_GA * TCS34725_DF);
  maxlux = 65535 / (cpl * 3);

  lux = (TCS34725_R_Coef * float(r_comp) + TCS34725_G_Coef * float(g_comp) + TCS34725_B_Coef * float(b_comp)) / cpl;
  ct = TCS34725_CT_Coef * float(b_comp) / float(r_comp) + TCS34725_CT_Offset;
}

tcs34725 rgb_sensor;

void setup() {
  // 1. Apri la comunicazione seriale a 9600 baud
  Serial.begin(9600);
  Serial.println(F("--- TEST AVVIATO ---"));

  // 2. Configura i tasti
  pinMode(pinGiallo, INPUT_PULLUP);
  pinMode(pinVerde,  INPUT_PULLUP);

  // 3. Avvia il Display
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("Errore OLED: Controlla A4/A5"));
    for(;;);
  }
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0,0);
  display.println(F("SISTEMA PRONTO"));
  display.display();

  // 4. Avvia il Sensore
  rgb_sensor.begin();
  pinMode(4, OUTPUT);
  digitalWrite(4, LOW); // @gremlins Bright light, bright light!
  Serial.println(F("Sensore TCS34725 trovato!"));
}

void loop() {  
  uint16_t valr, valg, valb, valc;
  char colorComp[64];
  Serial.println(F("Nulla da fare"));
  if (digitalRead(pinGiallo) == LOW) {
    Serial.println(F("Richiesta calibrazione"));
    rgb_sensor.getData();
    valr = rgb_sensor.r_comp;
    valg = rgb_sensor.g_comp;
    valb = rgb_sensor.b_comp;
    valc = rgb_sensor.c_comp;

    // Stampa su Monitor Seriale (PC)
    Serial.print(F("R:")); Serial.print(valr);
    Serial.print(F(" G:")); Serial.print(valg);
    Serial.print(F(" B:")); Serial.println(valb);

    // Stampa su Display OLED
    display.clearDisplay();
    display.setCursor(0,0);
    display.setTextSize(2);
    display.println(F("COLORE:"));
    display.setTextSize(1);
    display.print(F("R: ")); display.println(valr);
    display.print(F("G: ")); display.println(valg);
    display.print(F("B: ")); display.println(valb);
    display.display();

    delay(1000); // Evita letture multiple rapide
  }
  if (digitalRead(pinVerde) == LOW) {
    Serial.println(F("Rilevazione colore"));
    digitalWrite(4, HIGH); // @gremlins Bright light, bright light!
    valr = valg = valb = valc = 0;
    for(int i = 0; i < GET_COLOR_CYCLES ; i++){
      valr += rgb_sensor.r_comp;
      valg += rgb_sensor.g_comp;
      valb += rgb_sensor.b_comp;
      valc += rgb_sensor.c_comp;
      delay(10);
      sprintf(colorComp, "%d %d %d %c", valr, valg, valb, valc);
      Serial.println(colorComp);
    }
    valr = ((float)valr/GET_COLOR_CYCLES) + 0.5;
    valg = ((float)valg/GET_COLOR_CYCLES) + 0.5;
    valb = ((float)valb/GET_COLOR_CYCLES) + 0.5;
    display.clearDisplay();
    display.print(F("R: ")); display.println(valr);
    display.print(F("G: ")); display.println(valg);
    display.print(F("B: ")); display.println(valb);
    displayColore(valr, valg, valb);
    digitalWrite(4, LOW); // @gremlins Bright light, bright light!
    delay(1000);
  }
  delay(1000);
}


void displayStrings(char st1[], char st2[], char st3[], char st4[]) {
  display.clearDisplay();
  
  // Riga 1 (Sempre presente se chiamata)
  display.setCursor(0, 0);
  display.println(st1);

  // Riga 2
  if(st2 && st2[0] != '\0'){
    display.setCursor(0, ROWGAP);
    display.println(st2);
    // Riga 3
    if(st3 && st3[0] != '\0'){
      display.setCursor(0, ROWGAP * 2);
      display.println(st3);
      if(st4 && st4[0] != '\0'){
        display.setCursor(0, ROWGAP * 3);
        display.println(st4);
      }
    }
  }
  display.display(); 
}

void displayColore(uint16_t valr, uint16_t valg, uint16_t valb)
{
  char hex[8];
  char colorP[22];
  sprintf(hex, "#%02x%02x%02x", valr, valg, valb);
  display.clearDisplay();
  display.setCursor(0,0);
  sprintf(colorP,"R: %03d G: %03d B: %03d", valr, valg, valb);
  display.println(colorP);
  display.setCursor(0,ROWGAP);
  display.println(hex);
  display.display(); 
}

