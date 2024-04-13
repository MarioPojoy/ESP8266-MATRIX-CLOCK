#include <Arduino.h>
#include <esp8266WiFi.h>
#include <time.h>
#include <MD_Parola.h>
#include <SPI.h>
#include "Font_Data.h"
#include "credentials.h"

#define HARDWARE_TYPE MD_MAX72XX::GENERIC_HW
#define MAX_DEVICES 4

#define CLK_PIN   D5
#define DATA_PIN  D7
#define CS_PIN    D8

MD_Parola led_matrix = MD_Parola(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

#define SPEED_TIME  75
#define PAUSE_TIME  0
#define MAX_MESG  20

#define ARGENTINA -14400   // Numero de segundos de zona horaria Argentina (UTC-04:00)
#define GUATEMALA -21600   // Numero de segundos de zona horaria Guatemala City (UTC-06:00)
#define DST 0

uint16_t  h, m, s;
uint8_t dow;
int  day;
uint8_t month;
String  year;
char szTime[9];
char szsecond[4];
char szMesg[MAX_MESG+1] = "";


void getsec(char *psz)
{
  sprintf(psz, "%02d", s);
}

void getTime(char *psz, bool f = true)
{
  time_t now = time(nullptr);
  struct tm* p_tm = localtime(&now);
      h = p_tm->tm_hour;
      m = p_tm->tm_min;
      s = p_tm->tm_sec;
  sprintf(psz, "%02d%c%02d", h, (f ? ':' : ' '), m);
  Serial.println(psz);
}

void getTimentp()
{
  configTime(ARGENTINA, DST, "pool.ntp.org","time.nist.gov");
  while(!time(nullptr)){
        delay(500);
        Serial.print(".");
  }
  Serial.print("Time Update");
}

void setup(void)
{
  Serial.begin(115200);
  delay(10);

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  getTimentp();

  led_matrix.begin(3);
  led_matrix.setInvert(false);

  led_matrix.setZone(0, 0, 0);
  led_matrix.setZone(1, 1, 3);
  led_matrix.setFont(0, numeric7Seg);
  led_matrix.setFont(1, numeric7Se);
  led_matrix.displayZoneText(0, szsecond, PA_LEFT, SPEED_TIME, 0, PA_PRINT, PA_NO_EFFECT);
  led_matrix.displayZoneText(1, szTime, PA_CENTER, SPEED_TIME, PAUSE_TIME, PA_PRINT, PA_NO_EFFECT);

  getTime(szTime);
}

void loop(void)
{
  static uint32_t lastTime = 0;
  static bool flasher = false;

  led_matrix.displayAnimate();

  if (millis() - lastTime >= 1000)
  {
    lastTime = millis();
    getsec(szsecond);
    getTime(szTime, flasher);
    flasher = !flasher;

    led_matrix.displayReset(0);
    led_matrix.displayReset(1);
  }
}