/*
 * 
 *      LoraWan Based SmartA AgriCulture using Wio Terminal
 *      Node Type:Receiver
 *      Author:Muhammad Afzal
 *      Hackster Profile:https://www.hackster.io/mafzal/
 *      Linkedin Profile:https://www.linkedin.com/in/mafzal9/
 *      
 */
#include <Arduino.h>
#include"TFT_eSPI.h"
#include <SoftwareSerial.h>

SoftwareSerial e5(40,41);
Serial_             * debugUart;        // link to serial used for debugginf
SoftwareSerial      * e5SwUart;         // link to the sw serial for E5 communications

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
    
    e5.printf(p_cmd, args);//
    
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
        while (e5.available() > 0) //
        {
            ch = e5.read(); //
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
 
static int recv_prase(void)
{
    char ch;
    int index = 0;
    memset(recv_buf, 0, sizeof(recv_buf));
    while (e5.available() > 0)//
    {
        ch = e5.read(); //
        recv_buf[index++] = ch;
        Serial.print((char)ch);
        delay(2);
    }
 
    if (index)
    {
        char *p_start = NULL;
        char data[32] = {
            0,
        };
        int rssi = 0;
        int snr = 0;
 
        p_start = strstr(recv_buf, "+TEST: RX \"5345454544");
        if (p_start)
        {
            
            spr.fillSprite(TFT_BLACK);
            p_start = strstr(recv_buf, "5345454544");
            if (p_start && (1 == sscanf(p_start, "5345454544%s,", data)))
            {
                data[16] = 0;
                int SoilMoisture;
                int SoilTemperature;
                int DHTTemp;
                int DHTHum;
                char *endptr;
                char *endptr1;
                char *endptr2;
                char *endptr3;
                char datatvoc[5] = {data[0], data[1],data[2], data[3]};
                char dataco2[5] = {data[4], data[5], data[6], data[7]};
                char datatemp[5] = {data[8], data[9], data[10], data[11]};
                char datahumi[5] = {data[12], data[13],data[14], data[15]};
                
                SoilMoisture = strtol(datatvoc, &endptr, 16);
                SoilTemperature = strtol(dataco2, &endptr1, 16);
                DHTTemp = strtol(datatemp, &endptr, 16);
                DHTHum = strtol(datahumi, &endptr1, 16);
                
                spr.createSprite(100, 30);
                spr.setFreeFont(&FreeSansBold12pt7b);
                spr.setTextColor(TFT_WHITE);
                spr.drawNumber(SoilMoisture, 0, 0, 1);
                spr.pushSprite(15, 100);
                spr.deleteSprite();
                spr.createSprite(150, 30);
                spr.drawNumber(SoilTemperature, 0, 0, 1);
                spr.pushSprite(180, 100);
                spr.deleteSprite();
                spr.createSprite(100, 30);
                spr.drawNumber(DHTTemp, 0, 0, 1);
                spr.pushSprite(15, 185);
                spr.deleteSprite();
                spr.createSprite(150, 30);
                spr.drawNumber(DHTHum, 0 , 0 , 1);
                spr.pushSprite(180, 185);
                spr.deleteSprite();
                
                Serial.print("data received displaying on the wio terminal");
                Serial.print("\r\n");

                
            }
 
            p_start = strstr(recv_buf, "RSSI:");
            if (p_start && (1 == sscanf(p_start, "RSSI:%d,", &rssi)))
            {
                String newrssi = String(rssi);
          
                Serial.print(rssi);
                Serial.print("\r\n");

            }
            p_start = strstr(recv_buf, "SNR:");
            if (p_start && (1 == sscanf(p_start, "SNR:%d", &snr)))
            {
                Serial.print(snr);
                Serial.print("\r\n");

                
            }
            return 1;
        }
    }
    return 0;
}
 
static int node_recv(uint32_t timeout_ms)
{
    at_send_check_response("+TEST: RXLRPKT", 1000, "AT+TEST=RXLRPKT\r\n");
    int startMillis = millis();
    do
    {
        if (recv_prase())
        {
            return 1;
        }
    } while (millis() - startMillis < timeout_ms);
    return 0;
}
 

 
void setup(void)
{

    
    tft.begin();
    tft.setRotation(3);
    
    uint32_t start = millis();
    Serial.begin(9600);
    while ( !Serial && (millis() - start) < 2500 );  // Open the Serial Monitor to get started or wait for 1.5"
 
    e5.begin(9600);
    Serial.print("Receiver\r\n"); 
    
        //Display the Headings
    tft.fillScreen(TFT_BLACK);
    tft.setFreeFont(&FreeSansBold18pt7b);
    tft.setTextColor(TFT_WHITE);
    tft.drawString(" Receiver Node", 5, 10 , 1);
    
    tft.setFreeFont(&FreeSans12pt7b);
    tft.setTextColor(TFT_RED);
    tft.drawString("TVOC", 7 , 65 , 1);
    tft.drawString("Co2:", 180 , 65 , 1);
    tft.drawString("Temperature:", 7 , 150 , 1);
    tft.drawString("Humidity:", 180 , 150 , 1);
    
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

        node_recv(60000);

    }
}
