# Arduino IDE Setup Guide

## Required Libraries

Install these libraries through the Arduino Library Manager:

1. **TFT_eSPI** by Bodmer
   - Version: 2.5.43 or later
   - Used for: 2.8" ILI9341 TFT display control

2. **ESP32Encoder** by madhephaestus  
   - Version: 0.10.2 or later
   - Used for: Rotary encoder input handling

## TFT_eSPI Configuration

The TFT_eSPI library requires configuration for your specific display. 

### Method 1: User_Setup.h (Recommended)
1. Navigate to your Arduino libraries folder
2. Find `TFT_eSPI/User_Setup.h`
3. Add these settings:

```cpp
// Driver selection
#define ILI9341_DRIVER

// ESP32-S3 SPI pins for display
#define TFT_MISO 12
#define TFT_MOSI 11
#define TFT_SCLK 13
#define TFT_CS   10
#define TFT_DC   14
#define TFT_RST  15

// SPI frequency
#define SPI_FREQUENCY  40000000
```

### Method 2: Setup Selection
Alternatively, in `User_Setup_Select.h`, uncomment:
```cpp
#include <User_Setups/Setup25_TTGO_T_Display.h>
```
Then modify Setup25 with the pin definitions above.

## Board Selection

1. **Install ESP32 Board Package**:
   - Go to File → Preferences
   - Add to Additional Board Manager URLs:
     ```
     https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
     ```
   - Go to Tools → Board → Boards Manager
   - Search for "ESP32" and install "esp32 by Espressif Systems"

2. **Select Board**:
   - Tools → Board → ESP32 Arduino → ESP32S3 Dev Module

3. **Configure Settings**:
   ```
   Board: ESP32S3 Dev Module
   Upload Speed: 921600
   CPU Frequency: 240MHz (WiFi/BT)
   Flash Mode: QIO 80MHz
   Flash Size: 4MB (32Mb)
   Partition Scheme: Default 4MB with spiffs
   Core Debug Level: None
   USB CDC On Boot: Enabled
   Erase All Flash Before Sketch Upload: Disabled
   ```

## Uploading the Sketch

1. Connect your ESP32-S3 to computer via USB
2. Select the correct COM port in Tools → Port
3. Click Upload button

## Troubleshooting

### Upload Issues
- **Boot Mode**: Hold BOOT button while clicking Upload, release after upload starts
- **Driver Issues**: Install CP210x or CH340 USB drivers
- **Port Not Found**: Check USB cable, try different port

### Display Issues
- **Blank Screen**: Check TFT_eSPI pin configuration
- **Incorrect Colors**: Verify SPI connections and frequency
- **Flickering**: Reduce SPI frequency to 20MHz

### Audio Issues
- **No Sound**: Check I2S DAC connections and power
- **Distorted Audio**: Verify I2S pin assignments
- **Low Volume**: Adjust sample amplitude in code

## Pin Connections Reference

See `hardware/schematics/esp32-s3-pinout.md` for complete wiring diagram.

## Example Serial Output

When running correctly, you should see:
```
MintySynth ESP32-S3 Starting...
MintySynth ESP32-S3 Ready!
Step 1 ON
PLAY
Trigger step 1 Note: C4
Voice: 2
```