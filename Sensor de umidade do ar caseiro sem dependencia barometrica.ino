/*-------------------------------------------------------------------------------
* Projeto: Sensor de Umidade do Ar Caseiro com Arduino
* Saiba mais: https://elcereza.com/sensor-de-umidade-do-ar-caseiro/
* Autor: Gustavo Cereza
---------------------------------------------------------------------------------*/
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Thermistor.h>

#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1

#define ONE_WIRE_BUS 2
#define TEMPERATURE_PRECISION 11

#define display_enable !false
#define ds18b20_enable !false 

OneWire oneWire(ONE_WIRE_BUS);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

DallasTemperature sensors(&oneWire);
DeviceAddress dts, dtm;

Thermistor temps(1);                                                               // Entrada A1 do termistor seco
Thermistor tempm(2);                                                               // Entrada A2 do termistor umedecido


const int table[16][43] PROGMEM = {{3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 
                                    16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30,
                                    31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45},
                                  {92, 83, 93, 94, 94, 94, 94, 94, 94, 94, 95, 95, 95,
                                    95, 95, 95, 95, 96, 96, 96, 96, 96, 96, 96, 96, 96, 96, 96,
                                    96, 96, 96, 96, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97},
                                  {84, 85, 86, 87, 87, 88, 88, 89, 89, 89, 89, 89, 89,
                                    90, 90, 90, 90, 90, 91, 91, 91, 91, 92, 92, 92, 92, 93, 93,
                                    93, 93, 93, 93, 93, 93, 93, 93, 94, 94, 94, 94, 94, 94, 94},
                                  {76, 77, 78, 80, 81, 82, 82, 83, 83, 84, 84, 85, 85,
                                    86, 86, 87, 87, 87, 88, 88, 88, 88, 89, 90, 90, 90, 90, 90,
                                    90, 90, 90, 90, 90, 90, 90, 91, 91, 91, 91, 91, 91, 91, 91},
                                  {69, 70, 72, 73, 74, 75, 76, 76, 77, 78, 78, 79, 79,
                                    80, 80, 81, 82, 82, 83, 83, 84, 84, 85, 85, 86, 86, 86, 86,
                                    86, 86, 86, 87, 87, 87, 87, 88, 88, 88, 88, 88, 89, 89, 89},
                                  {62, 63, 65, 66, 67, 69, 70, 71, 72, 73, 74, 75, 76,
                                    77, 77, 78, 78, 79, 79, 80, 80, 81, 81, 81, 82, 82, 82, 82,
                                    82, 83, 83, 83, 84, 84, 84, 85, 85, 86, 86, 86, 87, 87, 87},
                                  {54, 56, 58, 60, 62, 64, 65, 66, 67, 68, 69, 70, 71,
                                    72, 72, 73, 74, 74, 75, 76, 77, 77, 78, 78, 79, 79, 79, 79,
                                    80, 80, 80, 80, 81, 81, 81, 82, 82, 82, 83, 83, 83, 84, 84},
                                  {47, 49, 51, 54, 54, 56, 59, 61, 66, 63, 64, 65, 66,
                                    67, 68, 69, 70, 71, 71, 72, 73, 74, 75, 75, 76, 76, 76, 76,
                                    77, 77, 77, 77, 78, 78, 78, 78, 79, 79, 79, 80, 80, 80, 80},
                                  {40, 43, 45, 47, 49, 51, 53, 56, 57, 58, 60, 61, 62,
                                    63, 64, 65, 66, 66, 67, 68, 69, 70, 71, 71, 72, 72, 73, 73,
                                    73, 73, 74, 74, 74, 75, 75, 75, 76, 76, 76, 77, 77, 77, 78},
                                  {32, 35, 38, 41, 43, 46, 48, 51, 52, 53, 55, 57, 58,
                                    59, 60, 61, 62, 63, 64, 64, 65, 66, 67, 67, 69, 69, 70, 70,
                                    70, 71, 71, 71, 72, 72, 73, 73, 74, 74, 75, 75, 76, 76, 76},
                                  {25, 29, 32, 35, 38, 40, 42, 45, 47, 48, 50, 52, 53,
                                    55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 64, 65, 66, 66, 66,
                                    67, 68, 68, 69, 69, 70, 70, 70, 71, 71, 71, 72, 72, 72, 72},
                                  {12, 16, 30, 23, 26, 29, 32, 35, 37, 38, 40, 48, 44,
                                    46, 48, 49, 51, 58, 53, 54, 54, 55, 56, 58, 59, 60, 61, 61,
                                    61, 62, 63, 63, 64, 64, 65, 66, 66, 66, 67, 67, 67, 68, 68},
                                  {0, 0, 0, 11, 15, 19, 22, 26, 28, 30, 32, 34, 36,
                                    38, 40, 42, 54, 45, 45, 47, 48, 49, 51, 52, 53, 54, 55, 55,
                                    56, 57, 57, 58, 59, 59, 60, 61, 61, 61, 62, 62, 62, 63, 63},
                                  {0, 0, 0, 0, 0, 0, 12, 17, 19, 21, 24, 26, 28,
                                    31, 36, 35, 37, 38, 39, 41, 42, 43, 45, 46, 47, 48, 49, 50,
                                    51, 52, 58, 62, 53, 54, 54, 55, 56, 56, 57, 57, 58, 58, 59},
                                  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 15, 18, 20,
                                    23, 25, 27, 29, 31, 32, 34, 36, 37, 39, 40, 41, 42, 43, 44,
                                    45, 46, 47, 48, 49, 50, 51, 51, 52, 52, 53, 53, 54, 54, 54},
                                  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 13,
                                    16, 18, 20, 22, 24, 26, 28, 30, 31, 36, 35, 36, 37, 38, 39,
                                    40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 51, 52, 52}};
            
