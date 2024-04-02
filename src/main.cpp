#include <Arduino.h>
#include <IRsend.h>
#include <RCSwitch.h>
#include <ESP8266WiFi.h>

#define IrLED D6
#define rSender D5
#define melderPin D7
#define alarmLED D4
#define empfangLED D3

// VentMessages
#define V_AN 0xC08
#define V_AUS 0xC20
#define V_OFF 0xC10
#define V_LOW 0xC43
#define V_MED 0xC04
#define V_HIGH 0xC01

// RC Messages
#define R_1AN 2394740224
#define R_1AUS 2176636416
#define R_2AN 2797393408
#define R_2AUS 2931611136

#define LichtAusZeit 300000

const uint16_t scanDelay = 100;
const uint16_t scanTimes = 5;

uint32_t LichtTimer = 0;
uint32_t alarmMillis = 0;

boolean LichtAn = false;
boolean alarmAktiv = false;
boolean alarmAktivAlt = false;
uint8_t SteckerTimer = 0;
boolean GarageZu = true;

IRsend irsend(IrLED); // Set the GPIO to be used to sending the message.
RCSwitch RC = RCSwitch();

void VentMessage(int code)
{
  irsend.sendSymphony(0xC00);
  irsend.sendSymphony(0xC7F);
  irsend.sendSymphony(code);
}

void Alarm()
{
  LichtAn = true;
  LichtTimer = millis();
  VentMessage(V_AN);
  RC.send(R_1AN, 32);
  RC.send(R_2AN, 32);
  Serial.print("ALARM\n");
}

void Quittiert()
{
  Serial.print("ENDE\n");
}

void LichtAus()
{
  LichtTimer = 0;
  LichtAn = false;
  RC.send(R_1AUS, 32);
  RC.send(R_2AUS, 32);
  VentMessage(V_AUS);
}

void manageSerial()
{
  if (Serial.available())
  {
    String rec = Serial.readStringUntil('\n');
    digitalWrite(empfangLED, true);
    if (rec == "OFF")
      VentMessage(V_OFF);
    if (rec == "LOW")
      VentMessage(V_LOW);
    if (rec == "MED")
      VentMessage(V_MED);
    if (rec == "HI")
      VentMessage(V_HIGH);
    if (rec == "ON")
      VentMessage(V_AN);
    if (rec == "LIGHTOFF")
      LichtAus();
    if (rec == "TEST")
      Alarm();
    if (rec == "GarageOffen")
      GarageZu = false;
    if (rec == "GarageZu")
      GarageZu = true;
  }
  digitalWrite(empfangLED, false);
  Serial.flush();
}

// Hier beginnt das eigentliche Programm

void setup()
{
  // WiFi nicht benötigt -> Aus
  WiFi.persistent(false);
  WiFi.disconnect();
  WiFi.mode(WIFI_OFF);
  WiFi.forceSleepBegin();

  // RC einrichten
  RC.enableTransmit(rSender);
  RC.setProtocol(2);
  RC.setPulseLength(714);
  RC.setRepeatTransmit(3);

  irsend.begin();
  pinMode(melderPin, INPUT_PULLUP);
  pinMode(alarmLED, OUTPUT);
  pinMode(empfangLED, OUTPUT);

  // Serielle Schnittstelle einrichten
  Serial.begin(115200, SERIAL_8E1);

  alarmAktivAlt = alarmAktiv;
}

void checkAlarm()
{
  alarmAktiv = digitalRead(melderPin) == LOW;
}

void loop()
{
  if (millis() - LichtTimer >= LichtAusZeit && LichtAn)
  {
    LichtAus();
  }

  for (int i = 0; i < scanTimes; i++)
  {
    delay(scanDelay);
    checkAlarm();
    if ((alarmAktiv == alarmAktivAlt))
    {
      break;
    }
    if (i < scanTimes - 1)
    {
      if (alarmAktiv)
        Alarm();
      else
        Quittiert();

      alarmAktivAlt = alarmAktiv;
    }
  }

  /*if ((alarmAktiv ^ alarmAktivAlt))
  {
    delay(scanDelay);
    checkAlarm();
    if ((alarmAktiv ^ alarmAktivAlt))
    {
      if (alarmAktiv)
        Alarm();
      else
        Quittiert();

      alarmAktivAlt = alarmAktiv;
    }
  }*/

  digitalWrite(alarmLED, !GarageZu);
  manageSerial();
}
