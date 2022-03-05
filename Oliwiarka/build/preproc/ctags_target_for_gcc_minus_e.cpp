# 1 "c:\\Users\\paszk\\OneDrive\\Dokumenty\\GitHub\\Oliwiarka\\Oliwiarka\\Oliwiarka.ino"
# 2 "c:\\Users\\paszk\\OneDrive\\Dokumenty\\GitHub\\Oliwiarka\\Oliwiarka\\Oliwiarka.ino" 2
# 3 "c:\\Users\\paszk\\OneDrive\\Dokumenty\\GitHub\\Oliwiarka\\Oliwiarka\\Oliwiarka.ino" 2
# 4 "c:\\Users\\paszk\\OneDrive\\Dokumenty\\GitHub\\Oliwiarka\\Oliwiarka\\Oliwiarka.ino" 2
# 5 "c:\\Users\\paszk\\OneDrive\\Dokumenty\\GitHub\\Oliwiarka\\Oliwiarka\\Oliwiarka.ino" 2

# 7 "c:\\Users\\paszk\\OneDrive\\Dokumenty\\GitHub\\Oliwiarka\\Oliwiarka\\Oliwiarka.ino" 2
# 8 "c:\\Users\\paszk\\OneDrive\\Dokumenty\\GitHub\\Oliwiarka\\Oliwiarka\\Oliwiarka.ino" 2
# 9 "c:\\Users\\paszk\\OneDrive\\Dokumenty\\GitHub\\Oliwiarka\\Oliwiarka\\Oliwiarka.ino" 2
# 10 "c:\\Users\\paszk\\OneDrive\\Dokumenty\\GitHub\\Oliwiarka\\Oliwiarka\\Oliwiarka.ino" 2
# 11 "c:\\Users\\paszk\\OneDrive\\Dokumenty\\GitHub\\Oliwiarka\\Oliwiarka\\Oliwiarka.ino" 2
# 12 "c:\\Users\\paszk\\OneDrive\\Dokumenty\\GitHub\\Oliwiarka\\Oliwiarka\\Oliwiarka.ino" 2
# 13 "c:\\Users\\paszk\\OneDrive\\Dokumenty\\GitHub\\Oliwiarka\\Oliwiarka\\Oliwiarka.ino" 2
# 22 "c:\\Users\\paszk\\OneDrive\\Dokumenty\\GitHub\\Oliwiarka\\Oliwiarka\\Oliwiarka.ino"
// const char* ssid     = "Oliwiarka-update";
// const char* password = "123456789";
const char *ssid = "sgx";
const char *password = ".........";

AsyncWebServer server(80);
BluetoothSerial SerialBT;
StaticJsonDocument<255> doc;
StaticJsonDocument<255> out;

Preferences preferences;

SimpleTimer firstTimer(50);
SimpleTimer statusTimer(2000);
SimpleTimer updateInputs(100);

int NotSave = 0;

int opTime = 10; //*50ms   -> przerwa
int opDuration = 10; //*50ms   -> czas pulsu
float voltage = 0.0;

int opCapacity = 200; // 500ml  (32x16) -> ilosc pulsow max stan pojemość (do sprawdzenia)
int opState = opCapacity; //        -> stan zbiornika
const int pulseWeigh = 16; // ml -> https://pl.aliexpress.com/item/1005001927588714.html?spm=a2g0s.9042311.0.0.428a5c0fPJiwVj

int cnt = 0;
int error_cnt = 0;

int incomingByte = 0;
int outputingByte = 0;

char str[255];
char bfr[255];

char c;
int val;
char save = 0;
char restore = 0;
char test[20];
String bt_name = "Oliwiarka";
char *ptr = test;
char suppress_enable = 0;
char Neutral = 0;

char blinker_cnt = 0;
char blinker_status = 0;

char lights_status = 0;





int potValue;

const int numReadings = 10;

