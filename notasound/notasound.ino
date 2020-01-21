/*
 * notasound - IR controlled 'synchronized' led lighting effects using FastLED across multiple Arduino AVR based controllers with sound reactive display sequences.
 * 
 *       By: Andrew Tuline
 *     Date: May, 2019 (latest update)
 *      URL: www.tuline.com
 *    Email: atuline@gmail.com
 *     Gist: https://gist.github.com/atuline
 *   GitHub: https://github.com/atuline
 *  Youtube: https://www.youtube.com/user/atuline/videos
 *  
 *  FastLED Support Forums: https://www.reddit.com/r/FastLED/
 *  
 * 
 * This works best with Arduino Nano, a TSOP38238 IR receiver, a 3.3V ADMP401 microphone and APA102 LED's.
 * 
 * CAUTION ************************************************************************************************************************************************
 * 
 * Before attempting to compile this program, make sure you are already comfortable programming in Arduino C and with FastLED. In addition, you
 * should already be able to download, install and USE 3rd party libraries. If you are a beginner, this is NOT the code you're looking for.
 * 
 * Effects in this program are programmed for up to 255 LED's. You will need to re-write it for longer strips.
 * 
 * These have been tested thoroughly on an Arduino Nano, but not other platforms and on the Nano, IR control works great with APA102's, but is unreliable
 * with WS2812's.
 * 
 * 
 * INTRODUCTION *******************************************************************************************************************************************
 *
 * Here's a pile of sound reactive display routines for Arduino microcontrollers (tested on a Nano) and addressable LED strips using the FastLED display library.
 * Each Arduino is IR controlled using Nico Hood's IR control library. The sound routines are customized for the 3.3V ADMP401 MEMS microphone.
 * 
 * 
 * My LED animation design philosophy is to:
 * 
 * NOT use delay statements in the loop as that breaks input routines (i.e. buttons).
 * No nested loops (for performance reasons).
 * Use millis() as a realtime counter, unless a fixed counter is otherwise required.
 * Spend a bit more time on high school math, rather than the elementary school arithmetic method of counting pixels.
 * Keep the display routines as short as possible.
 * Data typing as strict as possible i.e. why define an int when a uint8_t is all that is required, and no floats at all.
 * Localize variables to each routine as much as possible.
 * Break out the display routines into separate .h files for increased readability.
 * Be generous with comments and documentation.
 *
 *
 * VERSIONS ***********************************************************************************************************************************************
 * 
 * TODO
 * 
 * More testing of each routine for a quality of display. Let's see if we can make the best of the best.
 * 
 * 1.11
 * 
 * Adding agcAvg functionality and tune up the existing routines.
 * Also, try and line up notasound overall functionality with notamesh.
 * 
 * 
 * 1.10
 * 
 * I forgot to update some of the IR functionality for EEPROM with strand length and mesh delay. That's now fixed.
 * Also added more Serial.println statements for button presses.
 * Retrofit some updates that I'd made for notamesh.
 * 
 * 
 * 1.09
 * Updated IRLRemote to 2.0.2, which changes my code. Although it does work with WS2812, it doesn't do so very reliably.
 * Added Serial.print(F("Text") in order to save on RAM.
 * 
 * 
 * 1.08
 * 
 * Convert squelch control to save/read from EEPROM.
 * Add maxVol changes to increase/decrease peak detection sensitivity and save/read from EEPROM.
 * Stop the animation while we're setting strand length or delay.
 * Permanently removed strand favourites. A single startup mode should suffice.
 * Convert glitter control to save/read from EEPROM.
 * Convert brightness control to save/read from EEPROM.
 * Convert direction control to save/read from EEPROM.
 * Convert delay control to save/read from EEPROM. Not useful, as each routine has its own unique delay.
 * 
 * 
 * 1.07 Sampling fix, general fixes and a display change.
 * 
 * Change sampling algorithm to direct method. Can sample at various rates, but more complex to use.
 * Changed some of the averaging and squelching math.
 * Fix the samplepeak detection for the various routines. Each display routine must now reset samplepeak. Wow, what a realization that was.
 * Had a problem with it crashing consistently 80 seconds in with 40 LED's. Lowered the max_bright and problem went away. Strange.
 * Turned mode 0 from solid red to an ambient noise routine.
 * Fixed a couple of displays that addressed leds[NUM_LEDS]
 * Adjusted the default squelch value.
 * 
 * 
 * 1.06 Big update/change by merging notamesh framework with the soundmems demo routines to combine sound reactive functionality with IR control.
 * 
 * Use notamesh framework and combine/update soundmems demos.
 * Change potin squelch control to IR control with B4 and C1 buttons.
 * Adjust/fix some of the routines to even out sensitivity and limits.
 * Update the documentation.
 * 
 * 
 * 1.05 Update
 * 
 * Set/display a favourite.
 * 
 * 
 * 1.04 Update
 * 
 * Make programming length and delay to be unit selectable.
 * Change demo mode to start at ledMode 2.
 * Set default to non-demo mode.
 * Add plasma, remove circnoise routines.
 * NEW FUNCTIONALITY ---> Supports delayed sequences across Arduinos, which you might see in a mesh network. But it's 'notamesh' network.
 * NEW FUNCTIONALITY ---> Use paletteknife palettes (for notamesh palette consistency across multiple strands) rather than the previous random generated ones.
 * Re-add STRANDID.
 *
 * 
 * 1.03 Update
 *
 * Convert 'thisdir' direction variable from boolean to -1, 1 and update all directional based routines.
 * Reduce direction IR button press to a toggle.
 * Fix IR button pressed modes that don't animate.
 * Move IR codes to commands.h and update the values to the 24 button pad I have.
 * Remove keyboard and button functionality. I don't use it.
 * Remove STRANDID and strandactive security features, as well as select individual Arduino. I may bring this back.
 * Save strandlength after every change to B2/B3 instead of a separate 'save' command.
 * Add EEPROM initialization check and save initial values.
 * Add notamesh functionality which configures a delay. It displays 1 white LED per 100ms delay.
 * 
 * 
 * 1.02 Update and prior
 * 
 * Old stuff, to be sure.
 * 
 * 
 * HARDWARE SETUP (for Arduino Nano) **********************************************************************************************************************
 * 
 * This has been developed using an Arduino Nano.
 * The LED data line is connected to pin 12.
 * The LED clock line is connected to pin 11.
 * For IR functionality, connect the data output of a TSOP34838 38kHz IR receiver to pin D2 (other TSOP pins are 5V and Gnd).
 * This NOW supports If you use WS2812 LED's (but not very well).
 * This is currently configured to support up to 64 LED's. You can increase this by changing MAX_LEDS, up to 255 (not tested) with adequate memory.
 * Connect the AREF pin to 3.3V on the Arduino if using a 3.3V microphone (such as the ADMP401).
 * Connect the analog output of the ADMP401 to A5 on the Arduino.
 * 
 * 
 * 
 * EEPROM Functionality ***********************************************************************************************************************************
 * 
 * Save the startup display mode in EEPROM.
 * Save the ID of the strand so that length and meshdelay can be programmed individually.
 * Save the strand length in EEPROM, however the MAXIMUM strand length is pre-defined. We just use a lesser value for the array.
 * Save the mesh delay in EEPROM by 100ms at a time.
 * Save the squelch value in EEPROM, adjustable +/- 1.
 * Save the direction in EEPROM, adjustable +/-1.
 * Save the brightness in EEPROM, adjustable * or / 2.
 * Save the peak detection sensitivity in EEPROM, adjustable +/- 1.
 * 
 * 
 * Libraries Required (to download) ***********************************************************************************************************************
 * 
 * FastLED library from https://github.com/FastLED/FastLED
 * Nico Hood's IRL library from https://github.com/NicoHood/IRLremote
 * 
 * I don't know if there's any conflict between Ken Shiriff's IR Library and Nico's. I'll leave that up to you to sort out. Then there's ESP8266 based IR libraries.
 * 
 * 
 * Compile Time Configuration ******************************************************************************************************************************
 * 
 * Configure and compile notasound for your type of strand, microphone, IR and LED pins used.
 * Configure the STRANDID value for your Arduino (corresponding to a keypress value). Each Arduino should have a unique STRANDID, and you should write that down.
 * Set the demorun startup variable to 1 for demo mode or 0 for regular use. It's configured to 0 (non-demo mode) by default.
 * You may need to adjust soundmems.h for your microphone. It's currently configured for an ADMP401.
 * You may need to review the analogreference() in setup() for your microphone as well. It's currently configured for the 3.3V ADMP401.
 *
 * 
 * First Time Initialization ******************************************************************************************************************************
 * 
 * The first time notasound runs on an Arduino, the setup program will initialize the EEPROM so that:
 * 
 * An Arduino initialization flag will be set.
 * The starting mode will be 0 (a non-sound reactive display).
 * The starting NUM_LEDS length will be 20 LED's.
 * The starting notasound delay will be 0 ms (this only works with notamesh).
 * The starting squelch value is 20.
 * The starting maxVol for peak detection is 20.
 * 
 * If you want to re-initialize the EEPROM values, then change the value of INITVAL at compile time. You can also reboot twice within 5 seconds.
 * 
 * 
 * Notasound Initial Configuration ************************************************************************************************************************
 * 
 * 1) Set your strand length.
 * 
 * You'll need to 'activate' your strand for EEPROM programming. To do so:
 * 
 * - Press B1 to put the Arduino into 'Select strand' mode.
 * - Press the button equivalent to your STRANDID, i.e. C1 to 'activate' your Arduino for EEPROM programming.
 * - Press B2 to decrease the strand length.
 * - Press B3 to increase the strand length.
 * 
 * LED's will light up as 'white' to indicate the strand length. The strand length will be saved to EEPROM after each keypress.
 * Once done, press B1 again or press A3 to reset the Arduino.
 * 
 * 
 * 2) To increase/decrease the mesh delay (which works best with notamesh):
 * 
 * Again, you'll need to 'activate' your strand for EEPROM programming. To do so:
 * 
 * - Press B1 to put the Arduino into 'Select strand' mode.
 * - Press the button equivalent to your STRANDID, i.e. C1 to 'activate' your Arduino for EEPROM programming.
 * - Press E2 to decrease the amount of mesh delay by 100ms.
 * - Press E3 to increase the amount of mesh delay by 100ms.
 * 
 * LED's will light up as 'white' to indicate the mesh delay (1 led per 100ms). The mesh delay will be saved to EEPROM after each keypress.
 * Once done, press B1 again or A3 to reset the Arduino.
 * 
 * 
 * 
 * 
 * Notasound Operation **********************************************************************************************************************************
 * 
 * The notasound initial configuration is important so that there's a different delay between each LED strip.
 * Press A3 once all notasound Arduinos are running in order to synchronize them with the same millis() value.
 * From there, you should be able to select demo mode or individual sequences below.
 * The routines should run and you should get a cool synchronized display across the strips.
 * 
 * Note: Although the delay functionality doesn't work with notasound, each strand running the same sequence will look a bit different,
 * depending on the location of the microphone. 
 * 
 * 
 * 
 * IR Keys and Operation ********************************************************************************************************************************
 * 
 * This configuration uses a 24 button IR remote like the ones provided with 5V LED strands from China. If you use a different one, you'll need to map
 * the codes to the modes in the irtest(); routine in the commands.h file.
 * 
 * The chart below summarizes the commands. Button location uses characters as the row, and numbers are the columns, so C2 is the 3rd row, 2nd column.
 * 
 * 
 * Command                          IR Button location & description
 * --------                         --------------------------------
 * Increase brightness              A1  Increase brightness
 * Decrease brightness              A2  Decrease brightness
 * Reset and set mode 0             A3  Reboots the Arduino in order to sync millis() if using notasound. Factory reset if < 2 seconds.
 * Enable demo mode                 A4  Demo mode cycles through the routines based on the millis() counter. Not a toggle.
 * 
 * Select Arduino                   B1  This allows the EEPROM to be updated. Then press A1 through F4 as configured with STRANDID at compile time. (not A3 or B1 though).
 * Decrease strand length           B2  The # of LED's programmed are white, only if strand is active (via B1 & STRANDID). This is saved in EEPROM. Press B1 again or A3 to reboot when done.
 * Increase strand length           B3  The # of LED's programmed are white, only if strand is active (via B1 & STRANDID). This is saved in EEPROM. Press B1 again or A3 to reboot when done.
 * Palette rotation                 B4  Start palette rotation.
 * 
 * Save palette                     C1  Stop palette rotation and save current palette to EEPROM.
 * Slower animation                 C2  Increase value of EVERY_N_MILLISECONDS() for the current (fixed) display mode.
 * Faster animation                 C3  Decrease value of EVERY_N_MILLISECONDS() for the current (fixed) display mode.
 * Toggle direction                 C4  Toggle direction of some sequences. This is saved in EEPROM however very few demos use it.
 * 
 * Enable/disable glitter           D1  Toggles glitter. This is saved in EEPROM.
 * Previous display mode            D2  Also disables demo mode.
 * Next display mode                D3  Also disables demo mode.
 * Save Current mode to EEPROM      D4  This will be the startup mode, and disables demo mode temporarily (if it was enabled).
 * 
 * Decrease maxVol                  E1  Which increases peak detection sensitivity. This is saved in EEPROM.
 * Shorter mesh delay               E2  Decrease mesh delay by 100ms before starting (using white LED's), only if strand is active (with the Select Arduino command). This is saved in EEPROM. Press B1 again or A3 to reboot when done.
 * Longer mesh delay                E3  Increase mesh delay by 100ms before starting (using white LED's), only if strand is active (with the Select Arduino command). This is saved in EEPROM. Press B1 again or A3 to reboot when done.
 * Increase maxVol                  E4  Which decreases peak detection sensitivity. This is saved in EEPROM.
 * 
 * Decrease noise squelch           F1  Allows more ambient noise is displayed. This is saved in EEPROM.
 * Select previous palette          F2  Stop palette rotation and select previous palette immediately.
 * Select next palette              F3  Stop palette rotation and select next palette immediately.
 * Increase noise squelch           F4  Increases noise squelch, so that ambient noise = 0. This is saved in EEPROM.
 * 
 * 
 * Change to:
 * 
 * 
 * Nothing                          B4
 *                                  
 * Nothing                          C1
 * Same                             C2
 * Same                             C3
 * Same                             C4
 * 
 * Decrease maxVol                  D1
 * Increase maxVol                  D2
 * Decrease squelch                 D3
 * Increase squelch                 D4
 * 
 * Stop palette rotation            E1  Stop palette rotation at current palette. Save palette to EEPROM.
 * Select previous palette          E2  Stop palette rotation and select previous palette immediately. Save palette to EEPROM.
 * Select next palette              E3  Stop palette rotation and select next palette immediately. Save Palette to EEPROM.
 * Enable palette rotation          E4  Enable palette transitioning every 5 seconds.
 * 
 * Enable hue based palette         F1  Select palette that is hue/saturation changeable. 
 * Previous display mode            F2  Also disables demo mode.
 * Next display mode                F3  Also disables demo mode.
 * Save Current mode to EEPROM      F4  This will be the startup mode, and disables demo mode temporarily. This is saved in EEPROM.
 * 
 */


