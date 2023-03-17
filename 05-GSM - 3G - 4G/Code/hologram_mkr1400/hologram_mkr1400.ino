/*
Holgoram Adaptation of Arduino's GSM Web Client Example Sketch used to send messages to Hologram's Cloud


Arduino information:

  Web client

 Circuit:
 * MKR GSM 1400 board
 * Antenna
 * SIM card with a data plan

 created 8 Mar 2012
 by Tom Igoe

modified 15 May 2017
by Maiky Iberkleid
*/

// libraries
#include <MKRGSM.h>

//We replaced "arduino_secrets.h" as all the information for that is publicly available from https://hologram.io/docs/guide/connect/connect-device/#apn-settings

const char PINNUMBER[] = " ";
// APN data
const char GPRS_APN[] = "hologram";
const char GPRS_LOGIN[] = " ";
const char GPRS_PASSWORD[] = " ";
String HOLOGRAM_DEVICE_KEY = "[3t{y+G_";
String HOLOGRAM_MESSAGE = "{Temperature:20,Humdity:50,AQI:400}";
String HOLOGRAM_TOPIC = "SenseEnv";

// initialize the library instance
GSMClient client;
GPRS gprs;
GSM gsmAccess;

// Hologram's Embedded API (https://hologram.io/docs/reference/cloud/embedded/) URL and port
char server[] = "cloudsocket.hologram.io";
int port = 9999;

void setup() {
  // initialize serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
 }

  Serial.println("Starting Arduino web client.");
  // connection state
  boolean connected = false;

  // After starting the modem with GSM.begin()
  // attach to the GPRS network with the APN, login and password
  while (!connected) {
     Serial.println("Begin gsm Access");
    //Serial.println(gsmAccess.begin()); //Uncomment for testing
    
    if ((gsmAccess.begin() == GSM_READY) &&
        (gprs.attachGPRS(GPRS_APN, GPRS_LOGIN, GPRS_PASSWORD) == GPRS_READY)) {
      connected = true;
      Serial.println("GSM Access Success");
    } 
    else {
      Serial.println("Not connected");
      delay(1000);
    }
  }

  Serial.println("connecting...");

  // if you get a connection, report back via serial:
  if (client.connect(server, port)) {
    Serial.println("connected");
    // Send a Message request:
    client.println("{\"k\":\"" + HOLOGRAM_DEVICE_KEY +"\",\"d\":\""+ HOLOGRAM_MESSAGE+ "\",\"t\":\""+HOLOGRAM_TOPIC+"\"}");
  } else {
    // if you didn't get a connection to the server:
    Serial.println("connection failed");
  }
}

void loop() {
  // if there are incoming bytes available
  // from the server, read them and print them:
  if (client.available()) {
    char c = client.read();
    Serial.print(c);
  }

  // if the server's disconnected, stop the client:
  if (!client.available() && !client.connected()) {
    Serial.println();
    Serial.println("disconnecting.");
    client.stop();

    // do nothing forevermore:
    for (;;)
      ;
  }
}