int readings[numReadings]; // the readings from the analog input
int readIndex = 0; // the index of the current reading
int total = 0; // the running total
int average = 0; // the average
int loop_to_sleep = 0;

void gotoDeepSleep()
{
  StorePreferences();
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_35, 0);
  esp_deep_sleep_start();
}

void welcome()
{
  digitalWrite(22 /* GPIO22, VSPIWP, U0RTS, EMAC_TXD1*/, 0x0);
  digitalWrite(23 /* GPIO23, VSPID, HS1_STROBE*/, 0x1);
  delay(50);
  digitalWrite(23 /* GPIO23, VSPID, HS1_STROBE*/, 0x0);
  digitalWrite(22 /* GPIO22, VSPIWP, U0RTS, EMAC_TXD1*/, 0x1);
  delay(50);
  digitalWrite(22 /* GPIO22, VSPIWP, U0RTS, EMAC_TXD1*/, 0x0);
  digitalWrite(23 /* GPIO23, VSPID, HS1_STROBE*/, 0x1);
  delay(50);
  digitalWrite(23 /* GPIO23, VSPID, HS1_STROBE*/, 0x0);
  digitalWrite(22 /* GPIO22, VSPIWP, U0RTS, EMAC_TXD1*/, 0x1);
  delay(50);
  digitalWrite(23 /* GPIO23, VSPID, HS1_STROBE*/, 0x1);
  digitalWrite(22 /* GPIO22, VSPIWP, U0RTS, EMAC_TXD1*/, 0x1);
}
void StorePreferences()
{
  preferences.begin("Oliwiarka", false);
  preferences.putInt("opTime", opTime);
  preferences.putInt("opDuration", opDuration);
  preferences.putInt("opState", opState);
  preferences.putInt("opCapacity", opCapacity);
  preferences.putInt("suppress_enable", suppress_enable);
  preferences.putString("btName", bt_name);
  preferences.end();
}

void retrivePreferences()
{

  preferences.begin("Oliwiarka", false);
  opTime = preferences.getInt("opTime", opTime);
  opDuration = preferences.getInt("opDuration", opDuration);
  opState = preferences.getInt("opState", opState);
  opCapacity = preferences.getInt("opCapacity", opCapacity);
  suppress_enable = preferences.getInt("suppress_enable", suppress_enable);
  bt_name = preferences.getString("btName", bt_name);
  preferences.end();
}
void setup(void)
{

  pinMode(13 /* GPIO13, ADC2_CH4, TOUCH4, RTC_GPIO14, MTCK, HSPID,HS2_DATA3, SD_DATA3, EMAC_RX_ER*/, 0x02);
  pinMode(23 /* GPIO23, VSPID, HS1_STROBE*/, 0x02);
  pinMode(22 /* GPIO22, VSPIWP, U0RTS, EMAC_TXD1*/, 0x02);
  pinMode(15 /* GPIO15, ADC2_CH3, TOUCH3, MTDO, HSPICS0, RTC_GPIO13,HS2_CMD, SD_CMD, EMAC_RXD3*/, 0x01);
  pinMode(35 /* GPIO35, ADC1_CH7, RTC_GPIO5 / GPIO39, ADC1_CH3, RTC_GPIO3*/, 0x01);
  pinMode(34 /* GPIO34, ADC1_CH6, RTC_GPIO4*/, 0x01);
  pinMode(36 /* GPIO36, ADC1_CH0, RTC_GPIO0*/, 0x01);
  digitalWrite(22 /* GPIO22, VSPIWP, U0RTS, EMAC_TXD1*/, 0x1);
  digitalWrite(23 /* GPIO23, VSPID, HS1_STROBE*/, 0x1);
  digitalWrite(13 /* GPIO13, ADC2_CH4, TOUCH4, RTC_GPIO14, MTCK, HSPID,HS2_DATA3, SD_DATA3, EMAC_RX_ER*/, 0x0);
  retrivePreferences();
  welcome();
  bt_name.toCharArray(test, bt_name.length() + 1);
  SerialBT.begin(ptr); // Bluetooth device name

  if (digitalRead(36 /* GPIO36, ADC1_CH0, RTC_GPIO0*/) == 0x1)
  {
    WiFi.mode(WIFI_MODE_STA);
    WiFi.begin(ssid, password);

    // IPAddress ip(192,168,1,1);
    // IPAddress gateway(192,168,1,1);
    // IPAddress subnet(255,255,255,0);
    // WiFi.softAPConfig(ip, gateway, subnet);
    // WiFi.softAP(ssid, NULL, 9, 0, 1);
    // WiFi.mode(WIFI_AP);

    while (WiFi.status() != WL_CONNECTED)
    {
      delay(500);
    }
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(200, "text/html", "<a href=\"/update\"><img src=\"http://www.novi-travnik.info/under.png\"></a>"); });
    AsyncElegantOTA.begin(&server); // Start ElegantOTA
    server.begin();
  }

  for (int thisReading = 0; thisReading < numReadings; thisReading++)
  {
    readings[thisReading] = 0;
  }
}

