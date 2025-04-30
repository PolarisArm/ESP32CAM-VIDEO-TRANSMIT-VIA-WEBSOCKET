# ESP32CAM-VIDEO-TRANSMIT-VIA-WEBSOCKET

This project enables real-time video streaming from an ESP32-CAM module to a WebSocket server. The ESP32-CAM captures images from its camera and sends them as binary data over WebSockets, making it suitable for applications like surveillance systems, remote monitoring, or IoT-based camera projects.

### This project is prequiste for the [ESP32CAM-VIDEO-RECIVE-AND-SHOW-IN-ST7789-DISPLAY](https://github.com/PolarisArm/ESP32CAM-VIDEO-RECIVE-AND-SHOW-IN-ST7789-DISPLAY.git)


## Features

- Real-time video streaming at ~5 FPS
- WebSocket binary data transmission
- Automatic reconnection to WiFi and WebSocket server
- Error handling and recovery mechanisms
- Low latency streaming
- 240x240 resolution JPEG compression

## Hardware Requirements

- ESP32-CAM module (AI-THINKER model)
- USB-to-TTL converter (for programming)
- 5V power supply
- Optional: External antenna for better WiFi reception

## Software Requirements

- Arduino IDE
- ESP32 board support package
- Required libraries:
  - Arduino.h
  - WiFi.h
  - ArduinoWebsockets
  - esp_camera.h

## Installation

1. Install the Arduino IDE
2. Add ESP32 board support to Arduino IDE
   - Go to File -> Preferences
   - Add `https://dl.espressif.com/dl/package_esp32_index.json` to the "Additional Boards Manager URLs" field
   - Go to Tools -> Board -> Boards Manager
   - Search for ESP32 and install the ESP32 package
3. Install the required libraries:
   - Go to Sketch -> Include Library -> Manage Libraries
   - Search for and install:
     - ArduinoWebsockets by Gil Maimon

## Wiring for Programming

When programming the ESP32-CAM, connect it to your computer using a USB-to-TTL converter with the following connections:

| ESP32-CAM | USB-to-TTL Converter |
|-----------|----------------------|
| 5V        | 5V                   |
| GND       | GND                  |
| U0R (TX)  | RX                   |
| U0T (RX)  | TX                   |
| IO0       | GND (only during upload) |

Remember to press the reset button after connecting IO0 to GND and before uploading the code.

## Configuration

Update the following parameters in the code according to your setup:

```cpp
// WiFi Credentials
const char* ssid = "ESP";  // Your WiFi network name
const char* password = "12345678";  // Your WiFi password

// WebSocket server details
const char* websockets_server_host = "esp.local";  // WebSocket server address
const uint16_t websockets_server_port = 82;  // WebSocket server port
```

## Camera Settings

The camera is configured with the following settings:

- Frame size: 240x240 pixels
- JPEG quality: 12 (0-63, lower is better quality)
- Frame buffer count: 1
- Frame rate: ~5 FPS

These settings can be adjusted in the `setupCamera()` function.

## WebSocket Server

This code assumes you have a WebSocket server running at the specified address and port. The server should:

1. Accept WebSocket connections
2. Handle binary data (JPEG frames)
3. Process the received frames as needed (display, save, etc.)

## How It Works

1. The ESP32-CAM connects to the specified WiFi network
2. It establishes a WebSocket connection to the server
3. The camera captures frames at regular intervals
4. Each frame is sent as binary data through the WebSocket connection
5. If the connection is lost, the system will automatically attempt to reconnect

## Error Handling

- If the camera fails to initialize, the ESP32 will restart
- If WiFi connection fails, the system will retry indefinitely
- If the WebSocket connection fails, it will attempt to reconnect every 5 seconds
- After 5 failed WebSocket connection attempts, the ESP32 will restart

## Troubleshooting

1. **Camera initialization failed**: 
   - Check the camera module connections
   - Make sure the camera is properly seated in its connector

2. **WiFi connection issues**:
   - Verify the SSID and password are correct
   - Check if the WiFi signal is strong enough where the ESP32-CAM is placed

3. **WebSocket connection fails**:
   - Ensure the WebSocket server is running and accessible
   - Check if the server address and port are correctly configured
   - Verify network connectivity between the ESP32-CAM and the server

4. **Poor image quality or low frame rate**:
   - Adjust the JPEG quality and frame size in the camera configuration
   - Ensure adequate lighting for the camera
   - Check the available bandwidth of your network

## Power Considerations

The ESP32-CAM can consume significant power, especially when streaming video. For stable operation:

- Use a good quality 5V power supply capable of providing at least 500mA
- Consider using a separate power supply rather than powering through the USB-to-TTL converter
- For battery-powered applications, implement power saving techniques

## License

This project is open-source and free to use, modify, and distribute.

## Acknowledgments

- ESP32 Camera library by Espressif
- ArduinoWebsockets library by Gil Maimon