/*------------------------------------------------------------------------------------------
--------------------------------------- Start of variables ---------------------------------
------------------------------------------------------------------------------------------*/


//#define _ESP8266                                            // Uncomment this if using ESP8266. It's required for EEPROM changes between the platforms.
#define _NANO                                                 // Uncomment this if using UNO/Nano or similar AVR.

#define qsubd(x, b)  ((x>b)?x:0)                              // A digital unsigned subtraction macro. if result <0, then => 0. Otherwise, take on fixed value.
#define qsuba(x, b)  ((x>b)?x-b:0)                            // Unsigned subtraction macro. if result <0, then => 0.

#define NOTASOUND_VERSION 110                                 // Just a continuation of notamesh, previously seirlight and previous to that was irlight, then aalight and then atlight. Turtles. . . 

//#define FASTLED_ALLOW_INTERRUPTS 0                          // Used for ESP8266.
#include <FastLED.h>                                          // FastLED library.

#include "IRLremote.h"                                        // https://github.com/NicoHood/IRLremote
#include "EEPROM.h"                                           // This is included with base install

#include "commands.h"                                         // The IR commands transmitted from the keypad.

#if IRL_VERSION < 202
#error "Requires IRLRemote 2.0.2 or later. Check github for latest code."
#endif

#if FASTLED_VERSION < 3001000
#error "Requires FastLED 3.1 or later. Check github for latest code."
#endif

