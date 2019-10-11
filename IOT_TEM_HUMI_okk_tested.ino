#include<EEPROM.h>
#include<LM35.h>
#include<ThingSpeak.h>
#include<LiquidCrystal.h>
LiquidCrystal lcd(5, 6, 7, 8, 9, 10);
#include<Timer.h>
Timer t;
#include <SoftwareSerial.h>
SoftwareSerial Serial1(2, 3);//tx,rx//

String str="";

#define sw1 12
#define inc A2
#define dec A3

#define humiRly 11
#define minTempRly 13
#define midTempRly A4
#define maxTempRly 4

int moistureSense=A1;
int tempSense=A0;

char setFlag;
long count,count1;
int val;

float offset=0;

const char *host = "api.thingspeak.com";
char *api_key = "FF7K2ORV8BCHQ3T2";
static char postUrl[150];
float humi, tem;

long humiTimer,tempTimer;
unsigned int humiTimerCount,tempTimerCount;
long humiTime=0;

enum
{
  temMinLoc = 0,
  temMidLoc,
  temMaxLoc,
  humiLoc,
  temTimerLoc,
  humiTimerLoc,
};

void serialEvent1()
{
  while(Serial1.available())
  {
    char ch=Serial1.read();
  //  Serial.print(ch);
    str+=ch;
  }
}

void connect_wifi(String cmd, String res, int t)
{
  int temp = 0, i = 0;
  while (1)
  {
    //lcd.clear();
    //lcd.print(cmd);
    //Serial.println(cmd);
    str="";
    Serial1.println(cmd);
    delay(1000);
    serialEvent1();
    Serial.print(str);
    if(str.indexOf(res)>=0)
    {
      break;
    }
    delay(t);
    if (i > 5)
      break;
    i++;
  }
}

void send2server()
{
  count1++;
  if(count1>=120)
  {
    count1=0;
  lcd.clear();
  lcd.print("Sending Data ");
  lcd.setCursor(0,1);
  lcd.print("To Server");
  char tempStr[8];
  char humidStr[8];
  dtostrf(tem, 5, 3, tempStr);
  dtostrf(humi, 5, 3, humidStr);
  sprintf(postUrl, "update?api_key=%s&field1=%s&field2=%s", api_key, tempStr, humidStr);
  httpGet("api.thingspeak.com", postUrl, 80);
  httpGet("api.thingspeak.com", postUrl, 80);
  lcd.clear();
  }
}

void httpGet(String ip, String path, int port)
{
  int resp;
  String atHttpGetCmd = "GET /" + path + " HTTP/1.0\r\n\r\n";
  //AT+CIPSTART="TCP","192.168.20.200",80
  String atTcpPortConnectCmd = "AT+CIPSTART=\"TCP\",\"" + ip + "\"," + port + "";
  connect_wifi(atTcpPortConnectCmd, "CONNECT", 2000);
  int len = atHttpGetCmd.length();
  String atSendCmd = "AT+CIPSEND=";
  atSendCmd += len;
  Serial1.println(atSendCmd);
  delay(1000);
  Serial1.println(atHttpGetCmd);
  delay(2000);
}

