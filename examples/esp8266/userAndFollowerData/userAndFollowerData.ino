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

        Serial.print("Getting Data for: ");
        Serial.println(TWITCH_LOGIN);
        UserData user = twitch.getUserData(TWITCH_LOGIN);
        if(!user.error){
            Serial.println("---------User Details ---------");
            Serial.print("Id: ");
            Serial.println(user.id);
    
            Serial.print("Login: ");
            Serial.println(user.login);
    
            Serial.print("Display Name: ");
            Serial.println(user.displayName);
    
            Serial.print("Broadcaster Type: ");
            Serial.println(user.broadCasterType);
    
            Serial.print("Description: ");
            Serial.println(user.description);
    
            Serial.print("Profile Image URL: ");
            Serial.println(user.profileImageUrl);
    
            Serial.print("Offline Image URL: ");
            Serial.println(user.offlineImageUrl);
    
            Serial.print("View Count: ");
            Serial.println(user.viewCount);
            Serial.println("------------------------");
    
            // Follower data requires the user Id, this doesn't change
            // so you can hardcode it into the sketch after you get it once.
            // e.g. char *userId = "171235731";
    
            FollowerData followerData = twitch.getFollowerData(user.id);
            if(!followerData.error){
                Serial.println("---------Follower Data ---------");
            
                Serial.print("Number of Followers: ");
                Serial.println(followerData.total);
        
                Serial.print("Last Follower Id: ");
                Serial.println(followerData.fromId);
        
                Serial.print("Last Follower Name: ");
                Serial.println(followerData.fromName);
        
                Serial.print("Last Follower to Id: ");
                Serial.println(followerData.toId);
        
                Serial.print("Last Follower to Name: ");
                Serial.println(followerData.toName);
        
                Serial.print("Last Follower at: ");
                Serial.println(followerData.followedAt);
        
                Serial.println("------------------------");
            }
        }      

        requestDueTime = millis() + delayBetweenRequests;
    }
}