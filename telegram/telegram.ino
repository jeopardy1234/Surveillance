#include <WiFi.h>
#include <WiFiClientSecure.h>
#include "HTTPClient.h"
#include "ThingSpeak.h"
#include <UniversalTelegramBot.h> 
#include <ArduinoJson.h>

const char* ssid = "your ssid";
const char* password = "your password"; 
const char * myWriteAPIKey = "CQ7UD6JQL88FRNH4";
unsigned long myChannelNumber = mychannel_number;
int led = -1;

#define BOTtoken "your bot token" 
#define CHAT_ID "chat_id"
#define SOUND_SPEED 0.034
#define timeSeconds 10

WiFiClientSecure client;
WiFiClient cl;
UniversalTelegramBot bot(BOTtoken, client);

int botRequestDelay = 1000;
unsigned long lastTimeBotRan;

int num_times_motion = 0;
unsigned long now = millis();
unsigned long lastTrigger = 0;
const int ledPin = 2;
const int motionSensor = 27;
const int buzzerPin = 19;
const int trigPin = 5;
const int echoPin = 18;


long duration;
float distanceCm;
bool ledState = LOW;
bool buzzerState = LOW;
bool startTimer = false;

void IRAM_ATTR detectsMovement()
{
    num_times_motion++;
    startTimer = true;
    lastTrigger = millis();
}

void IRAM_ATTR led_blink()
{
     if(led == -1)return;
     ledState = !ledState;
     digitalWrite(ledPin, ledState);
     delay(500);
}

String chat_id = CHAT_ID;
String cse_ip = "your ip";
String cse_port = "8080";
String server = "http://" + cse_ip + ":" + cse_port + "/~/in-cse/in-name/";
String ae = "Ultrasonic_Sensor";
String cnt = "Distance";

void createCI(String& val){
    HTTPClient http;
    http.begin(server + ae + "/" + cnt + "/");
    http.addHeader("X-M2M-Origin", "admin:admin");
    http.addHeader("Content-Type", "application/json;ty=4");
    int code = http.POST("{\"m2m:cin\": {\"cnf\":\"application/json\",\"con\": " + String(val) + "}}");
    Serial.println(code);
    if (code == -1) {
    Serial.println("UNABLE TO CONNECT TO THE SERVER");
    }
    http.end();
}

void setup()
{
    Serial.begin(115200);
    pinMode(ledPin, OUTPUT);
    digitalWrite(ledPin, ledState);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    client.setCACert(TELEGRAM_CERTIFICATE_ROOT);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(1000);
        Serial.println("Connecting to WiFi..");
    }
    Serial.println(WiFi.localIP());
    pinMode(motionSensor, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(motionSensor), detectsMovement, RISING);
    //attachInterrupt(digitalPinToInterrupt(ledPin), led_blink, CHANGE);
    ledcSetup(0,1E5,18);
    ThingSpeak.begin(cl);
    pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
    pinMode(echoPin, INPUT); // Sets the echoPin as an Input
}

void handleNewMessages(int numNewMessages)
{
    Serial.println("handleNewMessages");
    Serial.println(String(numNewMessages));
    for (int i = 0; i < numNewMessages; i++)
    {
        // Chat id of the requester
        //    if (chat_id != CHAT_ID){
        //      bot.sendMessage(chat_id, "Unauthorized user", "");
        //      continue;
        //    }
        //
        // Print the received message
        String text = bot.messages[i].text;
        Serial.println(text);

        String from_name = bot.messages[i].from_name;

        if (text == "/start")
        {
            String welcome = "Welcome, " + from_name + ".\n";
            welcome += "Use the following commands to control your outputs.\n\n";
            welcome += "/led_warn to turn GPIO ON \n";
            welcome += "/led_off to turn GPIO OFF \n";
            welcome += "/buzzer_warn to turn GPIO ON \n";
            welcome += "/buzzer_off to turn GPIO OFF \n";
            welcome += "/led_state to request current GPIO state \n";
            welcome += "/buzzer_state to request current GPIO state \n";
            bot.sendMessage(chat_id, welcome, "");
        }

        if (text == "/led_warn")
        {
            led = 1;
            bot.sendMessage(chat_id, "LED state set to ON", "");
            ledState = HIGH;
            digitalWrite(ledPin, ledState);
            delay(500);
        }

        if (text == "/led_off")
        {
            led = -1;
            bot.sendMessage(chat_id, "LED state set to OFF", "");
            ledState = LOW;
            digitalWrite(ledPin, ledState);
        }

        if (text == "/buzzer_warn")
        {
           bot.sendMessage(chat_id, "Buzzer state set to ON", "");
           buzzerState = HIGH;
           ledcAttachPin(buzzerPin,0);
           ledcWriteTone(0,800);
        }

        if (text == "/buzzer_off")
        {
           bot.sendMessage(chat_id, "Buzzer state set to OFF", "");
           buzzerState = LOW;
           ledcDetachPin(buzzerPin);
        }

        if (text == "/led_state")
        {
            if (digitalRead(ledPin))
            {
                bot.sendMessage(chat_id, "LED is ON", "");
            }
            else
            {
                bot.sendMessage(chat_id, "LED is OFF", "");
            }
        }

        if (text == "/buzzer_state")
        {
            if (buzzerState == 1)
            {
                bot.sendMessage(chat_id, "Buzzer is ON", "");
            }
            else
            {
                bot.sendMessage(chat_id, "Buzzer is OFF", "");
            }
        }
    }
}

void loop()
{
    now = millis();
    if (millis() > lastTimeBotRan + botRequestDelay)
    {
        digitalWrite(trigPin, LOW);
        delayMicroseconds(2);
        digitalWrite(trigPin, HIGH);
        delayMicroseconds(10);
        digitalWrite(trigPin, LOW);
        
        duration = pulseIn(echoPin, HIGH);
        distanceCm = duration * SOUND_SPEED/2;
        Serial.print("Distance (cm): ");
        Serial.println(distanceCm);
        
        String val_write = String(distanceCm); 
        createCI(val_write);
        int x = ThingSpeak.writeField(myChannelNumber, 1, num_times_motion, myWriteAPIKey);
        if(x == 200){
          Serial.println("Channel update successful.");
        }
        else{
          Serial.println("Problem updating channel. HTTP error code " + String(x));
        }
        int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
        if (startTimer && (now - lastTrigger > (timeSeconds * 1000)))
        {
            startTimer = false;
            delay(100);
            Serial.println("Motion Detected!");
            Serial.println("LED ON");
            String welcome = "Intruder Sensed!!! What do you want to do.\n\n";
            welcome += "/led_warn to blink LED and warn people!\n";
            welcome += "/buzzer_warn to turn on buzzer to warn! \n";
            bot.sendMessage(chat_id, welcome, "");
            while (numNewMessages)
            {
                Serial.println("got response");
                handleNewMessages(numNewMessages);
                numNewMessages = bot.getUpdates(bot.last_message_received + 1);
            }
      
        }
        while (numNewMessages)
        {
            Serial.println("got response");
            handleNewMessages(numNewMessages);
            numNewMessages = bot.getUpdates(bot.last_message_received + 1);
        }
        lastTimeBotRan = millis();
    }
}
