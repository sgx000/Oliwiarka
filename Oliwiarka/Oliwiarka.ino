//Oliwiarka
/*Do dodania:
 *      -zapis ustawien - ok
 *      -sleep wakeup
 *      -lepsze sterowanie zaworem 
 *      
 * 
 * 
 * 
 */

#include "BluetoothSerial.h"
#include "Preferences.h"
#include <SimpleTimer.h>
#include <ArduinoJson.h>
#include <String.h>


#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BluetoothSerial SerialBT;
Preferences preferences;
StaticJsonDocument<255> doc;
StaticJsonDocument<255> out;

SimpleTimer firstTimer(50);
SimpleTimer statusTimer(2000);
int       NotSave     = 0;

int       opTime     = 10;    //*50ms   -> przerwa
int       opDuration = 10;    //*50ms   -> czas pulsu


int       opCapacity = 200;    // 500ml  (32x16) -> ilosc pulsow max stan pojemość (do sprawdzenia)
int       opState    = opCapacity;     //        -> stan zbiornika
const int pulseWeigh = 16;    //ml -> https://pl.aliexpress.com/item/1005001927588714.html?spm=a2g0s.9042311.0.0.428a5c0fPJiwVj


int cnt = 0;
int error_cnt = 0;



int incomingByte = 0; 
int outputingByte = 0;
char psettings=0;       //print settings
char str[255];
char bfr[255];
char c;
char save = 0;
char restore = 0;
//char* btName = "Oliwiarka";
char test[20];
String bt_name = "Oliwiarka";
char* ptr =test;
char suppress_enable = 0;
char Neutral = 0;


void setup() {
  Serial.begin(115200);
  //Serial.println("The device started, now you can pair it with bluetooth!");
  pinMode(22, OUTPUT);
  retrivePreferences();
  bt_name.toCharArray(test, bt_name.length()+1);

  Serial.println(test);
  SerialBT.begin(ptr); //Bluetooth device name
  //opState  = opCapacity - opState;

  
}

void loop() {
 
  if (Serial.available()) {
    SerialBT.write(Serial.read());
  }

  while(SerialBT.available()){
      c = SerialBT.read();
      if(incomingByte < 253){
        str[incomingByte++] = c;
        
      }
  }
  if(incomingByte > 0){
    
    DeserializationError error = deserializeJson(doc, str);
    JsonObject obj = doc.as<JsonObject>();
    
    if (error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      error_cnt++;
 
  }else{ 

    
     if(doc["CMD"] > 0){
           int cmd = doc["CMD"];
          Serial.print("Received command: ");
          switch(cmd){
            case 1:{
              Serial.println("Reset");  
              ESP.restart();  
            } break;
            case 9:{
              Serial.println("Restore settings"); 
              restore=1; 
            } break;
            case 11:{
              Serial.println("Save settngs");    
              save = 1;
            } break;
            case 15:{
              Serial.println("Supress lubrication");    
              suppress_enable = 1;
            } break;
            case 16:{
              Serial.println("Enable lubrication");    
              suppress_enable = 0;
            } break;
            
          }
          
     }

     if (obj["btName"]){
        Serial.print("new name received ");
        Serial.println(obj["btName"].as<String>());
        bt_name = obj["btName"].as<String>();
      
     }

     
     if(doc["opTime"] > 0){
          if(doc["opTime"] != opTime){
              opTime     = doc["opTime"];  
              cnt = 0;
              NotSave = 1;
          }
          
     }
     if(doc["opDuration"] >0){
        if(doc["opDuration"] != opDuration){
          opDuration = doc["opDuration"];
          NotSave = 1;
        }
     }
     if(doc["opState"] > 0){
        if(doc["opState"] != opState){
           opState    = doc["opState"];
           NotSave = 1;
        }
     }
     if(doc["opCapacity"] > 0){
        if(doc["opCapacity"] != opCapacity){
          opCapacity = doc["opCapacity"];
          NotSave = 1;
        }
     }
    if(save>0){
      StorePreferences();
      save=0;
      NotSave = 0;
    }
    if(restore>0){
      retrivePreferences();
      NotSave = 0;
      restore = 0;
    }
    Serial.println(opTime);
    Serial.println(opDuration);
   
  }
   incomingByte = 0;
  }

    
 

 
   if (firstTimer.isReady() ) {            // Check is ready a first timer
    if(!suppress_enable){
      if(cnt == opTime){
        if(opState > 0 ){
        digitalWrite(22, LOW);  
        }
      }
      
      if(cnt == opTime+opDuration){
        if(opState > 0){
        
        opState--;
        }
        digitalWrite(22, HIGH);
        cnt = 0;
      }
      cnt++;
      }else{

        digitalWrite(22, HIGH);
      }
      firstTimer.reset();
    }
 
 
   if(statusTimer.isReady()){
      out["opTime"] = opTime;
      out["opDuration"] = opDuration;
      out["opState"] = opState;
      out["opCapacity"] = opCapacity;
      out["error"] = error_cnt;
      out["suppress"] = suppress_enable;
      out["NotSave"] = NotSave;
      out["Neutral"] = Neutral;
      //serializeJson(out, bfr);
      serializeJson(out,SerialBT);
      SerialBT.write(0xd);
    statusTimer.reset();
   }
  
}

//suppress_enable
void StorePreferences(){
      preferences.begin("Oliwiarka", false);
        preferences.putInt("opTime", opTime);
        preferences.putInt("opDuration", opDuration);
        preferences.putInt("opState", opState);
        preferences.putInt("opCapacity", opCapacity);
        preferences.putInt("suppress_enable", suppress_enable);
        preferences.putString("btName", bt_name);
      preferences.end();
  
}

void retrivePreferences(){
      preferences.begin("Oliwiarka", false);
        opTime =      preferences.getInt("opTime", opTime);
        opDuration =  preferences.getInt("opDuration", opDuration);
        opState =     preferences.getInt("opState", opState);
        opCapacity =  preferences.getInt("opCapacity", opCapacity);
        suppress_enable = preferences.getInt("suppress_enable", suppress_enable);
        bt_name = preferences.getString("btName", bt_name);
      preferences.end();
  
}
