#include <HardwareSerial.h>
HardwareSerial sender( 2 );

#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#define RXD2 16                    //esp32 has 3 UART in which we gonn use uart 2, 
#define TXD2 17
 
const char* ssid ="Tony-Stark Indeed";              //wifi's SSID
const char* password ="SG3524INV";                 //wifi's Password
int sub;                                            // subcriber count as a integer for use in different functions
const char* serialdata;                             //serial data send to led matrix which has subscriber count
const char* subCount;                               // subcriber count but as a string for serial communication
const int send_signal_to_led_matrix = 5;                // indicator which send by led matrix and esp32 ready to send subcribers count


void setup() {
 // Serial.begin(115200);                            //for debug
   Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);       //this is neceassary to set baudrate of 2nd UART of esp32,which 9600 for atmega328p in led matrix
   delay(50);                                          
   WiFi.begin(ssid, password);                      //wifi begins in which esp32 ready to connect router
   while (WiFi.status() != WL_CONNECTED)
   {
    delay(100);
    Serial.println("Connecting to WiFi..");
   }
   Serial.println("Connected to the WiFi network");
   pinMode(send_signal_to_led_matrix, INPUT);                 //defines the mode of pin
 }
 
 
 //----------------------loop------------------------------------
void loop() {
  
  anything_new();                             //this fucntion simply check the state of send_signal_to_led_matrix pin to make sure led matrix wants subcribers count
//Serial.println(sub);                    // for debug
}

//----------------------check if any send signal is high,if high then send everything to atmega328p by calling google api and uart function------------------------------------

void anything_new()                        //as by the name of funtion 1st check the state of pin then
{ 
  if(digitalRead(send_signal_to_led_matrix) == HIGH)           //if pin is high  
   { 
    subscriber();                                               //call this function to use youtube api to get current subscribers count
   UART_esp32_to_arduino();                                   //then send the current sunbcribers to led matrix for display
   } 
   }

//----------------------Serial communication between esp32 and atmega328p------------------------------------

void UART_esp32_to_arduino()                               //by using 2nd UART of esp32,this function to send serial data to led matrix
{   serialdata = subCount;                                  //copy subcount,which is from subscriber() function
  Serial2.write(serialdata);                                //simple,Write the serial data
    }
//----------------------call google api then get json file deserialze it extract subscriber count------------------------------------
void subscriber()
{     if ((WiFi.status() == WL_CONNECTED)) { //Check the current connection status
 
    HTTPClient http;
    http.begin("https://www.googleapis.com/youtube/v3/channels?part=statistics&id=UCcuzwvvUHneSG_AcSkFWQ9Q&key=AIzaSyAZIsVhsXoWTwyap6siYmA9wW0ZDriUdPw"); //Specify the URL
    int httpCode = http.GET();                                        //Make the request
    if (httpCode > 0)                               //in return youtube api gives you json file in which details are specified in class form
     { 
    String mydata = http.getString();              // 1st save everthing as a string
    StaticJsonDocument<605> doc;                      //make a json document size because if data is too much then saved  data in document is not  enough
    deserializeJson(doc,mydata);                    //deserializejson converte json file into usable file which is easy to work with it.
    JsonObject items0 = doc["items"][0];          // now deserialize data are in class form so call class member in which subcriber count is situated
    JsonObject itemstat = items0["statistics"];       //item<[0]<statistics<subscriberCount
    subCount = itemstat["subscriberCount"];            // copy that in subcount char
    //Serial.println(subCount);                            // for debug
    sub=atoi(subCount);                             // subCoun as a string by use atoi convert into integer for future
    }
 
  else {
      Serial.println("Error on HTTP request");
       }
    http.end();                                       //Free the resources 
  }
  
}  