#define pinIR 2                                               // Choose a valid PinInterrupt pin of your Arduino board for IR operations. In this case, D2.
#define IRL_BLOCKING true
uint16_t IRAddress;
uint8_t IRCommand;

CNec IRLremote;


// Fixed definitions cannot change on the fly.
#define LED_DT 12                                             // Serial data pin for all strands
#define LED_CK 11                                             // Serial clock pin for WS2801 or APA102
#define COLOR_ORDER GRB                                       // It's GRB for WS2812
#define LED_TYPE WS2812                                       // Alternatively WS2801, or WS2812
#define MAX_LEDS 64                                           // Maximum number of LED's defined (at compile time).

#define MIC_PIN   5                                           // Analog port for microphone
uint8_t squelch = 7;                                          // Anything below this is background noise, so we'll make it '0'.
int sample;                                                   // Current sample.
float sampleAvg = 0;                                          // Smoothed Average.
float micLev = 0;                                             // Used to convert returned value to have '0' as minimum.
uint8_t maxVol = 11;                                          // Reasonable value for constant volume for 'peak detector', as it won't always trigger.
bool samplePeak = 0;                                          // Boolean flag for peak. Responding routine must reset this flag.

int sampleAgc, multAgc;
uint8_t targetAgc = 60;                                       // This is our setPoint at 20% of max for the adjusted output.


