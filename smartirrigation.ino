
#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <DHT.h>


char auth[] = "pfIGP8kglVfZVfRJlwMmOe00VUCSQdhO"; 
char ssid[] = "saikumar";  
char pass[] = "9059778846";  


#define DHTPIN D2
#define MoistureSensor A0 
float  moisture; 
#define DHTTYPE DHT11
int temp;
int humi;
DHT dht(DHTPIN,DHTTYPE);
BlynkTimer timer;
#define motor D3
#define mbutton D4
#define button D7
int buttonState;
#define Dry_soil 15
#define Wet_soil 50
#define led_motor_state V7
#define sleepbutton D8
int flag=0;

WidgetLED led1(led_motor_state);
WidgetTerminal terminal(V1);

BLYNK_WRITE(V1)
{
    terminal.write(param.getBuffer(), param.getLength());
    terminal.println();
    terminal.flush();
}

void sendSensor()
{
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  float moisture = analogRead(MoistureSensor);  
  moisture = map(moisture,1023,0,0,100); 
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  Blynk.virtualWrite(V4,moisture);
  Blynk.virtualWrite(V5,h);
  Blynk.virtualWrite(V6,t);
}

void setup()
{
  Serial.begin(9600);
  Blynk.begin(auth,ssid,pass);
  dht.begin();
  pinMode(button,INPUT);
  pinMode(MoistureSensor,INPUT);
  pinMode(motor,OUTPUT);
  timer.setInterval(1000L,sendSensor);
  terminal.flush();
}

void loop()
{  
  buttonState = digitalRead(button);
  Serial.print("buttonState: ");
  Serial.println(buttonState);
  delay(50);
  terminal.print("buttonState: ");
  terminal.println(buttonState);    
  delay(50);
  moisture = analogRead(MoistureSensor);  
  moisture = map(moisture,1023,0,0,100);
  Serial.print("moisture: ");
  Serial.print(moisture);
  Serial.println("%");  
  terminal.print("moisture: ");
  terminal.print(moisture);
  terminal.println("%");
  temp = dht.readTemperature();
  Serial.print("temp: ");
  Serial.print(temp);
  Serial.println(" c");  
  terminal.print("temp: ");
  terminal.print(temp);
  terminal.println(" c");
  humi = dht.readHumidity();
  Serial.print("humidity: ");
  Serial.print(humi);
  Serial.println(" %");  
  terminal.print("humidity: ");
  terminal.print(humi);
  terminal.println(" %");
  Serial.println("----");  
  terminal.print("----");
  delay(300);
  if(buttonState == LOW){
      selfmode();  
    }
  else{
      manualmode();
    }
   terminal.flush();
  Blynk.run();
  timer.run();
}

void selfmode(){
    Serial.println("AutoMode");
    delay(100);
    terminal.println("AutoMode");
    delay(200);
    if (moisture <= Dry_soil ){
      Serial.println("Dry land: motor should ON for a while");
      terminal.println("Dry land: motor should ON for a while");      
      flag=1;
      check_status();
      }
    else if (moisture < Wet_soil){
        Serial.println("Soil is slightly wet need some watering");
        terminal.println("Soil is slightly wet need some watering");
        flag=1;
        check_status();
    }
    else if (moisture >= Wet_soil){
        Serial.println("Soil is wet no need of watering");
        terminal.println("Soil is wet no need of watering");
        Serial.println("Going to sleep mode....");
        terminal.println("Going to sleep mode...."); 
        flag=0;
        check_status();
        sleepmode();
      }
    else if (temp <= 30 && humi>80){
        Serial.println("No need of watering");
        terminal.println("No need of watering");
        Serial.println("Going to sleep mode....");
        terminal.println("Going to sleep mode...."); 
        flag=0;
        check_status();
        sleepmode();
      }
    else{
        flag=0;
        check_status(); 
      } 
  }

 void manualmode(){
    Serial.println("ManualMode");
    delay(100);
    terminal.println("ManualMode");
    int motoronoff_button = digitalRead(mbutton);
    delay(200);
    int sleep_button = digitalRead(sleepbutton);
    delay(200);
    if (motoronoff_button == HIGH && sleep_button == LOW){
      Serial.println("Motor manually ON for a while");
      terminal.println("Motor manually ON for a while");
      turnPumpOn();
      }
    else if(motoronoff_button == LOW && sleep_button == HIGH){
      Serial.println("Going to sleep mode....");
      terminal.println("Going to sleep mode...."); 
      sleepmode();
      }
    else if(motoronoff_button == HIGH && sleep_button == HIGH){
      Serial.println("Please check both motor and deepsleep are pressed...");
      terminal.println("Please check both motor and deepsleep are pressed...");
      turnPumpOff();
      }
     else{
      Serial.println("Motor is in off state");
      terminal.println("Motor is in off state");
      turnPumpOff();
      }
  }
void check_status(){
  if(flag==1){
    turnPumpOn();
    }
   else if(flag==0){
    turnPumpOff();
    }
    else{
      Serial.println("Flsg Status not updating");
      terminal.println("Flsg Status not updating");
    }
  
  }

void turnPumpOn(){
  Blynk.notify("pump on for a while");
  led1.on();
  digitalWrite(motor,LOW);
  }
void turnPumpOff(){
  Blynk.notify("pump off");
  led1.off();
  digitalWrite(motor,HIGH);
  }
 void sleepmode(){
  Serial.println("Going to sleep for 10 secs...");
  ESP.deepSleep(10e6);
  }
