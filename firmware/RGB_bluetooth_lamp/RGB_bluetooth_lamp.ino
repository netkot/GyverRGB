#include <FastLED.h>
#include <avr/eeprom.h>

// debug
#define DEBUG_MODE 0

// How many leds in your strip?
#define NUM_LEDS 8
#define DATA_PIN 3
#define PIN_W 11
#define SMOOTH_K 0.25       // коэффициент плавности огня
#define SMOOTH_KD 0.5       // коэффициент плавности ручного огня


// EEPROM
#define EEPR_TIME 10000
#define EEPR_RESET 1023
#define RESET_VAL 10
#define PRESET_ADDR 200


// Define the array of leds
CRGB leds[NUM_LEDS];

// BT
#define BT_RX 7
#define BT_TX 8
#define USE_BT 1

// Переменные
// цвета для режима кнопки 1
const uint32_t HEXcolors[] = {
  0xf6e400,
  0xffc50b,
  0xff8d10,
  0xff5c0c,
  0xff0000,
  0xc9007e,
  0x722d9a,
  0x3643a9,
  0x1573c8,
  0x009bc4,
  0x00f502,
  0x94d50e,
};

int presetSettings[6];    // текущие настройки пресета
const byte setAmount[] = {5, 4, 3, 3, 3, 4, 4, 5, 5, 5, 3};         // количество настроек в режиме
const byte startFrom[] = {0, 4, 7, 9, 11, 13, 16, 19, 23, 27, 31}; // начало индекса в списке лейблов
const char *labels[] = {      // список названий лейблов
  "BR", "R", "G", "B",
  "HUE", "SAT", "VAL",
  "BR", "COL",
  "BR", "COL",
  "BR", "TEMP",
  "BR", "SPD", "STP",
  "BR", "SPD", "MIN",
  "BR", "COL", "SPD", "MIN",
  "HUE", "SAT", "VAL", "SPD",
  "HUE", "SAT", "VAL", "SPD",
  "BR", "SPD",
};

const char *modeNames[] = {    // список названий режимов
  "RGB", "HSV", "Color", "ColorSet", "Kelvin", "ColorW",
  "Fire", "FireM", "Strobe", "StrobeR", "Police",
};
const int maxVals[] = {
  255, 255, 255, 255,
  255, 255, 255,
  255, 1530,
  255, 15,
  255, 10000,
  255, 1000, 500,
  255, 1000, 255,
  255, 255, 1000, 255,
  255, 255, 255, 1000,
  255, 255, 255, 1000,
  255, 1000,
};
const char *colorNames[] = {
  "WHITE",
  "SILVER",
  "GRAY",
  "BLACK",
  "RED",
  "MAROON",
  "YELLOW",
  "OLIVE",
  "LIME",
  "GREEN",
  "AQUA",
  "TEAL",
  "BLUE",
  "NAVY",
  "PINK",
  "PURPLE",
};
const uint32_t colorVals[] = {
  0xFFFFFF,   //   "WHITE",
  0xC0C0C0,   //   "SILVER",
  0x808080,   //   "GRAY",
  0x000000,   //   "BLACK",
  0xFF0000,   //   "RED",
  0x800000,   //   "MAROON",
  0xFFFF00,   //   "YELLOW",
  0x808000,   //   "OLIVE",
  0x00FF00,   //   "LIME",
  0x008000,   //   "GREEN",
  0x00FFFF,   //   "AQUA",
  0x008080,   //   "TEAL",
  0x0000FF,   //   "BLUE",
  0x000080,   //   "NAVY",
  0xFF00FF,   //   "PINK",
  0x800080,   //   "PURPLE",
};

uint32_t eeprTimer;
uint32_t effTimer;
uint16_t effPeriod;
boolean eeprFlag;
int16_t presetNum;   // номер пресета
int16_t modeNum = 0;     // номер режима
int8_t navPos = 0;      // позиция навигации
int invSet = 0;
boolean invFlag;
boolean changeFlag;
boolean settingsChanged;
boolean ONflag = true;
uint32_t IRdata;
boolean toggleFlag;
int voltage;
byte batPerc;
uint32_t voltmeterTimer;
boolean backlState = true;
uint32_t backlTimer;
boolean btnControl = false;
int8_t btnColor;
boolean btnColorChanged = true;
int thisBright = 255;
boolean firstStartBtn = false;


#include <SoftwareSerial.h>
SoftwareSerial btSerial(BT_TX, BT_RX); // RX, TX








void setup()
{
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);  // GRB ordering is assumed
  FastLED.setBrightness(1);

  btSerial.begin(9600);
  if (DEBUG_MODE)  Serial.begin(9600);


  // работаем с EEPROM
  if (eeprom_read_byte((uint8_t*)EEPR_RESET) != RESET_VAL) {    // первый запуск
    eeprom_write_byte((uint8_t*)EEPR_RESET, RESET_VAL);         // запомнили
    for (byte i = 0; i < 10; i++) eeprom_write_byte(i, 0);
    for (byte i = 10; i < 200; i += 2) {
      eeprom_write_word(i, 0);
      if (i % 10 == 0) eeprom_write_word(i, 255);
    }
    eeprom_write_byte(PRESET_ADDR, 0);
    eeprom_write_byte(201, 0);
    eeprom_write_word(202, 100);
    writeSettings();
  }

  presetNum = eeprom_read_byte((uint8_t*)PRESET_ADDR);
  btnColor = eeprom_read_byte((uint8_t*)201);
  thisBright = eeprom_read_word((uint16_t*)202);
  readSettings();
  delay(100);
}

void loop() {
  controlTick();    // отработка управления
  effectsTick();
  if (USE_BT) bluetoothTick();
}
