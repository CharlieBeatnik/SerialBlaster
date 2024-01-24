#include <Arduino.h>
#include <M5StickC.h>
#include <IRremote.h>

bool userDebugEnabled = false;
unsigned int cursorYMax = 160;
#define M5_GROVEIR_RECIEVE_PIN 33
#define M5_GROVEIR_SEND_PIN 32

void setup()
{
  Serial.begin(115200);

  M5.begin();
  lcdBacklightEnable(userDebugEnabled);
  M5.Lcd.setTextWrap(true);
  lcdPrintln("SerialBlaster");
  lcdPrintln("Version 3.1");

  pinMode(M5_LED, OUTPUT);
  digitalWrite(M5_LED, LOW);
  digitalWrite(M5_LED, HIGH);

  pinMode(M5_BUTTON_HOME, INPUT);
  pinMode(M5_BUTTON_RST, INPUT);

  IrSender.begin(M5_GROVEIR_SEND_PIN, false, 0); 
}

size_t lcdPrintln(const char *string)
{
    lcdManageVerticalWrap();

    size_t len = M5.Lcd.print(string);
    len += M5.Lcd.println();
    return len;
}

size_t lcdPrintf(const char *format, ...)
{
    lcdManageVerticalWrap();

    char loc_buf[64];
    char * temp = loc_buf;
    va_list arg;
    va_list copy;
    va_start(arg, format);
    va_copy(copy, arg);
    size_t len = vsnprintf(NULL, 0, format, arg);
    va_end(copy);
    if(len >= sizeof(loc_buf)){
        temp = new char[len+1];
        if(temp == NULL) {
            return 0;
        }
    }
    len = vsnprintf(temp, len+1, format, arg);
    M5.Lcd.print(temp);
    va_end(arg);
    if(len >= sizeof(loc_buf)){
        delete[] temp;
    }
    return len;
}

void lcdManageVerticalWrap()
{
  int16_t cursorY = M5.Lcd.getCursorY();
  if(cursorY >= cursorYMax)
  {
    M5.Lcd.fillScreen(0x000000);
    M5.Lcd.setCursor(0, 0);
  }
}

void lcdBacklightEnable(bool enable)
{
  if(enable)
  {
    M5.Axp.SetLDO2(true);
    M5.Axp.SetLDO3(true);
  }
  else
  {
    M5.Axp.SetLDO2(false);
    M5.Axp.SetLDO3(false);
  }
}

void loop()
{
  String in;

  static int buttonHomePrevious = HIGH;
  int buttonHome = digitalRead(M5_BUTTON_HOME);

  static int buttonResetPrevious = HIGH;
  int buttonReset = digitalRead(M5_BUTTON_RST);

  if(buttonHomePrevious == HIGH && buttonHome == LOW)
  {
      userDebugEnabled = !userDebugEnabled;
      lcdBacklightEnable(userDebugEnabled);
  }
  buttonHomePrevious = buttonHome;

  if(buttonResetPrevious == HIGH && buttonReset == LOW)
  {
    ESP.restart();
  }
  buttonResetPrevious = buttonReset;
  
  while(Serial.available())
  {
    in = Serial.readStringUntil('\r');

    //Example
    //send nec 0x3EC14DB2

    char *action = strtok((char*)in.c_str(), " ");
    if (action != NULL)
    {
      lcdPrintf("%s\n", action);
      if(strcasecmp(action, "send") == 0)
      {
        char *encoding = strtok(NULL, " ");
        if(encoding != NULL)
        {
          lcdPrintf("%s\n", encoding);
          if(strcasecmp(encoding, "nec") == 0)
          {
            char *command = strtok(NULL, " ");
            if(command != NULL)
            {
              int numRepeats = 0;
              char *repeat = strtok(NULL, " ");
              if(repeat != NULL)
              {
                numRepeats = atoi(repeat);
              }
              
              lcdPrintf("%s\n", command);
              unsigned long commandUL;
              commandUL = strtoul(command, NULL, 16);
              //commandUL = reverse(commandUL);
              
              uint16_t address = (uint16_t)commandUL;
              uint8_t com = (uint8_t)(commandUL >> 16);
              IrSender.sendNEC(address, com, numRepeats);
              
              Serial.println("OK");

              if(userDebugEnabled)
              {
                digitalWrite(M5_LED, LOW);
                delay(50);
                digitalWrite(M5_LED, HIGH);
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
          lcdPrintf("%s\n", message);
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
}

unsigned long reverse(unsigned long x)
{
    x = (((x & 0xaaaaaaaa) >> 1) | ((x & 0x55555555) << 1));
    x = (((x & 0xcccccccc) >> 2) | ((x & 0x33333333) << 2));
    x = (((x & 0xf0f0f0f0) >> 4) | ((x & 0x0f0f0f0f) << 4));
    x = (((x & 0xff00ff00) >> 8) | ((x & 0x00ff00ff) << 8));
    return((x >> 16) | (x << 16));
}