void Manu()
{
  setFlag=1;
  if (digitalRead(sw1) == 0)
  {
    count++;
    if (count>6)
      count = 0;
    setFlag = 0;
    lcd.clear();
    if(count == 1)
    val=EEPROM.read(temMinLoc);
    else if(count == 2)
    val=EEPROM.read(temMidLoc);
    else if(count == 3)
    val=EEPROM.read(temMaxLoc);
    else if(count == 4)
    val=EEPROM.read(humiLoc);
    else if(count == 5)
    val=EEPROM.read(temTimerLoc);
    else if(count == 6)
    val=EEPROM.read(humiTimerLoc);
  }

 /* else if (digitalRead(ok) == 0)
  {
    setFlag = 1;
    delay(500);
  }
*/
  else if (digitalRead(inc) == 0)
  {
    val++;
    if (val >= 100)
      val = 0;
      delay(75);
  }

  else if (digitalRead(dec) == 0)
  {
    val--;
    if (val <= 0)
      val = 100;
      delay(75);
  }

  switch (count)
  {
    
    case 1:
      lcd.setCursor(0, 0);
      lcd.print("1. Temp Min:");
      if (setFlag)
      {
        lcd.setCursor(0, 1);
       // val = (int)EEPROM.read(temMinLoc) + val;
        lcd.print(val);
        EEPROM.write(temMinLoc, val);
       //                 lcd.setCursor(6, 1);
        lcd.print(" *C");
      }
      break;

    case 2:
      lcd.setCursor(0, 0);
      lcd.print("2. Temp Mid:");
      if (setFlag)
      {
       lcd.setCursor(0, 1);
       // val = EEPROM.read(temMidLoc) + val;
        lcd.print(val);
        EEPROM.write(temMidLoc, val);
                    //    lcd.setCursor(6, 1);
        lcd.print(" *C");
      }
      break;

    case 3:
      lcd.setCursor(0, 0);
      lcd.print("3. Temp Max:");
      if (setFlag)
      {
        lcd.setCursor(0, 1);
        //val = (int)EEPROM.read(temMaxLoc) + val;
        lcd.print(val);
          //      lcd.setCursor(6, 1);
        lcd.print(" *C");
        EEPROM.write(temMaxLoc, val);
      }
      break;

    case 4:
      lcd.setCursor(0, 0);
      lcd.print("4. Moisture limit:");
      if (setFlag)
      {
       // lcd.setCursor(0, 1);
        //val = EEPROM.read(humiLoc) + val;
        
                lcd.setCursor(0, 1);
                lcd.print(val);
        lcd.print(" %");
        EEPROM.write(humiLoc, val);
      }
      break;

    case 5:
      lcd.setCursor(0, 0);
      lcd.print("5. Temp Timer:");
      if (setFlag)
      {
      //  lcd.setCursor(0, 1);
        //val = EEPROM.read(temTimerLoc) + val;
        
      //  lcd.print("      ");
                lcd.setCursor(0, 1);
                lcd.print(val);
        lcd.print("  Minute");
        EEPROM.write(temTimerLoc, val);
      }
      break;

    case 6:
      lcd.setCursor(0, 0);
      lcd.print("6. Moisture Timer:");
      if (setFlag)
      {
      //  
        //val = EEPROM.read(humiTimerLoc) + val;
        lcd.setCursor(0, 1);
        lcd.print(val);
        lcd.print("  Minute");
        EEPROM.write(humiTimerLoc, val);
      }
      break;
  }
  lcd.print("     ");
  delay(400);
}

void timerFun()
{
 // humiTimerCount++;
  tempTimerCount++;
}

void setup()
{
  lcd.begin(16, 2);
  lcd.clear();
  lcd.print("  Temp & Humi    ");
  lcd.setCursor(0, 1);
  lcd.print("Control & Monitor");
  delay(2000);
  Serial1.begin(9600);
  Serial.begin(9600);
   lcd.clear();
    lcd.print("WIFI Connecting");
    lcd.setCursor(0,1);
    lcd.print("Please wait....");
  //  Serial.println("Connecting Wifi....");
    Serial1.println("AT+UART_DEF=9600,8,1,0,0");
    connect_wifi("AT","OK",1000);
    connect_wifi("ATE1","OK",1000);
    Serial1.println("AT+CWMODE=3");
    delay(1000);
    connect_wifi("AT+CWQAP","OK",1000);
//    connect_wifi("AT+RST",5000);
    connect_wifi("AT+CWJAP=\"harshit\",\"12345678\"","CONNECT",5000);
 // connect_wifi("AT+CWJAP=\"Redmi\",\"arishkhan\"","CONNECT",5000);
    Serial.println("Wifi Connected");
    lcd.clear();
    lcd.print("WIFI Connected.");

 // pinMode(heart, OUTPUT);
  pinMode(sw1, INPUT_PULLUP);
  pinMode(inc, INPUT_PULLUP);
  pinMode(dec, INPUT_PULLUP);
  pinMode(humiRly, OUTPUT);
  pinMode(minTempRly, OUTPUT);
  pinMode(midTempRly, OUTPUT);
  pinMode(maxTempRly, OUTPUT);
  for (int i = 0 ; i < 255 ; i++) 
  {
    if(EEPROM.read(i) == 255)
    EEPROM.write(i, 0);
  }
  delay(2000);
float val1=0;
  for(int i=0;i<50;i++)
{
  val1+=analogRead(moistureSense);
}
offset=((val1/50)/100)+0.01;
Serial.println(offset);
  
  //t.oscillate(heart, 1000, LOW);
 t.every(30000, send2server);
//  t.every(1000, timerFun);
  lcd.clear();
}

