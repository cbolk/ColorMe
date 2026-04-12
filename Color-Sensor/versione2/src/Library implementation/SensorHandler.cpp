//SensorHandler implementation 
#include "SensorHandler.h"

#define COLOR_CALIBRATION_CYCLES    50
#define GET_COLOR_CYCLES            10
#define CALIBRATION_FILE            "/calibration.txt"

void StartCalibrationPrint(DisplayHandler &display){
    delay(2000);
    Serial.println("Preparazione calibrazione: preparare colore bianco");
    display.print("Calibrazione","Rilevare il colore bianco");
    delay(1000);        
    Serial.println("Calibrazione inizia tra:");
    display.print("Calibrazione inizia tra:");
    Serial.println("3");
    display.print("3");
    delay(1000);
    Serial.println("2");
    display.print("2");
    delay(1000);
    Serial.println("1");
    display.print("1");
    delay(1000);
    Serial.println("Calibrazione");
    display.print("Calibrazione");
}

sensor_status SensorHandler :: begin(){
    CalibrationHandler::CalibrationData cal;
    while (!tcs.begin()) {
            Serial.println("Sensore non trovato");
            display.print("Sensore non trovato");
            delay(100);
    }
    Serial.println("Sensore trovato");
    if (CalibrationHandler::loadCalibration(cal, CALIBRATION_FILE)) {
        Serial.println("Configurazione calibrazione disponibile");
        Serial.println(cal.r);
        Serial.println(cal.realwhite);
    } else {
        Serial.println("Configurazione calibrazione non disponibile. Procedere con la calibrazione.");
        display.print("Nessuna Configurazione disponibile");
        return SENSOR_NEED_CALIBRATION;
    }
    Serial.println("Impostati i valori correttivi: ");
    Serial.print("coeff_r: ");
    Serial.print(cal.r);
    calibration_coef.r = cal.r;
    Serial.print(" coeff_g: ");
    Serial.print(cal.g);
    calibration_coef.g = cal.g;
    Serial.print(" coeff_b: ");
    Serial.print(cal.b);
    calibration_coef.b = cal.b;
    Serial.print(" coeff_bianco: ");
    Serial.println(cal.realwhite);
    calibration_coef.realwhite = cal.realwhite;
    return SENSOR_OK_AND_CALIBRATED;
}

const bool SensorHandler::Calibration()
{
    CalibrationHandler::CalibrationData cal;
    // temporary variable for all the 3 color
    uint32_t tot_r=0;
    uint32_t tot_g=0;
    uint32_t tot_b=0;
    uint16_t tmp_r, tmp_g, tmp_b, tmp_c;
    char tmp[3];

    StartCalibrationPrint(display);
    display.print("Calibrazione (");
    Serial.print("Calibrazione (");
    sprintf(tmp, "%d", COLOR_CALIBRATION_CYCLES);
    display.print(tmp);
    Serial.print(COLOR_CALIBRATION_CYCLES);
    display.print(" campionamenti)");
    Serial.println(" campionamenti)");
    display.printcont(".", 0);
    for(int i = 0; i < COLOR_CALIBRATION_CYCLES ; i++){
        display.printcont(".", i);
        Serial.print(".");
        tcs.enable();
        delay(10);
        tcs.getRawData(&tmp_r, &tmp_g, &tmp_b, &tmp_c);
        delay(10);
        tcs.disable();
        tot_r += tmp_r;
        tot_g += tmp_g;
        tot_b += tmp_b;   
        //Serial.print(tmp_r); Serial.print(" "); Serial.print(tmp_g); Serial.print(" "); Serial.println(tmp_b);
    }   
    tot_r /= COLOR_CALIBRATION_CYCLES;
    tot_g /= COLOR_CALIBRATION_CYCLES;
    tot_b /= COLOR_CALIBRATION_CYCLES;    

    //evaluating the real white
    Serial.print("Max value: ");Serial.print(max(tot_r,max(tot_g,tot_b)));
    calibration_coef.realwhite = max(tot_r,max(tot_g,tot_b)); //real_white saving
    calibration_coef.r = (float)calibration_coef.realwhite / tot_r;
    calibration_coef.g = (float)calibration_coef.realwhite / tot_g;
    calibration_coef.b = (float)calibration_coef.realwhite / tot_b;

    cal.r          = calibration_coef.r;
    cal.g          = calibration_coef.g;
    cal.b          = calibration_coef.b;
    cal.realwhite  = calibration_coef.realwhite;
    CalibrationHandler::saveCalibration(cal, CALIBRATION_FILE);

    Serial.println("\n");
    Serial.print("Coefficienti: ");
    Serial.print(calibration_coef.r);
    Serial.print(" ");
    Serial.print(calibration_coef.g);
    Serial.print(" ");
    Serial.println(calibration_coef.b);
    Serial.println("\n"); 
    
    Serial.println("Calibrazione conclusa");
    display.print("Calibrazione conclusa");
    return true;
}


const RGB SensorHandler::GetColor(){
    uint32_t r=0, g=0, b=0, c=0;
    uint16_t r_tmp,g_tmp,b_tmp,c_tmp;
    char tmp[4];

    display.print("Acquisizione colore");
    Serial.print("Acquisizione colore");
    display.print(tmp,"campionamenti" );
    Serial.print("(");
    Serial.print(GET_COLOR_CYCLES);
    Serial.println(" campionamenti)");
    delay(100);
    for(int i=0; i < GET_COLOR_CYCLES ; i++){
        Serial.print(".");
        tcs.enable();
        Serial.print(">");
        delay(10);
        tcs.getRawData(&r_tmp, &g_tmp, &b_tmp, &c_tmp);
        r += r_tmp;
        g += g_tmp;
        b += b_tmp;
        c += c_tmp;
        delay(10);
        Serial.print("<");
        tcs.disable();
//        Serial.print("R: ");Serial.print(r);Serial.print(" G: ");Serial.print(g);Serial.print(" B: ");Serial.print(b);Serial.print(" C: ");Serial.println(c);
    }
    Serial.println("\n"); 
    //adding 0.5 in order to permform a round up
    // rgb.r = (((float)r / GET_COLOR_CYCLES) * calibration_coef.r / calibration_coef.realwhite * 255) + 0.5; 
    rgb.r = ((float)r * calibration_coef.r * 255) / (GET_COLOR_CYCLES * calibration_coef.realwhite) + 0.5;
    rgb.g = ((float)g * calibration_coef.g * 255) / (GET_COLOR_CYCLES * calibration_coef.realwhite) + 0.5;
    rgb.b = ((float)b * calibration_coef.b * 255) / (GET_COLOR_CYCLES * calibration_coef.realwhite) + 0.5;
    Serial.print("R: ");Serial.print(rgb.r);
    Serial.print(" G: ");Serial.print(rgb.g);
    Serial.print(" B: ");Serial.println(rgb.b);
    if (rgb.r > 255) rgb.r = 255;
    if (rgb.g > 255) rgb.g = 255;
    if (rgb.b > 255) rgb.b = 255;
    Serial.print("R: ");Serial.print(rgb.r);
    Serial.print(" G: ");Serial.print(rgb.g);
    Serial.print(" B: ");Serial.println(rgb.b);
    return rgb;
}
