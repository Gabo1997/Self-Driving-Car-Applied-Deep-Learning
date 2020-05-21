#include "esp_camera.h"
#include <WiFi.h>
//#include <analogWrite.h>
//#include "esp_task_wdt.h"
//
// WARNING!!! Make sure that you have either selected ESP32 Wrover Module,
//            or another board which has PSRAM enabled
//

// Select camera model
//#define CAMERA_MODEL_WROVER_KIT
//#define CAMERA_MODEL_ESP_EYE
//#define CAMERA_MODEL_M5STACK_PSRAM
//#define CAMERA_MODEL_M5STACK_WIDE
#define CAMERA_MODEL_AI_THINKER

#include "camera_pins.h"

const char* ssid = "DESKTOP-339FV3P 4123";
const char* password =  "1fM349+2";

void startCameraServer();

//// Motor Derecho A
extern int vel_der = 15;
int INA=15;

//// Motor Izquierdo B
extern int vel_izq = 2;
int INB=2; 


extern int gpB =  12; // Wheel Back
extern int gpF = 13; //  Wheel Forward

extern int gpLed =  4; // Light


extern String WiFiAddr ="";

//unsigned long now = millis();
//unsigned long lastTrigger = 0;

//WiFiServer wifiServer(81);

//TaskHandle_t Task1;

void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();
  
  pinMode(gpB, OUTPUT); //Left Backward
  pinMode(gpF, OUTPUT); //Left Forward
  pinMode(gpLed, OUTPUT); //Light
  // Initialize channels 
  // channels 0-15, resolution 1-16 bits, freq limits depend on resolution
  // ledcSetup(uint8_t channel, uint32_t freq, uint8_t resolution_bits);
  ledcSetup(vel_der,5000,8);// 12 kHz PWM, 8-bit resolution
  ledcSetup(vel_izq,5000,8);
  ledcAttachPin(INA, vel_der);
  ledcAttachPin(INB, vel_izq);
  
  //initialize
  digitalWrite(gpB, LOW);
  digitalWrite(gpF, LOW);
  digitalWrite(gpLed, LOW);
  ledcWrite(vel_der,0);
  ledcWrite(vel_izq,0);
  
//  pinMode (ENA, OUTPUT);
//  pinMode (ENB, OUTPUT);
//  pinMode (IN1, OUTPUT);
//  pinMode (IN2, OUTPUT);
//  pinMode (IN3, OUTPUT);
//  pinMode (IN4, OUTPUT);
  
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
  //init with high specs to pre-allocate larger buffers
  if(psramFound()){
    config.frame_size = FRAMESIZE_UXGA;
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 5;
    config.fb_count = 1;
  }

#if defined(CAMERA_MODEL_ESP_EYE)
  pinMode(13, INPUT_PULLUP);
  pinMode(14, INPUT_PULLUP);
#endif

  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }
  sensor_t * s = esp_camera_sensor_get();
  //initial sensors are flipped vertically and colors are a bit saturated
  if (s->id.PID == OV3660_PID) {
    s->set_vflip(s, 1);//flip it back
    s->set_brightness(s, 1);//up the blightness just a bit
    s->set_saturation(s, -2);//lower the saturation
  }
  //drop down frame size for higher initial frame rate
  s->set_framesize(s, FRAMESIZE_QVGA);

#if defined(CAMERA_MODEL_M5STACK_WIDE)
  s->set_vflip(s, 1);
  s->set_hmirror(s, 1);
#endif

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  startCameraServer();

  Serial.print("Camera Ready! Use 'http://");
  Serial.print(WiFi.localIP());
  WiFiAddr = WiFi.localIP().toString();
  Serial.println("' to connect");

  
//  wifiServer.begin();
//  xTaskCreatePinnedToCore(
//      motores, /* Function to implement the task */
//      "Task1", /* Name of the task */
//      10000,  /* Stack size in words */
//      NULL,  /* Task input parameter */
//      5,  /* Priority of the task */
//      &Task1,  /* Task handle. */
//      0); /* Core where the task should run */
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(10000);
}
//void motores(void * pvParameters){
//  for(;;){
//    WiFiClient client = wifiServer.available();
//    if (client) {
//      Serial.println("Client connected");
//      while (client.connected()) {
//        while (client.available()>0) {      
//          //client.write(c);
//          char c = client.read();
//          if (c=='w'){
//            lastTrigger=millis();
//            Adelante();
//          }else if(c=='a'){
//            lastTrigger=millis();
//            Izquierda();
//          }else if(c=='d'){
//            lastTrigger=millis();
//            Derecha();
//          }else if(c=='s'){
//            lastTrigger=millis();
//            Atras();
//          }
//        }
//        now=millis();
//        if(now-lastTrigger>=300){
//           Detenerse();
//        }
//        esp_task_wdt_reset();
//      }
//      client.write("Client disconnected");
//    }
//    esp_task_wdt_reset();
//  }
//}