// Initialize changeable global variables.
uint8_t NUM_LEDS;                                             // Number of LED's we're actually using, and we can change this only the fly for the strand length.

uint8_t max_bright = 128;                                     // Overall brightness definition. It can be changed on the fly.

struct CRGB leds[MAX_LEDS];                                   // Initialize our LED array. We'll be using less in operation.

CRGBPalette16 currentPalette;                                 // Use palettes instead of direct CHSV or CRGB assignments.
CRGBPalette16 targetPalette;                                  // Also support smooth palette transitioning.

TBlendType currentBlending = LINEARBLEND;                     // NOBLEND or LINEARBLEND for palettes

extern const TProgmemRGBGradientPalettePtr GradientPalettes[]; // These are for the fixed palettes in gradient_palettes.h
extern const uint8_t GradientPaletteCount;                     // Total number of fixed palettes to display.
uint8_t currentPaletteNumber = 0;                              // Current palette number from the 'playlist' of color palettes
uint8_t currentPatternIndex = 0;                               // Index number of which pattern is current


// EEPROM location definitions.
#define ISINIT    0                                           // EEPROM location used to verify that this Arduino has been initialized.
#define STARTMODE 1                                           // EEPROM location for the starting mode.
#define STRANDLEN 2                                           // EEPROM location for the actual Length of the strand, which is < MAX_LEDS
#define STRANDEL  3                                           // EEPROM location for the mesh delay value.

#define STARTPAL  6                                           // EEPROM location of current palette. If !0, then rotation = 0.
#define GLIT      7                                           // EEPROM location for glitter.
#define BRIT      8                                           // EEPROM location is brightness.
#define SPED      9                                           // EEPROM location of speed.
#define DIRN      10                                          // EEPROM location of direction.
#define LHUE      11                                          // EEPROM location of lamp hue.
#define LSAT      12                                          // EEPROM location of lamp saturation.
#define DEMO      13                                          // EEPROM location of demo mode toggle.

#define SQU       20                                           // EEPROM location of squelch.
#define MXV       21                                           // EEPROM location of maxVol.


#define INITVAL   0x51                                        // If this is the value in ISINIT, then the Arduino has been initialized. Change to completely reset your Arduino.


#define INITBRIT 255                                          // Initial max_bright.
#define INITDEL  0                                            // Starting mesh delay value of the strand in milliseconds.
#define INITDIRN 1                                            // Initial thisdir value.
#define INITGLIT 0                                            // Glitter is off by default.
#define INITLEN  40                                           // Start length is 20 LED's.
#define INITMODE 0                                            // Startmode is 0, which is black.
#define INITPAL  0                                            // Starting palette number.
#define INITSPED 0                                            // Initial thisdelay value.
#define INITLHUE 0                                            // Initial lamphue value.
#define INITLSAT 1                                            // Initial lampsat value.
#define INITDEMO 1                                            // Initial demo mode value.

#define INITMAX  9                                            // Starting maxVol value.
#define INITSQU  7                                            // Starting squelch value.


const uint32_t STRANDID = IR_C1;                              // This is the ID button of the strand and should be unique for each strand in a series (if you want them to be different).
bool strandActive = 0;                                        // Used for configuration changes only. 0=inactive, 1=active. Must be activated by button press of B1, followed by C1 (or the appropriate STRANDID button).
bool strandFlag = 0;                                          // Flag to let us know if we're changing the active strand.

uint16_t meshdelay;                                           // Timer for the notasound. Works with INITDEL.

uint8_t ledMode = 0;                                          // Starting mode is typically 0. Change INITMODE if you want a different starting mode.
uint8_t demorun = 1;                                          // 0 = regular mode, 1 = demo mode, 2 = shuffle mode.
uint8_t maxMode = 16;                                         // Maximum mode number.
uint16_t demotime = 10;                                       // Set the length of the demo timer.


// Generic/shared routine variables ----------------------------------------------------------------------
uint16_t loops = 0;                                           // Our loops per second counter for showfps().
uint8_t lamphue = 0;                                          // Hue value of lamp mode.
uint8_t lampsat = 1;                                          // Saturation value of lamp mode.


// Reko Merio's global variables
const int maxBeats = 10;                                      // Min is 2 and value has to be divisible by two.
const int maxBubbles = 10; //NUM_LEDS / 3;                     // Decrease if there is too much action going on.
const int maxTrails = 5;                                      // Maximum number of trails.

// IR changeable variables
uint8_t palchg = 3;                                           // 0=no change, 1=similar, 2=random
uint8_t  thisdelay;                                           // Standard delay is initialized by EEPROM.
uint8_t    glitter;                                           // Glitter flag is initialized by EEPROM.
int8_t     thisdir;                                           // Standard direction is either -1 or 1. Used as a multiplier rather than boolean and is initialized by EEPROM.



// Display functions -----------------------------------------------------------------------

// Support functions
#include "getsample.h"                                        // New sound reactive routines.
#include "structs.h"                                          // Reko Merio's structures.
#include "support.h"                                          // Support routines, such as showfps, glitter and routines to move up/down strand.
#include "gradient_palettes.h"                                // Using fixed gradient palettes rather than random ones.