const float column[] = {0, 0.5, 1, 1.5, 2, 2.5, 3, 3.5, 4, 4.5, 5, 6, 7, 8, 9, 10};

void setup() {
  Serial.begin(9600);

#if ds18b20_enable
  sensors.begin();
  sensors.getAddress(dts, 0);
  sensors.getAddress(dtm, 1);
  sensors.setResolution(dts, TEMPERATURE_PRECISION);
  sensors.setResolution(dtm, TEMPERATURE_PRECISION);
#endif
#ifdef display_enable
  while(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    delay(3000);
  }
  display.display();
  display.clearDisplay();
#endif
}

#ifdef ds18b20_enable
float getTemp(DeviceAddress deviceAddress){
  sensors.requestTemperatures();
  return sensors.getTempC(deviceAddress);
}
#endif

int pgm_table(int c, int l){
  return pgm_read_word(&table[c][l]);
}

void loop() {
#if ds18b20_enable
  float ts = getTemp(dts), 
        tm = getTemp(dtm);
#else
  float ts = temps.getTemp(), 
        tm = tempm.getTemp();
#endif

  int ur = 0;

  static int last_ur = 0;
  float dt = ts - tm;
  int line = 0, c = 0;
  if (ts >= 3 && ts <= 45) {
    if (dt > column[0] && dt <= column[15]) {
      for (line = 0; line < 41; ++line){
        if ((int)(ts) >= pgm_table(0, line) && (int)(ts) < pgm_table(0, line))
          break;
      }

      for (c = 1; c < 16; ++c) {
        if (c < 15 && dt >= column[c] && dt < column[c + 1])
          break;
        else if (c == 15)
          break;
      }
      ur = pgm_table(c, line);
    }
  }
  if (dt < column[1])
    ur = 100;
  else if (dt > column[15])
    ur = 0;
 
#ifdef display_enable
  display.clearDisplay();
  display.setTextSize(2); 
  display.setTextColor(WHITE);
  display.setCursor(10, 10);
  display.print(F("Ts: "));
  display.println(String(ts));
  display.setCursor(10, 35);
  display.print(F("Ur: "));
  display.println(String(ur) + "%");
  display.display();
#endif

  Serial.print("C : ");
  Serial.print(c);
  Serial.print(" | L : ");
  Serial.print(line);
  Serial.print(" | Ts : ");
  Serial.print(ts);
  Serial.print("*C | ");
  Serial.print("Tm : ");
  Serial.print(tm);
  Serial.print("*C | ");
  Serial.print("Ur : ");
  Serial.println(ur);
  Serial.println("---------------------------------------------------------");
  delay(1000);
}
