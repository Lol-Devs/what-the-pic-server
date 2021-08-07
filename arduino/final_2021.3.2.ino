#include "WiFi.h"
#include <WebServer.h>
#include <time.h>
#include <ArduinoJson.h>
#include <FreeRTOS.h>

//camera imports
#include "esp_camera.h"
#include "Arduino.h"
#include "FS.h"                // SD Card ESP32
#include "SD_MMC.h"            // SD Card ESP32
#include "soc/soc.h"           // Disable brownour problems
#include "soc/rtc_cntl_reg.h"  // Disable brownour problems
#include "driver/rtc_io.h"
#include <EEPROM.h>            // read and write from flash memory

#include <HTTPClient.h>

#include <SD.h>

#include <iostream>
#include <fstream>
using namespace std;

//TaskData
TaskHandle_t TaskHandleTakePictures;
bool taskTakePictureIsRunning = false;

// initiate rest and wifi client
WebServer server(80);
// credentials for wifi connection
const char* SSID = "hackathon";
const char* PWD =  "summerhack21";

// JSON data buffer
StaticJsonDocument<250> jsonDocument;
char buffer[250];

//camera global vars
int INTERVAL = 10000;
int QUALITY = 10;

String serverName = "http://10.10.206.46:8000/api/v1/picture/";

// define the number of bytes you want to access
#define EEPROM_SIZE 1

// Pin definition for CAMERA_MODEL_AI_THINKER
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27

#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

bool RUNNING =false;

void setup() {
  Serial.begin(9600);
  connectToWiFi();
  setupRouting();
  syncTime();
  setupCamera();
}

void TaskTakePictures( void * parameter) {
  for(;;) {
    takePicture();
    vTaskDelay(INTERVAL);
  }
}
/*
 * Initial setup for wifi
 */
 void connectToWiFi() {
  Serial.print("Connecting to ");
  Serial.println(SSID);
  
  WiFi.begin(SSID, PWD);
  
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
    // we can even make the ESP32 to sleep
  }
 
  Serial.print("Connected. IP: ");
  Serial.println(WiFi.localIP());
}

// setup API resources
void setupRouting() {
  server.on("/takePictureNow",HTTP_POST, takePictureNow);
  server.on("/setRunning",HTTP_POST,setRunning);
  server.on("/SetInterval",HTTP_POST,setInterval);
  server.on("/SetQualityOnStartup",HTTP_POST,setQualityOnStartup);
 
  // start server
  server.begin();
}

/*
 * Inital setup for the camera
 */
void setupCamera() {
  
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG; 

  if(psramFound()){
    config.frame_size = FRAMESIZE_UXGA; // FRAMESIZE_ + QVGA|CIF|VGA|SVGA|XGA|SXGA|UXGA
    config.jpeg_quality = QUALITY; //org 10
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12; //org 12
    config.fb_count = 1;
  }
  
  // Init Camera
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

    //Serial.println("Starting SD Card");
  if(!SD_MMC.begin("/sdcard", true)){
    Serial.println("SD Card Mount Failed");
    return;
  }
  
  uint8_t cardType = SD_MMC.cardType();
  if(cardType == CARD_NONE){
    Serial.println("No SD Card attached");
    return;
  }

}

/*
 * Synchronizes the time with a internet time server
 */
void syncTime() {
   configTime(3 * 3600, 0, "0.it.pool.ntp.org", "1.it.pool.ntp.org");
  Serial.println("\nWaiting for time");
  while (!time(nullptr)) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("");
}

void loop() {
  server.handleClient();
}

/*
 * API-POINT: Takes a Photo right now
 */
void takePictureNow(){
  Serial.println("POST: Take picture now");
  takePicture();
  server.send(200, "application/json", "{}");
}

/*
 * Starts or Stops the interval-job
 */
void setRunning(){
  Serial.println("POST: Set running");
  if (server.hasArg("plain") == false) {
    //handle error here
  }
  String body = server.arg("plain");
  deserializeJson(jsonDocument, body);
  RUNNING = jsonDocument["running"];
  if(RUNNING and not taskTakePictureIsRunning){
      xTaskCreatePinnedToCore(
      TaskTakePictures, /* Function to implement the task */
      "TaskTakePictures", /* Name of the task */
      10000,  /* Stack size in words */
      NULL,  /* Task input parameter */
      0,  /* Priority of the task */
      &TaskHandleTakePictures,  /* Task handle. */
      0); /* Core where the task should run */  
  }else{
    vTaskDelete(TaskHandleTakePictures);
  }
  server.send(200, "application/json", "{}");
}

/*
 * Saves config that was received by Request to config.json 
 */
void setInterval(){
  Serial.println("POST: Set config");
  if (server.hasArg("plain") == false) {
    //handle error here
  }
  String body = server.arg("plain");
  deserializeJson(jsonDocument, body);
  if(!SD_MMC.begin("/sdcard", true)){
    Serial.println("SD Card Mount Failed");
    return;
  }

  File interval = SD_MMC.open("/interval.cofig", FILE_WRITE);
  if (!interval) {
    Serial.println("Opening file failed");
    return;
  }

  configs.print(jsonDocument["interval"]);
  configs.close();

  
  server.send(200, "application/json", "{}");
}
/*
 * Saves config that was received by Request to config.json 
 */