//Reko Merio's global structure definitions
Bubble bubble[maxBubbles];
Bubble trail[maxTrails];


// Non sound reactive routine
#include "noisepal.h"

// Main sound reactive routines

#include "besin.h"          // sampleAgc  - Center to edges with black
#include "fillnoise.h"      // sampleAvg & sampleAgc  - Center to edges with base color and twinkle
#include "fire.h"           // sampleAvg  - Start to end noise based fire
#include "firewide.h"       // sampleAvg  - Center to edges
#include "jugglep.h"        // sampleAgc  - Long line of sinewaves
#include "matrix.h"         // sample     - Start to end with twinkles
#include "myvumeter.h"      // sampleAvg  - My own vu meter
#include "noisewide.h"      // sampleAvg  - Center to edges
#include "onesine.h"        // sampleAvg  - Long line of shortlines
#include "pixel.h"          // sampleAgc  - Long line of colours
#include "plasma.h"         // sampleAgc  - Long line of short lines
#include "rainbowpeak.h"    // samplepeak - Long line of short lines with twinkles
#include "ripple.h"         // samplepeak - Juggle with twinkles
#include "sinephase.h"      // sampleAgc  - Changing phases of sine waves

// Reko Merio display routines
#include "bubbles.h"        // samplePeak - Bubbles
#include "trails.h"         // samplePeak - Trails


/*------------------------------------------------------------------------------------------
--------------------------------------- Start of code --------------------------------------
------------------------------------------------------------------------------------------*/

void setup() {

  Serial.begin(115200);                                                           // Setup serial baud rate
  delay(1000);                                                                    // Slow startup so we can re-upload in the case of errors.

  Serial.println(F(" ")); Serial.println(F("---SETTING UP notasound---"));

  analogReference(EXTERNAL);                                                      // Comment out this line for 3.3V Arduino's, ie. Flora, etc or if powering microphone with 5V.
  
  if (!IRLremote.begin(pinIR))
    Serial.println(F("You did not choose a valid pin."));
  
//    LEDS.addLeds<LED_TYPE, LED_DT, LED_CK, COLOR_ORDER >(leds, MAX_LEDS);       // APA102 or WS2801 4 pin definition.
  LEDS.addLeds<LED_TYPE, LED_DT, COLOR_ORDER >(leds, MAX_LEDS);                   // WS2812 3 pin definition.
  
  FastLED.setMaxPowerInVoltsAndMilliamps(5, 500);                                   // 5V, 500mA maximum power draw.

  #if defined(_ESP8266)
  EEPROM.begin(32);                                                               // Not used by the UNO/Nano, but critical for ESP8266.
  #endif


  if (EEPROM.read(ISINIT) != INITVAL) {                                           // Check to see if Arduino has been initialized, and if not, do so. A cold reset as it were.
    EEPROM.write(ISINIT, INITVAL);                                                // Initialize the starting value (so we know it's initialized) to INITVAL.

    EEPROM.write(BRIT, INITBRIT);                                                 // Initialize the brightness value.
    EEPROM.write(STRANDEL, INITDEL);                                              // Initialize the notamesh delay to 0.
    EEPROM.write(DIRN, INITDIRN);                                                 // Initial thisdir value.
    EEPROM.write(GLIT, INITGLIT);                                                 // Initialize glitter value.
    EEPROM.write(STRANDLEN, INITLEN);                                             // Initialize the starting length to 20 LED's.
    EEPROM.write(STARTMODE, INITMODE);                                            // Initialize the starting mode to 0.
    EEPROM.write(STARTPAL, INITPAL);                                              // Initialize the palette to 0.
    EEPROM.write(SPED, INITSPED);                                                 // Initial thisdelay value.
    EEPROM.write(LHUE, INITLHUE);                                                 // Initial lamphue value.
    EEPROM.write(LSAT, INITLSAT);                                                 // Initial lampsat value.
    EEPROM.write(DEMO, INITDEMO);

    EEPROM.write(SQU, INITSQU);                                                   // Initialize the squelch value.
    EEPROM.write(MXV, INITMAX);                                                   // Initialize maxVol value.
    
    Serial.println(F("Cold reset."));
  }


  ledMode = EEPROM.read(STARTMODE);                                               // Location 0 is the starting mode.
  NUM_LEDS = EEPROM.read(STRANDLEN);                                              // Need to ensure NUM_LEDS < MAX_LEDS elsewhere.
  meshdelay = EEPROM.read(STRANDEL);                                              // This is our notamesh delay for cool delays across strands.
  glitter = EEPROM.read(GLIT);                                                    // notamesh glitter.
  max_bright = EEPROM.read(BRIT);                                                 // max_bright value.
  thisdelay = EEPROM.read(SPED);                                                  // thisdelay value.
  thisdir = EEPROM.read(DIRN);                                                    // thisdir value.
  lamphue = EEPROM.read(LHUE);                                                    // lamphue value.
  lampsat = EEPROM.read(LSAT);                                                    // lampsat value.
  demorun = EEPROM.read(DEMO);
 
  squelch = EEPROM.read(SQU);                                                     // notasound squelch is stored in EEPROM.
  maxVol = EEPROM.read(MXV);                                                      // notasound maxVol for peak detection in EEPROM.


  if (EEPROM.read(STARTPAL) != 0) {currentPaletteNumber = EEPROM.read(STARTPAL); palchg = 0;}

  Serial.println(F("---EEPROM COMPLETE---"));
 
  Serial.print(F("Initial mesh delay: ")); Serial.print(meshdelay*100); Serial.println(F("ms delay"));
  Serial.print(F("Initial strand length: ")); Serial.print(NUM_LEDS); Serial.println(F(" LEDs"));
  Serial.print(F("Strand ID: ")); Serial.println(STRANDID);
  Serial.print(F("Glitter: ")); Serial.println(glitter);
  Serial.print(F("Brightness: ")); Serial.println(max_bright);
  Serial.print(F("Delay: ")); Serial.println(thisdelay);
  Serial.print(F("Direction: ")); Serial.println(thisdir);
  Serial.print(F("Palette: ")); Serial.println(currentPaletteNumber);
  Serial.print(F("Demo: ")); Serial.println(demorun);

  Serial.print(F("Squelch: ")); Serial.println(squelch);
  Serial.print(F("Maxvol: ")); Serial.println(maxVol);

  LEDS.setBrightness(max_bright);                                                 // Set the generic maximum brightness value.
  
  currentPalette = CRGBPalette16(CRGB::Black);
  targetPalette = (GradientPalettes[0]);
  
  strobe_mode(ledMode, 1);                                                        // Initialize the first sequence.

  Serial.println(F("---SETUP COMPLETE---")); Serial.println(F(" "));

} // setup()