void loop()
{
  if (count == 0)
  {
    //pinMode(moistureSense, OUTPUT);
   // pinMode(moistureSense, INPUT);
    float val1=analogRead(moistureSense);
    
    Serial.println(val1);
    humi =100.0-((val1/offset));
    lcd.setCursor(0, 0);
    lcd.print("Mois:");
    lcd.print(humi);

    if(humi < EEPROM.read(humiLoc))
    {
      lcd.print("  Norm");
      digitalWrite(humiRly, HIGH);
      //humiTimer=0;
    }

    else
    {
      lcd.print("  Crit");
      if(humiTimer == 0)
      {
       // lcd.print("HumiRly ON");
        digitalWrite(humiRly, LOW);
        digitalWrite(maxTempRly, HIGH);
        humiTimer=1;
        humiTimerCount=millis();
        Serial.println(humiTimerCount);
        humiTime=((int)EEPROM.read(humiTimerLoc) * 60000);
        Serial.print("Humi Time:");
        Serial.println(humiTime);
        delay(2000);
      //  lcd.clear();
      }
    }
    Serial.println(millis()-humiTimerCount);
    
    if((millis()-humiTimerCount > humiTime) && humiTimer == 1)
    {
     //  lcd.clear();
       //lcd.print("HumiRly OFF");
       digitalWrite(humiRly, HIGH);
       digitalWrite(maxTempRly, LOW);
       humiTimer=0;
       delay(2000);
      // lcd.clear();
    }

   float val=analogRead(tempSense);
    Serial.println(val);
    tem = val*(5.0/1024.0)*100.0;
    
    lcd.setCursor(0, 1);
    lcd.print("Temp:");
    lcd.print(tem);

    if(tem < EEPROM.read(temMinLoc))
    {
      lcd.print("  Norm");
      digitalWrite(minTempRly, LOW);
      digitalWrite(midTempRly, LOW);
      //digitalWrite(maxTempRly, LOW);
    }

    else if((tem < EEPROM.read(temMidLoc)) && tem>EEPROM.read(temMinLoc))
    {
      lcd.print("  Min");
      digitalWrite(minTempRly, HIGH);
      digitalWrite(midTempRly, LOW);
      //digitalWrite(maxTempRly, LOW);
    }

    else if((tem < EEPROM.read(temMaxLoc)) && (tem > EEPROM.read(temMidLoc)))
    {
      lcd.print("  Mid");
      digitalWrite(minTempRly, HIGH);
      digitalWrite(midTempRly, HIGH);
      //digitalWrite(maxTempRly, LOW);
    }

    else if(tem > EEPROM.read(temMaxLoc) )
    {
      digitalWrite(minTempRly, HIGH);
      digitalWrite(midTempRly, HIGH);
      
      if(tempTimer == 0)
      {
        lcd.print(" Max");
        tempTimer=1;
        digitalWrite(maxTempRly, HIGH);
        tempTimerCount=millis();
        delay(2000);
       // lcd.clear();
      }
    }

    if((millis()-tempTimerCount > ((int)EEPROM.read(temTimerLoc) * 60000)) && tempTimer == 1)
    {
       //lcd.clear();
       //lcd.print("Temp Rly OFF");
       digitalWrite(maxTempRly, LOW);
       tempTimer=0;
       delay(2000);
       //lcd.clear();
    }    
  }
  Manu();
  delay(200);
  t.update();
}