void setQuality(){
  Serial.println("POST: Set config");
  if (server.hasArg("plain") == false) {
    //handle error here
  }
  String body = server.arg("plain");
  deserializeJson(jsonDocument, body);
  if(!SD_MMC.begin("/sdcard", true)){
    Serial.println("SD Card Mount Failed");
    return;
  }

  File quality = SD_MMC.open("/quality.cofig", FILE_WRITE);
  if (!quality) {
    Serial.println("Opening file failed");
    return;
  }

  configs.print(jsonDocument["quality"]);
  configs.close();

  
  server.send(200, "application/json", "{}");
}


/*
 * Get the current time and date as String
 */
String getTimestamp() {
  time_t rawtime;
  struct tm * timeinfo;
  char timestampBuffer [80];

  time (&rawtime);
  timeinfo = localtime (&rawtime);

  strftime (timestampBuffer,80,"%F-%H-%M-%S",timeinfo);
  return timestampBuffer;
}

/*
 * Take a picture
 */
void takePicture() {
  pinMode(4, OUTPUT);
  digitalWrite(4, LOW);
  rtc_gpio_hold_en(GPIO_NUM_4);
  
  camera_fb_t * fb = NULL;
  
  // Take Picture with Camera
  fb = esp_camera_fb_get();  
  if(!fb) {
    Serial.println("Camera capture failed");
    return;
  }

  
  // initialize EEPROM with predefined size
  EEPROM.begin(EEPROM_SIZE);
   String timestamp = getTimestamp();
  Serial.printf("%s\n",timestamp);
  // Path where new picture will be saved in SD Card
  String path = "/"+ timestamp +".jpg";

  fs::FS &fs = SD_MMC; 
  Serial.printf("Picture file name: %s\n", path.c_str());
  
  File file = fs.open(path.c_str(), FILE_WRITE);
  if(!file){
    Serial.println("Failed to open file in writing mode");
  } 
  else {
    file.write(fb->buf, fb->len); // payload (image), payload length
    Serial.printf("Saved file to path: %s\n", path.c_str());
    //EEPROM.write("%s", timestamp);
    EEPROM.commit();
  }

  sendPhoto(fb, path);
  
  file.close();
  esp_camera_fb_return(fb); 

  digitalWrite(4, LOW);
  rtc_gpio_hold_en(GPIO_NUM_4);
}


String sendPhoto(camera_fb_t *fb, String filename) {

  String getAll;
  String getBody;

  WiFiClient client;
  String serverName = "10.10.206.46";
  String serverPath = "/api/v1/picture/" + WiFi.localIP().toString();

  if (client.connect("10.10.206.46", 8000)) {
    Serial.println("Connection successful!");
    
    String head = "--RandomNerdTutorials\r\nContent-Disposition: form-data; name=\"imageFile\"; filename=\"" + filename + "\"\r\nContent-Type: image/jpeg\r\n\r\n";
    String tail = "\r\n--RandomNerdTutorials--\r\n";

    uint32_t imageLen = fb->len;
    uint32_t extraLen = head.length() + tail.length();
    uint32_t totalLen = imageLen + extraLen;

    client.println("POST " + serverPath + " HTTP/1.1");
    client.println("Host: " + serverName);
    client.println("Content-Length: " + String(totalLen));
    client.println("Content-Type: multipart/form-data; boundary=RandomNerdTutorials");
    client.println();
    client.print(head);

    uint8_t *fbBuf = fb->buf;
    size_t fbLen = fb->len;
    for (size_t n=0; n<fbLen; n=n+1024) {
      if (n+1024 < fbLen) {
        client.write(fbBuf, 1024);
        fbBuf += 1024;
      }
      else if (fbLen%1024>0) {
        size_t remainder = fbLen%1024;
        client.write(fbBuf, remainder);
      }
    }   
    client.print(tail);
        int timoutTimer = 10000;
    long startTimer = millis();
    boolean state = false;
    
    while ((startTimer + timoutTimer) > millis()) {
      Serial.print(".");
      delay(100);      
      while (client.available()) {
        char c = client.read();
        if (c == '\n') {
          if (getAll.length()==0) { state=true; }
          getAll = "";
        }
        else if (c != '\r') { getAll += String(c); }
        if (state==true) { getBody += String(c); }
        startTimer = millis();
      }
      if (getBody.length()>0) { break; }
    }
    Serial.println();
    client.stop();
    Serial.println(getBody);
  }
  else {
    getBody = "Connection to " + serverName +  " failed.";
    Serial.println(getBody);
  }
  return getBody;
}


/*
 * Send the taken picture over the internet
 *
void sendPicture(String path) {

  
  HTTPClient http;
  String serverPath = serverName + WiFi.localIP();
  
  // Your Domain name with URL path or IP address with path
  http.begin(serverPath.c_str());
  http.addHeader("Content-Type", "application/json");
  // Send HTTP POST request
  int httpResponseCode = http.POST();
}*/

/*
 * Delete the picture with the given filename from the SD card
 */
void deletePicture(String filename) {
  
}

void create_json(char *tag, float value, char *unit) {  
  jsonDocument.clear();  
  jsonDocument["type"] = tag;
  jsonDocument["value"] = value;
  jsonDocument["unit"] = unit;
  serializeJson(jsonDocument, buffer);
}

void add_json_object(char *tag, float value, char *unit) {
  JsonObject obj = jsonDocument.createNestedObject();
  obj["type"] = tag;
  obj["value"] = value;
  obj["unit"] = unit; 
}