//------------------MAIN LOOP---------------------------------------------------------------
void loop() {

  getSample();                                                                // Sample sound, measure averages and detect peak.
  agcAvg();
  
  getirl();                                                                   // Read a command from the IR LED and process command as required.
  
  demo_check();                                                               // If we're in demo mode, check the timer to see if we need to increase the strobe_mode value.

  EVERY_N_MILLISECONDS(50) {                                                  // Smooth palette transitioning runs continuously.
    uint8_t maxChanges = 24; 
      nblendPaletteTowardPalette(currentPalette, targetPalette, maxChanges);  
  }

  EVERY_N_SECONDS(5) {                                                        // If selected, change the target palette to a random one every 5 seconds.
    if (palchg==3) {
      currentPaletteNumber = addmod8(currentPaletteNumber, 1, GradientPaletteCount);
    }
    targetPalette = GradientPalettes[currentPaletteNumber];                  // We're just ensuring that the targetPalette WILL be assigned.
  }

  EVERY_N_MILLIS_I(thistimer, thisdelay) {                                    // Sets the original delay time.
    thistimer.setPeriod(thisdelay);                                           // This is how you update the delay value on the fly.
    strobe_mode(ledMode, 0);                                                  // Strobe to display the current sequence, but don't initialize the variables, so mc=0;
  }

  if(glitter) addGlitter(sampleAvg/2);                                        // If the glitter flag is set, let's add some.

  if (!IRLremote.receiving()) {
    FastLED.show();                                                           // Power managed display of LED's.
  }
  
} // loop()



//-------------------OTHER ROUTINES----------------------------------------------------------
void strobe_mode(uint8_t newMode, bool mc){                   // mc stands for 'Mode Change', where mc = 0 is to display the routine, while mc = 1 is to initialize and then display the routine.

  if(mc) {
    fill_solid(leds,NUM_LEDS,CRGB(0,0,0));                    // Clean up the array for the first time through. Don't show display though, so you may have a smooth transition.
    Serial.print(F("Mode: ")); 
    Serial.println(newMode);
  }

  if (!strandActive) {                                          // Stops the display sequence if we're updating the EEPROM in ACTIVE mode.
    switch (newMode) {                                          // If first time through a new mode, then initialize the variables for a given display, otherwise, just call the routine.
      case   0: if(mc) {thisdelay=20;} noisepal(); break;       // Change mode 0 to a generic non-reactive noise routine.
      case   1: if(mc) {thisdelay=20;} ripple(); break;         // samplepeak - Juggle with twinkles
      case   2: if(mc) {thisdelay=40;} fillnoise(); break;      // sampleAvg  - Center to edges with base color and twinkle
      case   3: if(mc) {thisdelay=40;} bubbles(); break;        // samplepeak - Bubbles
      case   4: if(mc) {thisdelay= 0;} pixel(); break;          // sample     - Long line of colours
      case   5: if(mc) {thisdelay=30;} onesine(); break;        // sampleAvg  - Long line of shortlines
      case   6: if(mc) {thisdelay=10;} rainbowpeak(); break;    // samplepeak - Long line of short lines with twinkles
      case   7: if(mc) {thisdelay=10;} noisewide(); break;      // sampleAvg  - Center to edges
      case   8: if(mc) {thisdelay=30;} myvumeter(); break;      // sampleAvg  - My own vu meter
      case   9: if(mc) {thisdelay=10;} jugglep(); break;        // sampleAvg  - Long line of sinewaves
      case  10: if(mc) {thisdelay=10;} firewide(); break;       // sampleAvg  - Center to edges
      case  11: if(mc) {thisdelay=40;} trails(); break;         // samplepeak - Trails
      case  12: if(mc) {thisdelay=20;} plasma(); break;         // sampleAvg  - Long line of short lines
      case  13: if(mc) {thisdelay=30;} besin(); break;          // sampleAvg  - Center to edges with black
      case  14: if(mc) {thisdelay=40;} matrix(); break;         // sampleAgc  - Start to end with twinkles
      case  15: if(mc) {thisdelay= 0;} fire(); break;           // sampleAvg  - Start to end noise based fire
      case  16: if(mc) {thisdelay=10;} sinephase(); break;      // sampleAvg  - Changing phases of sine waves
      default: break;
    } // switch newMode
  } // !strandActive

} // strobe_mode()



void demo_check(){                                                // Are we in demo mode or not, and if so, change the routine every 'demotime' seconds.
  
  if(demorun) {                                                   // Is the demo flag set? If so, let's cycle through them.
    uint16_t secondHand = (millis() / 1000) % (maxMode*demotime+1);  // Adjust for total time of the loop, based on total number of available modes.

    static uint16_t lastSecond = 99;                               // Static variable, means it's only defined once. This is our 'debounce' variable.
    if (lastSecond != secondHand) {                               // Debounce to make sure we're not repeating an assignment.
      lastSecond = secondHand;
        if(secondHand%demotime==0) {                              // Every 10 seconds.
          if(demorun == 2) ledMode = random8(0,maxMode); else {   // Shuffle mode, which is not used.
            ledMode = secondHand/demotime;
          }
          meshwait();
          strobe_mode(ledMode,1);                                 // Does NOT reset to 0.
      } // if secondHand
    } // if lastSecond
  } // if demorun
  
} // demo_check()


