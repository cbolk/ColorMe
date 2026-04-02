//SensorHandler implementation 
#include "SensorHandler.h"
#include <vector>

#define COLOR_CALIBRATION_CYCLES 50
#define GET_COLOR_CYCLES 10
#define CONFIG_JSON_PATH "/sensor_config.json"
const std::vector<String> KEY_NAME_CONFIG_JSON = {"r_coef", "g_coef", "b_coef", "real_white_coef"};

void starting_calibration_print(DisplayHandler &theDisplay){
    delay(2000);
    Serial.println("Starting calibration process!");
    Serial.println("point your sensor to a white surface");
    theDisplay.print("Calibrazione","Puntare ad una superficie bianca");

    delay(5000);        
    Serial.println("Calibrazione inizia tra:");
    theDisplay.print("Calibrazione inizia tra:");
      
    Serial.println("5");
    theDisplay.print("5");
    delay(1000);

    Serial.println("4");
    theDisplay.print("4");
    delay(1000);

    Serial.println("3");
    theDisplay.print("3");
    delay(1000);

    Serial.println("2");
    theDisplay.print("2");
    delay(1000);

    Serial.println("1");
    theDisplay.print("1");
    delay(1000);

    Serial.println("Calibrazione");
    theDisplay.print("Calibrazione");
}

sensor_status SensorHandler :: begin(){
    LittleFSHandler fs;
    std :: vector<String> data;

    theDisplay.begin();
    while (!tcs.begin()) {
        theDisplay.print("Sensore non trovato");
        delay(100);
    }

    data.reserve(KEY_NAME_CONFIG_JSON.size());
    if(!fs.loadFS_json(data,KEY_NAME_CONFIG_JSON,CONFIG_JSON_PATH))
        theDisplay.print("Errore nel caricare la configurazione");

    if(data.empty()){
        //SensorHandler:: calib_saving();
        theDisplay.print("Nessuna configurazione disponibile");
        return SENSOR_NEED_CALIBRATION;
    }
    
    calibration_coef.r = data[0].toFloat();
    calibration_coef.g = data[1].toFloat();
    calibration_coef.b = data[2].toFloat();
    calibration_coef.realwhite = data[3].toInt();
    
    return SENSOR_OK_AND_CALIBRATED;
}

void SensorHandler :: calib_saving(){
    LittleFSHandler fs;
    JsonDocument json;
    json[KEY_NAME_CONFIG_JSON[0]] = calibration_coef.r;
    json[KEY_NAME_CONFIG_JSON[1]] = calibration_coef.g;
    json[KEY_NAME_CONFIG_JSON[2]] = calibration_coef.b;
    json[KEY_NAME_CONFIG_JSON[3]] =calibration_coef.realwhite;
    fs.new_file(json,CONFIG_JSON_PATH);
}


const bool SensorHandler :: Calibration()
{
    // temporary variable for all the 3 color
    uint32_t tot_r = 0;
    uint32_t tot_g = 0;
    uint32_t tot_b = 0;
    uint16_t tmp_r, tmp_g, tmp_b, tmp_c;
    int i;

    starting_calibration_print(theDisplay);

    for(i = 0; i < COLOR_CALIBRATION_CYCLES; i++){
        tcs.enable();
        delay(10);

        tcs.getRawData(&tmp_r, &tmp_g, &tmp_b, &tmp_c);
        delay(10);

        tcs.disable();
        tot_r += tmp_r;
        tot_g += tmp_g;
        tot_b += tmp_b;   
        Serial.print(tmp_r);
        Serial.print(tmp_g);
        Serial.println(tmp_b);
    }   
    tot_r /= COLOR_CALIBRATION_CYCLES;
    tot_g /= COLOR_CALIBRATION_CYCLES;
    tot_b /= COLOR_CALIBRATION_CYCLES;    

    //evaluating the real white
    Serial.print("Valore massimo:");Serial.print(max(tot_r,max(tot_g,tot_b)));
    calibration_coef.realwhite = max(tot_r,max(tot_g,tot_b)); //real_white saving
    
    calibration_coef.r = (float)calibration_coef.realwhite / tot_r;
    calibration_coef.g = (float)calibration_coef.realwhite / tot_g;
    calibration_coef.b = (float)calibration_coef.realwhite / tot_b;

    calib_saving();

    Serial.print("\n\n");
    Serial.print("Aggiustamento colore: ");Serial.print(calibration_coef.r);Serial.print(calibration_coef.g);Serial.println(calibration_coef.b);
    Serial.print("\n\n"); 
    
    Serial.println("Calibrazione terminata");
    theDisplay.print("Calibrazione terminata");
    return true;
}

const RGB SensorHandler :: GetColor(){
    uint32_t r=0, g=0, b=0, c=0;
    char str[32];
    char str1[64];
    TableHandler colorTB;
    
    theDisplay.print("Acquisizione colore");
    Serial.print("Acquisizione colore");
    delay(100);
    for(int i=0; i<GET_COLOR_CYCLES ; i++){
    tcs.enable();
        delay(10);
        uint16_t r_tmp,g_tmp,b_tmp,c_tmp;
        tcs.getRawData(&r_tmp,&g_tmp,&b_tmp,&c_tmp);
        r += r_tmp;
        g += g_tmp;
        b += b_tmp;
        c += c_tmp;
        delay(10);
        tcs.disable();
        Serial.print("R: ");Serial.print(r);Serial.print(" G: ");Serial.print(g);Serial.print(" B: ");Serial.print(b);Serial.print(" C: ");Serial.println(c);
    }
    Serial.print("\n\n\n"); 
    //adding 0.5 in order to permform a round up
    // rgb.r = (((float)r / GET_COLOR_CYCLES) * calibration_coef.r / calibration_coef.realwhite * 255) + 0.5; 
    rgb.r = ((float)r * calibration_coef.r * 255) / (GET_COLOR_CYCLES * calibration_coef.realwhite) + 0.5;
    rgb.g = ((float)g * calibration_coef.g * 255) / (GET_COLOR_CYCLES * calibration_coef.realwhite) + 0.5;
    rgb.b = ((float)b * calibration_coef.b * 255) / (GET_COLOR_CYCLES * calibration_coef.realwhite) + 0.5;
    if (rgb.r > 255) rgb.r = 256;
    if (rgb.g > 255) rgb.g = 256;
    if (rgb.b > 255) rgb.b = 256;
    rgb = colorTB.getColor(rgb);
    return rgb;
}
