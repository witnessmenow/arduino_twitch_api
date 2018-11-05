/*******************************************************************
 *  Get User Data and follower data for a given twitch user
 *
 *  By Brian Lough
 *  https://www.youtube.com/channel/UCezJOfu7OtqGzd5xrP3q6WA
 *******************************************************************/

#include <TwitchApi.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

#include <ArduinoJson.h> // This Sketch doesn't technically need this, but the library does so it must be installed.

//------- Replace the following! ------
char ssid[] = "SSID";         // your network SSID (name)
char password[] = "password"; // your network key

// Create a new application on https://dev.twitch.tv/
#define TWITCH_CLIENT_ID "1234567890654rfsc"

// Username of who you are getting the data for (e.g. "ninja")
#define TWITCH_LOGIN "brianlough"

WiFiClientSecure client;
TwitchApi twitch(client, TWITCH_CLIENT_ID);

unsigned long delayBetweenRequests = 60000; // Time between requests (1 minute)
unsigned long requestDueTime;               //time when request due

void setup()
{

    Serial.begin(115200);

    // kss._debug = true;

    // Set WiFi to station mode and disconnect from an AP if it was Previously
    // connected
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);

    // Attempt to connect to Wifi network:
    Serial.print("Connecting Wifi: ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(500);
    }
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    IPAddress ip = WiFi.localIP();
    Serial.println(ip);
}

void loop()
{

    if (millis() > requestDueTime)
    {
        Serial.print("Free Heap: ");
        Serial.println(ESP.getFreeHeap());

        Serial.print("Getting Stream info for:");
        Serial.println(TWITCH_LOGIN);
        StreamInfo stream = twitch.getStreamInfo(TWITCH_LOGIN);
        if(!stream.error){
            Serial.println("---------Stream Info ---------");

            Serial.print("Id: ");
            Serial.println(stream.id);

            Serial.print("User Id: ");
            Serial.println(stream.userId);

            Serial.print("User Name: ");
            Serial.println(stream.userName);

            Serial.print("Game Id: ");
            Serial.println(stream.gameId);

            Serial.print("Type: ");
            Serial.println(stream.type);

            Serial.print("Title: ");
            Serial.println(stream.title);

            Serial.print("Viewer Count: ");
            Serial.println(stream.viewerCount);

            Serial.print("Started At: ");
            Serial.println(stream.startedAt);

            Serial.print("Language: ");
            Serial.println(stream.language);

            Serial.print("Thumbnail URL: ");
            Serial.println(stream.thumbnailUrl);
            Serial.println("------------------------");
        }

        requestDueTime = millis() + delayBetweenRequests;
    }
}