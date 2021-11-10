#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h> // Universal Telegram Bot Library written by Brian Lough: https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot
#include <ArduinoJson.h>

// Replace with your network credentials
const char* ssid = "****";
// havent put my actual password for security reasons
const char* password = "*****"; 
int ledX = -1;

// Initialize Telegram BOT
#define BOTtoken "******" // your Bot Token (Get from Botfather)

// Use @myidbot to find out the chat ID of an individual or a group
// Also note that you need to click "start" on a bot before it can
// message you
#define CHAT_ID "******"

#ifdef ESP8266
X509List cert(TELEGRAM_CERTIFICATE_ROOT);
#endif

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

// Checks for new messages every 1 second.
#define timeSeconds 10

int botRequestDelay = 1000;
unsigned long lastTimeBotRan;

unsigned long now = millis();
unsigned long lastTrigger = 0;
const int ledPin = 2;
const int motionSensor = 27;
bool ledState = LOW;

bool startTimer = false;

void IRAM_ATTR detectsMovement()
{
    startTimer = true;
    lastTrigger = millis();
}

// Handle what happens when you receive new messages
String chat_id = CHAT_ID;
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
            welcome += "/led_on to turn GPIO ON \n";
            welcome += "/led_off to turn GPIO OFF \n";
            welcome += "/state to request current GPIO state \n";
            bot.sendMessage(chat_id, welcome, "");
        }

        if (text == "/led_on")
        {
            bot.sendMessage(chat_id, "LED state set to ON", "");
            ledState = HIGH;
            digitalWrite(ledPin, ledState);
        }

        if (text == "/led_off")
        {
        
            bot.sendMessage(chat_id, "LED state set to OFF", "");
            ledState = LOW;
            digitalWrite(ledPin, ledState);
        }

        
        if (text == "/led_warn")
        {
            bot.sendMessage(chat_id, "Blinking LED ON to scare!", "");
            ledX = 10;
            while(ledX--){
               ledState = HIGH;
               digitalWrite(ledPin, ledState);
               delay(500);
               ledState = LOW;
               digitalWrite(ledPin, ledState);
               delay(500);
            }
           
        }

        if (text == "/state")
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

        
    }
}

void setup()
{
    Serial.begin(115200);

#ifdef ESP8266
    configTime(0, 0, "pool.ntp.org"); // get UTC time via NTP
    client.setTrustAnchors(&cert);    // Add root certificate for api.telegram.org
#endif

    pinMode(ledPin, OUTPUT);
    digitalWrite(ledPin, ledState);

    // Connect to Wi-Fi
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
#ifdef ESP32
    client.setCACert(TELEGRAM_CERTIFICATE_ROOT); // Add root certificate for api.telegram.org
#endif
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(1000);
        Serial.println("Connecting to WiFi..");
    }
    Serial.println(WiFi.localIP());
    pinMode(motionSensor, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(motionSensor), detectsMovement, RISING);
}

void loop()
{
    now = millis();
    if (millis() > lastTimeBotRan + botRequestDelay)
    {
        int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
        if (startTimer && (now - lastTrigger > (timeSeconds * 10000)))
        {
            bot.sendMessage(chat_id, "Chor Aya, Chor Aya!!", "");
            startTimer = false;
            Serial.println("Motion Detected!");
            Serial.println("LED ON");
            String welcome = "Intruder Sensed!!! What do you want to do.\n\n";
            welcome += "/led_warn to blink LED and warn people!\n";
            welcome += "/led_off to blink LED and warn people!\n";
            welcome += "/buz_warn to turn on buzzer to warn! \n";
            bot.sendMessage(chat_id, welcome, "");
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
