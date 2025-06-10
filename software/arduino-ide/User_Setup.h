/*
 * TFT_eSPI User Setup for MintySynth ESP32-S3
 * Configuration for 2.8" ILI9341 display with software SPI
 * 
 * INSTRUCTIONS:
 * Copy this file to your Arduino libraries folder:
 * Documents/Arduino/libraries/TFT_eSPI/User_Setup.h
 * 
 * Or create a User_Setup_Select.h file in the same directory and uncomment:
 * #include <User_Setups/Setup_MintySynth.h>
 */

// Driver selection - ILI9341 2.8" display
#define ILI9341_DRIVER

// *** IMPORTANT: Comment out parallel mode - we're using SPI ***
// #define ESP32_PARALLEL

// Pin definitions for MintySynth ESP32-S3 hardware
#define TFT_CS   10  // Chip select control pin
#define TFT_DC    9  // Data Command control pin  
#define TFT_RST  14  // Reset pin (could connect to ESP32 RST, see next line)
//#define TFT_RST  -1   // Set TFT_RST to -1 if display RESET is connected to ESP32 board RST

// Software SPI pins (slower but uses any GPIO pins)
#define TFT_MOSI 11  // Data out (SDA)
#define TFT_SCLK 13  // Clock out (SCL)

// MISO not needed for display-only communication
// #define TFT_MISO -1

// Tell the library we're using SPI interface (not parallel)
#define TFT_SPI_MODE

// SPI frequency - start conservative, can increase if stable
#define SPI_FREQUENCY  20000000  // 20MHz - good balance of speed/stability
#define SPI_READ_FREQUENCY  20000000
#define SPI_TOUCH_FREQUENCY  2500000

// Color depth - 16-bit gives good color with reasonable memory usage  
#define TFT_RGB_ORDER TFT_BGR  // Colour order Blue-Green-Red

// Display size
#define TFT_WIDTH  240
#define TFT_HEIGHT 320

// Fonts to load (comment out unused fonts to save memory)
#define LOAD_GLCD   // Font 1. Original Adafruit 8 pixel font needs ~1820 bytes in FLASH
#define LOAD_FONT2  // Font 2. Small 16 pixel high font, needs ~3534 bytes in FLASH, 96 characters
#define LOAD_FONT4  // Font 4. Medium 26 pixel high font, needs ~5848 bytes in FLASH, 96 characters
#define LOAD_FONT6  // Font 6. Large 48 pixel high font, needs ~2666 bytes in FLASH, only characters 1234567890:-.apm
#define LOAD_FONT7  // Font 7. 7 segment 48 pixel high font, needs ~2438 bytes in FLASH, only characters 1234567890:.
#define LOAD_FONT8  // Font 8. Large 75 pixel high font needs ~3256 bytes in FLASH, only characters 1234567890:-.
//#define LOAD_FONT8N // Font 8. Alternative to Font 8 above, slightly narrower, so 3 digits fit a 160 pixel TFT
#define LOAD_GFXFF  // FreeFonts. Include access to the 48 Adafruit_GFX free fonts FF1 to FF48 and custom fonts

// Smooth (anti-aliased) fonts
#define SMOOTH_FONT

// Performance settings
#define SPI_TOUCH_FREQUENCY  2500000

// Comment out if you don't want to use DMA (can cause issues with some setups)
// #define TFT_DMA

// ESP32-S3 specific optimizations
// #define USE_HSPI_PORT  // Comment out - let library auto-select

// Debug settings (comment out for production)
// #define TFT_DEBUG

/*
 * WIRING DIAGRAM for MintySynth ESP32-S3:
 * 
 * ILI9341 Display -> ESP32-S3
 * VCC     -> 3.3V
 * GND     -> GND  
 * CS      -> GPIO 10
 * RESET   -> GPIO 14
 * DC      -> GPIO 9
 * SDI(MOSI) -> GPIO 11
 * SCK     -> GPIO 13
 * LED     -> 3.3V (backlight always on)
 * SDO(MISO) -> Not connected (display-only)
 */