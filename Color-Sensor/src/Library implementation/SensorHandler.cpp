//SensorHandler implementation 
#include "SensorHandler.h"

#include <vector>



#define COLOR_CALIBRATION_CYCLES 50
#define GET_COLOR_CYCLES 10
#define CONFIG_JSON_PATH "/sensor_config.json"
const std::vector<String> KEY_NAME_CONFIG_JSON = {"r_coef", "g_coef", "b_coef", "real_white_coef"};


void starting_calibration_print(DisplayHandler &display){
    
    delay(2000);
    Serial.println("Starting calibration process!");
    Serial.println("point your sensor to a white surface");
    display.print("Starting calibration process!","point your sensor to a white surface");

    delay(5000);        
    Serial.println("Calibration starting in:");
    display.print("Calibration starting in:");
      
    Serial.println("5");
    display.print("5");
    delay(1000);

    Serial.println("4");
    display.print("4");
    delay(1000);

    Serial.println("3");
    display.print("3");
    delay(1000);

    Serial.println("2");
    display.print("2");
    delay(1000);

    Serial.println("1");
    display.print("1");
    delay(1000);

    Serial.println("Calibrating!!");
    display.print("Calibrating!!");
}

sensor_status SensorHandler :: begin(){
    display.begin();
    while (!tcs.begin()) {
            display.print("Sensor not found");
            delay(100);
    }


    LittleFSHandler fs;
    std :: vector<String> data;
    data.reserve(KEY_NAME_CONFIG_JSON.size());
    if(!fs.loadFS_json(data,KEY_NAME_CONFIG_JSON,CONFIG_JSON_PATH))
        display.print("Error while loading");

    if(data.empty()){
        //SensorHandler:: calib_saving();
        display.print("Nessuna Configurazione disponibile");
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


const bool SensorHandler::Calibration()
{
    starting_calibration_print(display);

    // temporary variable for all the 3 color
    uint32_t tot_r=0;
    uint32_t tot_g=0;
    uint32_t tot_b=0;
    uint16_t tmp_r,tmp_g,tmp_b,tmp_c;

    for(int i=0; i<COLOR_CALIBRATION_CYCLES ; i++){
        tcs.enable();
        delay(10);
        tcs.getRawData(&tmp_r,&tmp_g,&tmp_b,&tmp_c);
        delay(10);
        tcs.disable();
        tot_r += tmp_r;
        tot_g += tmp_g;
        tot_b += tmp_b;   
        Serial.print(tmp_r);Serial.print(tmp_g);Serial.println(tmp_b);
    }   
    tot_r /= COLOR_CALIBRATION_CYCLES;
    tot_g /= COLOR_CALIBRATION_CYCLES;
    tot_b /= COLOR_CALIBRATION_CYCLES;    

    //evaluating the real white
    Serial.print("Max value:");Serial.print(max(tot_r,max(tot_g,tot_b)));
    calibration_coef.realwhite = max(tot_r,max(tot_g,tot_b)); //real_white saving


    
    calibration_coef.r = (float)calibration_coef.realwhite / (float)tot_r;
    calibration_coef.g = (float)calibration_coef.realwhite / (float)tot_g;
    calibration_coef.b = (float)calibration_coef.realwhite / (float)tot_b;

    calib_saving();

    Serial.print("\n\n");
    Serial.print("color adjustation: ");Serial.print(calibration_coef.r);Serial.print(calibration_coef.g);Serial.println(calibration_coef.b);
    Serial.print("\n\n"); 
    
    Serial.println("Calibration Done");
    display.print("Calibration Done");
    return true;
}




const RGB SensorHandler::GetColor(){
    uint32_t r=0, g=0, b=0, c=0;
    
    display.print("getting color");
    Serial.print("getting color");
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
    rgb.r = (int)((((float)r / (float)GET_COLOR_CYCLES) * calibration_coef.r / (float)calibration_coef.realwhite * 255.0) + 0.5); 
    rgb.g = (int)((((float)g / (float)GET_COLOR_CYCLES) * calibration_coef.g / (float)calibration_coef.realwhite * 255.0) + 0.5);
    rgb.b = (int)((((float)b / (float)GET_COLOR_CYCLES) * calibration_coef.b / (float)calibration_coef.realwhite * 255.0) + 0.5);
    if(rgb.r>255)
        rgb.r=256;
    if(rgb.g>255)
        rgb.g=256;
    if(rgb.b>255)
        rgb.b=256;

    Serial.println("COLORI CALIBRATI:");
    Serial.print("R: ");Serial.print(rgb.r);Serial.print(" G: ");Serial.print(rgb.g);Serial.print(" B: ");Serial.print(rgb.b);

    TableHandler colorTB;
    rgb = colorTB.getColor(rgb);

    Serial.println("COLORI CALIBRATI:");
    Serial.print("R: ");Serial.print(rgb.r);Serial.print(" G: ");Serial.print(rgb.g);Serial.print(" B: ");Serial.print(rgb.b);
    char str[32];
    char str1[64];
    snprintf(str,sizeof(str),"COLORE: %s",rgb.color_name); 
    snprintf(str1,sizeof(str),"R: %d G: %d B: %d", rgb.r, rgb.g, rgb.b);
    display.print(str,str1);
    Serial.print("\n\n\n");
    return rgb;
}