//----------------- IR Receiver and Button Command Processing ---------------------------------------------

void getirl() {                                                   // This is the IR function that gets the value and selects/performs a command.
  
  if (IRLremote.available()) {
  
    auto irdata = IRLremote.read();           // Get the new data from the remote.
    IRAddress = irdata.address;             // Do this if we want to swap out values.
    IRCommand = irdata.command;             // Do this if we want to swap out values.

//    if(strandFlag == 1) set_strand();       // Set the strand length
    
//    Serial.print(F("Address: "));           // Print the protocol data. Note that there's also 65535, which we don't use.
//    Serial.println(irdata.address);
//    Serial.print(F("Command: "));
//    Serial.println(irdata.command);
//    Serial.println();

    if (IRAddress == IR_ADD) {     
      switch(IRCommand) {

        case IR_A1:  max_bright=min(max_bright*2+1,255); EEPROM.write(BRIT, max_bright); Serial.print(F("Bright: ")); Serial.println(max_bright); LEDS.setBrightness(max_bright); break;  //a1 - Increase max bright
        case IR_A2:  max_bright=max(max_bright/2,0); EEPROM.write(BRIT, max_bright); Serial.print(F("Bright: ")); Serial.println(max_bright); LEDS.setBrightness(max_bright); break;    //a2 - Decrease max bright
        case IR_A3:  fill_solid(leds,NUM_LEDS,CRGB(0,0,0)); FastLED.show(); Serial.println(F("Rebooting . . ")); FastLED.delay(100); bootme(); break;                                   //a3 - Change to mode 0, display and reboot
        case IR_A4:  demorun = 1; EEPROM.write(DEMO, demorun); Serial.println(F("Demo mode")); break;                                                                                   //a4 - Enable demo mode
  
        case IR_B1:  Serial.println(F("Activate request")); set_strand(); break;                                                                            //b1 - Set Strand Active or Inactive for EEPROM programming.
        case IR_B2:  if (strandActive==1) set_strandlen(); break;                                                                                           //b2 - Decrease # of LED's and write to EEPROM
        case IR_B3:  if (strandActive==1) set_strandlen(); break;                                                                                           //b3 - Increase # of LED's and write to EEPROM
//        case IR_B4:  if (strandActive==1) set_meshdel(); break;                      //b4 - Shorter mesh delay by 100ms

//        case IR_C1:  if (strandActive==1) set_meshdel(); break;                                                                                                         //c1 - Longer mesh delay by 100ms
        case IR_C2:  thisdelay++; EEPROM.write(SPED, thisdelay); Serial.print(F("Delay: ")); Serial.println(thisdelay); break;                                                    //c2 - Slow down the sequence as much as you want.
        case IR_C3:  thisdelay--; if(thisdelay >30000) thisdelay = 0; EEPROM.write(SPED, thisdelay); Serial.print(F("Delay: ")); Serial.println(thisdelay); break;                //c3 - Speed up the sequence, but don't go too far.
        case IR_C4:  thisdir = thisdir*-1; EEPROM.write(DIRN, thisdir); Serial.print(F("thisdir = ")); Serial.println(thisdir);  break;                                           //c4 - Change the direction of the LEDs.

        case IR_D1:  maxVol--; if(maxVol <0) maxVol = 0; EEPROM.write(MXV, maxVol);Serial.print(F("Maxvol: ")); Serial.println(maxVol);  break;   //d1 -  Reduce maxVol for more sensitive peak detection
        case IR_D2:  maxVol++; EEPROM.write(MXV, maxVol);Serial.print(F("Maxvol: ")); Serial.println(maxVol); break;                              //d2 -  Increase maxVol for less sensitive peak detection
        case IR_D3:  squelch--; if(squelch <0) squelch = 0; EEPROM.write(SQU, squelch);Serial.print(F("Squelch: ")); Serial.println(squelch);  break;  //d3 -  Reduce squelch value
        case IR_D4:  squelch++; EEPROM.write(SQU, squelch); Serial.print(F("Squelch: ")); Serial.println(squelch); break;                    //d4 - Increase squelch value.
  
//        case IR_D1:  glitter = !glitter; EEPROM.write(GLIT, glitter); Serial.print(F("Glitter is: ")); Serial.println(glitter);   break;    //d1 - Toggle glitter.
//        case IR_D2:  demorun = 0; ledMode=(ledMode-1); if (ledMode==255) ledMode=maxMode; meshwait(); strobe_mode(ledMode,1); break;        //d2 - Stop demo and display previous mode.
//        case IR_D3:  demorun = 0; ledMode=(ledMode+1)%(maxMode+1); meshwait(); strobe_mode(ledMode,1); break;                               //d3 - stop demo and display next mode.
//        case IR_D4:  EEPROM.write(STARTMODE,ledMode); Serial.print(F("Writing startup mode: ")); Serial.println(ledMode);  break;           //d4 - Save current mode as startup mode.

        case IR_E1:  palchg = 0; Serial.print(F("Stop and select current Palette: ")); EEPROM.write(STARTPAL, currentPaletteNumber); Serial.println(currentPaletteNumber); break;                                                                                               //f1 - Stop and select current Palette
        case IR_E2:  palchg = 1; Serial.print(F("Stop and select previous Palette: ")); currentPaletteNumber -= 1; if(currentPaletteNumber == 255) currentPaletteNumber = GradientPaletteCount; EEPROM.write(STARTPAL, currentPaletteNumber); Serial.println(currentPaletteNumber); currentPalette = (GradientPalettes[currentPaletteNumber]); break;    //f2 - Stop and select previous Palette
        case IR_E3:  palchg = 2; Serial.print(F("Stop and select next Palette: ")); currentPaletteNumber = addmod8( currentPaletteNumber, 1, GradientPaletteCount);  EEPROM.write(STARTPAL, currentPaletteNumber); Serial.println(currentPaletteNumber);  currentPalette = (GradientPalettes[currentPaletteNumber]); break;             //f3 - Stop and select next Palette
        case IR_E4:  palchg = 3; Serial.print(F("Continuous palette change: "));  Serial.println(currentPaletteNumber); EEPROM.write(STARTPAL, 0); break;                                                                                                    //f4 - Continuous palette change

//        case IR_F1:  palchg = 4; Serial.println(F("Hue/Palette based palette change.")); EEPROM.write(STARTPAL, 4); break;                                              //d1 - Hue/saturation palette.  
        case IR_F1:  break;                                                                                                                                             //f1 - Hue/saturation palette.  
        case IR_F2:  demorun = 0;  EEPROM.write(DEMO, demorun); ledMode=(ledMode-1); if (ledMode==255) ledMode=maxMode; meshwait(); strobe_mode(ledMode,1); break;      //f2 - strobe_mode(ledMode--);
        case IR_F3:  demorun = 0;  EEPROM.write(DEMO, demorun); ledMode=(ledMode+1)%(maxMode+1); meshwait(); strobe_mode(ledMode,1); break;                             //f3 - strobe_mode(ledMode++);
        case IR_F4:  EEPROM.write(STARTMODE,ledMode); Serial.print(F("Writing startup mode: ")); Serial.println(ledMode);  break;                                       //f4 - Save startup mode

        default:     break;                                     // We could do something by default.

      } // switch IRCommand
    } // if IR_ADD
  } // if IRLRemote

} // getirl()



