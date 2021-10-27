#define FASTLED_INTERNAL
#include <Arduino.h>
#include <M5Atom.h>
#include <IRremote.h>

#define M5_IR 12

void setup()
{
  Serial.begin(115200);

  IrSender.begin(M5_IR, false, 0); 

  M5.begin(true, false, true); //SerialEnable, I2CEnable, DisplayEnable
  delay(50);
  M5.dis.clear();
}

void loop()
{
  String in;

  static bool flashOnSend = false;
  
  if(M5.Btn.wasPressed())
  {
    flashOnSend = !flashOnSend;

    if(flashOnSend)
    {
      M5.dis.drawpix(0, CRGB(0,255,0));
    }
    else
    {
      M5.dis.drawpix(0, CRGB(255,0,0));
    }

    delay(50);
    M5.dis.clear();
  }
  
  while(Serial.available())
  {
    in = Serial.readStringUntil('\r');

    //Example
    //send nec 0x3EC14DB2
    char *action = strtok((char*)in.c_str(), " ");
    if (action != NULL)
    {
      if(strcasecmp(action, "send") == 0)
      {
        char *encoding = strtok(NULL, " ");
        if(encoding != NULL)
        {
          if(strcasecmp(encoding, "nec") == 0)
          {
            char *command = strtok(NULL, " ");
            if(command != NULL)
            {
              unsigned long commandUL;
              commandUL = strtoul(command, NULL, 16);
              IrSender.sendNEC(commandUL, 32);
              Serial.println("OK");

              if(flashOnSend)
              {
                M5.dis.drawpix(0, CRGB(255,255,255));
                delay(50);
                M5.dis.clear();
              }
            }
            else
            {
              Serial.println("ERROR: No command specified.");
            }
          }
          else
          {
            Serial.printf("ERROR: Unknown encoding '%s'.\r\n", encoding);
          }
        }
        else
        {
          Serial.println("ERROR: No encoding specified.");
        }
      }
      else if(strcasecmp(action, "message") == 0)
      {
        char *message = strtok(NULL, " ");
        if(message != NULL)
        {
          Serial.println("OK");
        }
        else
        {
          Serial.println("ERROR: No message specified.");
        }
      }
      else
      {
        Serial.printf("ERROR: Unknown action '%s'.\r\n", action);
      }
    }
    else
    {
      Serial.println("ERROR: Empty message.");
    }

    Serial.flush();
  }

  delay(50);
  M5.update();    //Read the press state of the key.
}
