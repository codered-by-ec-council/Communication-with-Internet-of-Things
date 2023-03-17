#include <Arduino.h>
#include"TFT_eSPI.h"
#include<SoftwareSerial.h>

SoftwareSerial e5(0, 1);

//SoftwareSerial e5(BCM3, BCM2,false);
//SoftwareSerial e5(BCM27, BCM22,false);
static char recv_buf[512];
static bool is_exist = false;

TFT_eSPI tft;
TFT_eSprite spr = TFT_eSprite(&tft);  //sprite



static int at_send_check_response(char *p_ack, int timeout_ms, char *p_cmd, ...)
{
    int ch = 0;
    int index = 0;
    int startMillis = 0;
    va_list args;
    memset(recv_buf, 0, sizeof(recv_buf));
    va_start(args, p_cmd);
    e5.printf(p_cmd, args);
    Serial.printf(p_cmd, args);
    va_end(args);
    delay(200);
    startMillis = millis();
 
    if (p_ack == NULL)
    {
        return 0;
    }
 
    do
    {
        while (e5.available() > 0)
        {
            ch = e5.read();
            recv_buf[index++] = ch;
            Serial.print((char)ch);
            delay(2);
        }
 
        if (strstr(recv_buf, p_ack) != NULL)
        {
            return 1;
        }
 
    } while (millis() - startMillis < timeout_ms);
    return 0;
}



 
static int node_send(uint32_t timeout)
{
    static uint16_t count = 0;
    int ret = 0;
    char data[32];
    char cmd[128];

    int tvoc=random(100,1500), co2_eq_ppm=random(100,1500);
    float temperature=random(-10,50), humidity=random(0,100);
    uint16_t error;
    char errorMessage[256];

    spr.createSprite(100, 30);
    spr.setFreeFont(&FreeSansBoldOblique12pt7b);
    spr.setTextColor(TFT_WHITE);
    spr.drawNumber(tvoc, 0, 0, 1);
    spr.pushSprite(15, 100);
    spr.deleteSprite();
    spr.createSprite(150, 30);
    spr.setFreeFont(&FreeSansBoldOblique12pt7b);
    spr.setTextColor(TFT_WHITE);
    spr.drawNumber(co2_eq_ppm, 0, 0, 1);
    spr.pushSprite(150, 100);
    spr.deleteSprite();
    spr.createSprite(150, 30);
    spr.setFreeFont(&FreeSansBoldOblique12pt7b);
    spr.setTextColor(TFT_WHITE);
    spr.drawNumber(humidity , 0 , 0 , 1);
    spr.pushSprite(180, 185);
    spr.deleteSprite();
    spr.createSprite(100, 30);
    spr.setFreeFont(&FreeSansBoldOblique12pt7b);
    spr.setTextColor(TFT_WHITE);
    spr.drawNumber(temperature, 0, 0, 1);
    spr.pushSprite(15, 185);
    spr.deleteSprite();

                
    memset(data, 0, sizeof(data));
    sprintf(data, "%04X,%04X,%04X,%04X", tvoc, co2_eq_ppm, int(temperature),int(humidity));
    sprintf(cmd, "AT+TEST=TXLRPKT,\"5345454544%s\"\r\n", data);
    ret = at_send_check_response("TX DONE", 2000, cmd);
    if (ret == 1)
    {
 
        //send Airquality
        Serial.print("Sent successfully!\r\n");
    }
    else
    {
        Serial.print("Send failed!\r\n");
    }
    return ret;
}
 

void setup(void)
{

    tft.begin();
    tft.setRotation(3);

    tft.fillScreen(TFT_BLACK);
    tft.setFreeFont(&FreeSansBoldOblique12pt7b);
    tft.setTextColor(TFT_RED);
    tft.drawString("TVoC", 7 , 65 , 1);
    tft.drawString("CO2", 165 , 65 , 1);
    
    tft.setFreeFont(&FreeSansBoldOblique12pt7b);
    tft.setTextColor(TFT_RED);
    tft.drawString("Humi", 7 , 150 , 1);
  
    tft.setFreeFont(&FreeSansBoldOblique12pt7b);
    tft.setTextColor(TFT_RED);
    tft.drawString("Temp:", 165 , 150 , 1);
    
    tft.setFreeFont(&FreeSansBoldOblique18pt7b);
    tft.setTextColor(TFT_WHITE);
    tft.drawString("E5 Sender", 50, 10 , 1);
    
    uint32_t start = millis();
    Serial.begin(9600);
    while ( !Serial && (millis() - start) < 1500 );  // Open the Serial Monitor to get started or wait for 1.5"
 
    e5.begin(9600);
    
    uint16_t error;
    char errorMessage[256];
    
    Serial.print("ping pong communication!\r\n");
 
 
    if (at_send_check_response("+AT: OK", 100, "AT\r\n"))
    {
        is_exist = true;
        at_send_check_response("+MODE: TEST", 1000, "AT+MODE=TEST\r\n");
        at_send_check_response("+TEST: RFCFG", 1000, "AT+TEST=RFCFG,868,SF12,125,12,15,14,ON,OFF,OFF\r\n");
        delay(200);

    }
    else
    {
        is_exist = false;
        Serial.print("No E5 module found.\r\n");
        tft.setFreeFont(&FreeSansBoldOblique12pt7b);
        tft.setTextColor(TFT_RED);
        tft.drawString("No E5 Module", 7 , 150 , 1);

    }
}
 
void loop(void)
{
    if (is_exist)
    {

        //node_send_then_recv(2000);
        node_send(2000);
        delay(3000);

    }
}