void bootme() {                                                     // This is used to reset all the Arduinos so that their millis() counters are all in sync.

  if (millis() < 5000) {
    Serial.println(F("Factory Reset."));                            // If we reset within 5 seconds of startup, then it means factory reset.
    EEPROM.write(ISINIT, 0);
    delay(200);
  }

  #if defined(_ESP8266)    
  EEPROM.commit();                                                  // We need to actually write to the EEPROM on the ESP8266. Am not sprinkling this throughout the existing code, so here it sits.
  ESP.restart();
  #endif

  #if defined(_NANO)
  asm volatile("jmp 0");
  #endif
  
} // bootme()



void meshwait() {                                               // After we press a mode key, we need to wait a bit for the sequence to start.

//  Serial.print(F("Mesh delay: ")); Serial.print(meshdelay*100); Serial.println(F("ms delay."));
  FastLED.delay(meshdelay*100);                                 // Here's our notasound wait upon keypress. I'm so sorry there's a delay statement here. At least it's only used upon mode change keypress. Makes life a LOT simpler.

} // meshwait()



void set_strand() {                                             // Setting the active strand. This logic is not great, so we have to reboot when done.

  if (IRAddress == IR_ADD && IRCommand == IR_B1) {
    strandFlag = !strandFlag;
    if (strandFlag == false) {IRCommand = 255; strandActive = 0; Serial.println(F("INACTIVE"));}
  }

  if (strandFlag == true && IRAddress == IR_ADD && IRCommand == STRANDID) {
    strandActive = 1; Serial.println(F("ACTIVE"));
  }

} // set_strand()



void set_strandlen() {                                                  // Setting our strand length with button presses.

  Serial.println(F("Setting strand length."));
  
  if(strandActive == 1) {                                               // Only do this if the strand is active.
    demorun = 0;                                                        // First we disable the demo mode.
    fill_solid(leds,MAX_LEDS, CRGB::Black);                             // Let's make it black manually.
    
    if (IRCommand == IR_B2) {
      NUM_LEDS--; if (NUM_LEDS >255) NUM_LEDS=0;                        // Don't be stupid with our strand length selection.
    } else {
     NUM_LEDS++;  if (NUM_LEDS >= MAX_LEDS) NUM_LEDS=MAX_LEDS-1;        // Again, don't be stupid with our strand length selection.
    }
    fill_solid(leds,NUM_LEDS,CRGB(64, 64, 64));                         // Turn on the number of LEDs that we have selected as our length.
    EEPROM.write(STRANDLEN,NUM_LEDS);                                   // Write that value to EEPROM.
    Serial.print(F("Writing IR: ")); Serial.print(NUM_LEDS); Serial.println(F(" LEDs"));
  } // if strandActive
  
} // set_strandlen()



void set_meshdel() {                                                    // Setting our notasound delay for whatever strands are powered up.

  if(strandActive == 1) {                                               // Only do this if the strand is active.
    demorun = 0;                                                        // First we disable the demo mode.
    ledMode = 0;                                                        // And set to mode 0 (black).
    fill_solid(leds,MAX_LEDS, CRGB::Black);                             // Let's make it black manually.
    
    if (IRCommand == IR_E2) {                                           // Which button did we press (either E2 or E3).
      meshdelay = meshdelay - 1;
      if (meshdelay >10000) meshdelay = 0;                              // Again, don't be stupid with our buttons.
    } else {
     meshdelay = meshdelay + 1;                                         // Increase the delay as much as you want. . .
    } // if IRCommand
    
    fill_solid(leds,meshdelay,CRGB(64, 64, 64));                        // Turn on the number of LED's that we have selected (100ms is 1 LED)
    EEPROM.write(STRANDEL,meshdelay);                                   // Write out the delay to EEPROM.
    Serial.print(F("Writing IR: ")); Serial.print(meshdelay*100); Serial.println(F("ms delay."));
  } // if strandActive
  
} // set_meshdel()
