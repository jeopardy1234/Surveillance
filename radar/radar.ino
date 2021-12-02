#include "Servo.h"
#include "WiFi.h"
#include "HTTPClient.h"
#include "time.h"
#include <ArduinoJson.h>
int array[7] = {0};
static const int servoPin = 18;
#define TRIGGER_PIN 4
#define ECHO_PIN 5
#define SOUND_SPEED 0.034
Servo servo1;
//Global variables
long time_taken;  // stores the time taken for the wave to come back
float dist_in_cm; // will store the value of distance in centimeters
char *ssid = "";
char *password = "";
String cse_ip = ""; // YOUR IP from ipconfig/ifconfig
String cse_port = "8080";
String server = "http://" + cse_ip + ":" + cse_port + "/~/in-cse/in-name/";
String ae = "Radar";
String cn[7] = {"dist_0_deg", "dist_30_deg", "dist_60_deg", "dist_90_deg", "dist_120_deg", "dist_150_deg", "dist_180_deg"};

void createCI()
{
    String val;
    HTTPClient http;
    for (int i = 0; i < 7; i++)
    {
        val = "";
        val.concat(array[i]);
        http.begin(server + ae + "/" + cn[i] + "/");
        http.addHeader("X-M2M-Origin", "admin:admin");
        http.addHeader("Content-Type", "application/json;ty=4");
        int code = http.POST("{\"m2m:cin\": {\"cnf\": \"application/json\",\"con\": " + String(val) + "}}");
        Serial.println(code);
        if (code == -1)
        {
            Serial.println("UNABLE TO CONNECT TO THE SERVER");
        }
    }
    http.end();
}

void setup()
{
    Serial.begin(115200);
    pinMode(ECHO_PIN, INPUT);
    pinMode(TRIGGER_PIN, OUTPUT);
    WiFi.mode(WIFI_STA);

    Serial.print("Connecting");
    while (WiFi.status() != WL_CONNECTED)
    {
        WiFi.begin(ssid, password);
        delay(5000);
        Serial.print("...\n");
    }
    Serial.print("Connected successfully!\n");
    servo1.attach(servoPin);
}
void print_radar()
{
    for (int i = 0; i < 7; i++)
    {
        Serial.print(array[i]);
        Serial.print("\t");
    }
    Serial.println(" ");
}
void loop()
{
    delay(1000);
    for (int deg = 0; deg <= 180; deg += 30)
    {
        servo1.write(deg);
        digitalWrite(TRIGGER_PIN, LOW);
        delayMicroseconds(2);

        //Emit ultrasound starts
        digitalWrite(TRIGGER_PIN, HIGH);
        delayMicroseconds(10);
        digitalWrite(TRIGGER_PIN, LOW);
        // Emiitting ultrasound ends

        time_taken = pulseIn(ECHO_PIN, HIGH); // timetaken now has the time taken for the wave to come back

        dist_in_cm = time_taken * SOUND_SPEED / 2; // distance to speed * time
        array[deg / 30] = dist_in_cm;
        print_radar();
        delay(2000);
    }

    for (int deg = 180; deg >= 0; deg -= 30)
    {
        servo1.write(deg);
        digitalWrite(TRIGGER_PIN, LOW);
        delayMicroseconds(2);

        //Emit ultrasound starts
        digitalWrite(TRIGGER_PIN, HIGH);
        delayMicroseconds(10);
        digitalWrite(TRIGGER_PIN, LOW);
        // Emiitting ultrasound ends

        time_taken = pulseIn(ECHO_PIN, HIGH); // timetaken now has the time taken for the wave to come back

        dist_in_cm = time_taken * SOUND_SPEED / 2; // distance to speed * time
        array[deg / 30] = dist_in_cm;
        print_radar();
        delay(2000);
    }
    createCI();
}
