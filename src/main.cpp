#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoWebsockets.h>
#include "esp_camera.h"

#define CAMERA_MODEL_AI_THINKER

#include "camera_pins.h"

using namespace websockets;

// WiFi Credentials
const char* ssid = "ESP";
const char* password = "12345678";

// WebSocket server details
const char* websockets_server_host = "esp.local";
const uint16_t websockets_server_port = 82;

WebsocketsClient client;
bool connected = false;
unsigned long lastConnectionAttempt = 0;
const unsigned long connectionRetryInterval = 5000; // 5 seconds between connection attempts

// Camera frame settings
static unsigned long lastFrameTime = 0;
const unsigned long frameInterval = 10; // ~5 FPS (200ms per frame)
// Watchdog Timer

volatile unsigned int connectToServer = 0;


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
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  config.grab_mode = CAMERA_GRAB_LATEST; // To grab the latest frame


  config.frame_size = FRAMESIZE_240X240;
  config.jpeg_quality = 12;
  config.fb_count = 1;


  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x\n", err);
    ESP.restart();
  }

}

void connectToWifi() {
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  
 

  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  if(WiFi.status() != WL_CONNECTED) {
    Serial.println("Failed to connect to WiFi");
    ESP.restart();
  }
  
  Serial.println("WiFi Connected");
  Serial.println(WiFi.localIP());
}

void onMessageCallback(WebsocketsMessage message) {
  Serial.println("Received from server: " + message.data());
}

void onEventsCallback(WebsocketsEvent event, String data) {
  if (event == WebsocketsEvent::ConnectionOpened) {
    Serial.println("WebSocket Connected!");
    connected = true;
  } 
  else if (event == WebsocketsEvent::ConnectionClosed) {
    Serial.println("WebSocket Disconnected!");
    connected = false;
  } 
  
}

bool ensureConnection() {
  if (connected && client.available()) {
    return true;
  }
    //Serial.println(connectToServer);
    if(connectToServer == 5) {
        ESP.restart();
    }

      unsigned long now = millis();
    if (now - lastConnectionAttempt < connectionRetryInterval) {
      return false;
    }
    lastConnectionAttempt = now;
    Serial.println("Attempting to connect/reconnect to WebSocket server");
    
    connected = client.connect(websockets_server_host, websockets_server_port, "/ws");
    if (connected) {
      client.send("ESP32-CAM Connected");
      Serial.println("WebSocket client connected to server");
        connectToServer += 1;

    } else {
      Serial.println("WebSocket connection failed!");
            connectToServer += 1;

    }
    
  return connected;
}

void setup() {
  Serial.begin(115200);

  connectToWifi();
  setupCamera();

  client.onMessage(onMessageCallback);
  client.onEvent(onEventsCallback);
  
  ensureConnection();
}

void loop() {
  // Handle WebSocket events
  if (connected) {
      client.poll();
  } 
  else {
    ensureConnection();
    delay(100);
    return;
  }

  // Frame rate limiting
  unsigned long now = millis();
  if (now - lastFrameTime < frameInterval) {
    return;
  }
  lastFrameTime = now;

  // Capture frame
  camera_fb_t *fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Camera capture failed");
    return;
  }

  // Send frame
  if(client.available())
  {
      bool success = client.sendBinary((const char *)fb->buf, fb->len);
      if (!success) {
        Serial.println("Frame send failed");
      } 
      else {
        Serial.printf("Frame sent | Heap: %lu | size: %u\n", ESP.getFreeHeap(), fb->len);
      }
  }else {
    connected = false;
  }


  esp_camera_fb_return(fb);
}