void loop(void)
{

  while (SerialBT.available())
  {
    c = SerialBT.read();
    if (incomingByte < 253)
    {
      str[incomingByte++] = c;
    } /*

     if (c == '}')

     {

       Serial.println(c);

     }

     else

     {

       Serial.print(c);

     }*/
# 198 "c:\\Users\\paszk\\OneDrive\\Dokumenty\\GitHub\\Oliwiarka\\Oliwiarka\\Oliwiarka.ino"
  }

  if (incomingByte > 0)
  {

    DeserializationError error = deserializeJson(doc, str);
    JsonObject obj = doc.as<JsonObject>();

    if (error)
    {
      error_cnt++;
      digitalWrite(22 /* GPIO22, VSPIWP, U0RTS, EMAC_TXD1*/, 0x0);
    }
    else
    {
      digitalWrite(22 /* GPIO22, VSPIWP, U0RTS, EMAC_TXD1*/, 0x1);
      if (obj["btName"])
      {
        bt_name = obj["btName"].as<String>();
      }

      if (doc["opTime"] > 0)
      {
        if (doc["opTime"] != opTime)
        {
          opTime = doc["opTime"];
          cnt = 0;
          NotSave = 1;
        }
      }
      if (doc["opDuration"] > 0)
      {
        if (doc["opDuration"] != opDuration)
        {
          opDuration = doc["opDuration"];
          NotSave = 1;
        }
      }
      if (doc["opState"] > 0)
      {
        if (doc["opState"] != opState)
        {
          opState = doc["opState"];
          NotSave = 1;
        }
      }
      if (doc["opCapacity"] > 0)
      {
        if (doc["opCapacity"] != opCapacity)
        {
          opCapacity = doc["opCapacity"];
          NotSave = 1;
        }
      }
      // if(doc["suppress"] > 0){
      if (doc["suppress"] != suppress_enable)
      {
        suppress_enable = doc["suppress"];
        NotSave = 1;
      }
      //}

      if (doc["CMD"] > 0)
      {
        int cmd = doc["CMD"];
        // Serial.print("Received command: ");
        switch (cmd)
        {
        case 1:
        {
          // Serial.println("Reset");
          ESP.restart();
        }
        break;
        case 9:
        {
          // Serial.println("Restore settings");
          restore = 1;
        }
        break;
        case 11:
        {
          // Serial.println("Save settngs");
          save = 1;
        }
        break;
        case 15:
        {
          // Serial.println("refill");
          suppress_enable = 1;
        }
        break;
        case 20:
        {
          // Serial.println("Sleep");
          gotoDeepSleep();
        }
        break;
        }
      }
      if (save > 0)
      {
        StorePreferences();
        save = 0;
        NotSave = 0;
      }
      if (restore > 0)
      {
        retrivePreferences();
        NotSave = 0;
        restore = 0;
      }
    }
    incomingByte = 0;
  }

  total = total - readings[readIndex];
  readings[readIndex] = analogRead(34 /* GPIO34, ADC1_CH6, RTC_GPIO4*/);
  total = total + readings[readIndex];
  readIndex = readIndex + 1;
  if (readIndex >= numReadings)
  {
    // ...wrap around to the beginning:
    readIndex = 0;
  }

  // calculate the average:
  average = total / numReadings;

  if (firstTimer.isReady())
  {
    if (digitalRead(36 /* GPIO36, ADC1_CH0, RTC_GPIO0*/) == 0x1)
    {
      Neutral = 1;
    }
    else
    {
      Neutral = 0;
    }

    if (!suppress_enable and Neutral == 0)
    {
      if (cnt == opTime)
      {
        if (opState > 0)
        {

          {
            digitalWrite(13 /* GPIO13, ADC2_CH4, TOUCH4, RTC_GPIO14, MTCK, HSPID,HS2_DATA3, SD_DATA3, EMAC_RX_ER*/, 0x1);
          }
        }
      }

      if (cnt == opTime + opDuration)
      {
        if (opState > 0)
        {

          opState--;
        }
        digitalWrite(13 /* GPIO13, ADC2_CH4, TOUCH4, RTC_GPIO14, MTCK, HSPID,HS2_DATA3, SD_DATA3, EMAC_RX_ER*/, 0x0);
        cnt = 0;
      }
      cnt++;
    }
    else
    {

      digitalWrite(13 /* GPIO13, ADC2_CH4, TOUCH4, RTC_GPIO14, MTCK, HSPID,HS2_DATA3, SD_DATA3, EMAC_RX_ER*/, 0x0);
    }
    if (opState == 0)
    {
      digitalWrite(22 /* GPIO22, VSPIWP, U0RTS, EMAC_TXD1*/, 0x0);
    }
    firstTimer.reset();
  }

  if (statusTimer.isReady())
  {
    digitalWrite(23 /* GPIO23, VSPID, HS1_STROBE*/, !digitalRead(23 /* GPIO23, VSPID, HS1_STROBE*/));
    if (SerialBT.hasClient())
    {
      out["opTime"] = opTime;
      out["opDuration"] = opDuration;
      out["opState"] = opState;
      out["opCapacity"] = opCapacity;
      out["error"] = error_cnt;
      out["suppress"] = suppress_enable;
      out["NotSave"] = NotSave;
      out["Neutral"] = Neutral;
      out["sleep"] = loop_to_sleep;
      out["Binker"] = blinker_status;
      out["Volt"] = average / 100.0;
      serializeJson(out, SerialBT);
      SerialBT.write(0xd);
    }
    statusTimer.reset();
  }
  if (updateInputs.isReady())
  {
    if (digitalRead(35 /* GPIO35, ADC1_CH7, RTC_GPIO5 / GPIO39, ADC1_CH3, RTC_GPIO3*/) == 0x1)
    {
      lights_status = 0;
      loop_to_sleep++;
      if (loop_to_sleep > 599)
      { // 60sec
        loop_to_sleep = 0;
        gotoDeepSleep();
      }
    }
    else
    {
      loop_to_sleep = 0;
      lights_status = 1;
    }

    // char blinker_cnt = 0;
    // char blinker_status = 0;

    if (digitalRead(15 /* GPIO15, ADC2_CH3, TOUCH3, MTDO, HSPICS0, RTC_GPIO13,HS2_CMD, SD_CMD, EMAC_RXD3*/) == 0x0)
    {
      // binker active
      blinker_cnt = 15; // 1.5sec
      blinker_status = 1; // blinker active
    }
    else
    {
      if (blinker_cnt > 0)
      {
        blinker_cnt--;
      }
      else
      {
        blinker_status = 0; // blinker inactive
      }
    }

    updateInputs.reset();
  }
}
