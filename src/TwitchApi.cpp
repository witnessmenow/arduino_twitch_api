/*
   Copyright (c) 2018 Brian Lough. All right reserved.

   TwitchApi - An Arduino library to wrap the Twitch API

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include "TwitchApi.h"

TwitchApi::TwitchApi(Client &client, char *clientId)
{
    this->client = &client;
    this->_clientId = clientId;
}

bool TwitchApi::makeGetRequestWithClientId(char *command)
{

    client->setTimeout(5000);
    if (!client->connect(TWITCH_HOST, portNumber))
    {
        Serial.println(F("Connection failed"));
        return false;
    }

    //Serial.println(F("Connected!"));

    // Default client doesnt have a verify, need to figure something else out.
    // if (_checkFingerPrint && !client->verify(TWITCH_FINGERPRINT, TWITCH_HOST))
    // {
    //     Serial.println(F("certificate doesn't match"));
    //     return false;
    // }

    // give the esp a breather
    yield();

    // Send HTTP request
    client->print(F("GET "));
    client->print(command);
    client->println(F(" HTTP/1.1"));

    //Headers
    client->print(F("Host: "));
    client->println(TWITCH_HOST);

    client->print(F("Client-ID: "));
    client->println(_clientId);

    client->println(F("Cache-Control: no-cache"));

    if (client->println() == 0)
    {
        Serial.println(F("Failed to send request"));
        return false;
    }

    // Check HTTP status
    char status[32] = {0};
    client->readBytesUntil('\r', status, sizeof(status));
    if (strcmp(status, "HTTP/1.1 200 OK") != 0)
    {
        Serial.print(F("Unexpected response: "));
        Serial.println(status);
        return false;
    }

    // Skip HTTP headers
    char endOfHeaders[] = "\r\n\r\n";
    if (!client->find(endOfHeaders))
    {
        Serial.println(F("Invalid response"));
        return false;
    }

    // Let the caller of this method parse the JSon from the client
    return true;
}

UserData TwitchApi::getUserData(char *loginName)
{
    char command[100] = "/helix/users?login=";
    strcat(command, loginName);
    if (_debug) {
        Serial.println(command);
    }

    // Use arduinojson.org/assistant to compute the capacity.
    const size_t bufferSize = JSON_ARRAY_SIZE(1) + JSON_OBJECT_SIZE(1) + JSON_OBJECT_SIZE(9) + 1000;

    UserData user;
    user.error = true;
    if (makeGetRequestWithClientId(command))
    {
        // Allocate JsonBuffer
        DynamicJsonDocument root(bufferSize);

        // Parse JSON object
        DeserializationError error = deserializeJson(root, *client);
        if (error)
        {   
            Serial.print(F("getUserData deserializeJson() failed: "));
            Serial.println(error.c_str());
        }
        else
        {
            // clang-format off
            JsonObject data0 = root["data"][0];
            user.id = (char *)data0["id"].as<char*>();
            user.login = (char *)data0["login"].as<char*>();
            user.displayName = (char *)data0["display_name"].as<char*>();
            user.type = (char *)data0["type"].as<char*>();
            user.broadCasterType = (char *)data0["broadcaster_type"].as<char*>();
            user.description = (char *)data0["description"].as<char*>();
            user.profileImageUrl = (char *)data0["profile_image_url"].as<char*>();
            user.offlineImageUrl = (char *)data0["offline_image_url"].as<char*>();
            user.viewCount = data0["view_count"].as<long>();
            user.error = false;
            // clang-format on
        }

    }
    closeClient();
    return user;
}

FollowerData TwitchApi::getFollowerData(char *id)
{
    char command[100] = "/helix/users/follows?to_id=";
    strcat(command, id);
    strcat(command, "&first=1");
    if (_debug) {
        Serial.println(command);
    }

    // Use arduinojson.org/assistant to compute the capacity.
    const size_t bufferSize = JSON_ARRAY_SIZE(1) + JSON_OBJECT_SIZE(1) + JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(5) + 1000;

    FollowerData follower;
    follower.error = true;
    if (makeGetRequestWithClientId(command))
    {
        // Allocate JsonBuffer
        DynamicJsonDocument root(bufferSize);

        // Parse JSON object
        DeserializationError error = deserializeJson(root, *client);
        if (error)
        {   
            Serial.print(F("getFollowerData deserializeJson() failed: "));
            Serial.println(error.c_str());
        }
        else
        {
            // clang-format off
            follower.total = root["total"].as<long>();
            JsonObject data0 = root["data"][0];
            follower.fromId = (char *)data0["from_id"].as<char*>();
            follower.fromName = (char *)data0["from_name"].as<char*>();
            follower.toId = (char *)data0["to_id"].as<char*>();
            follower.toName = (char *)data0["to_name"].as<char*>();
            follower.followedAt = (char *)data0["followed_at"].as<char*>();
            follower.error = false;
            // clang-format on
        }
    }

    closeClient();
    return follower;
}

StreamInfo TwitchApi::getStreamInfo(char *loginName)
{
    char command[100] = "/helix/streams?user_login=";
    strcat(command, loginName);
    strcat(command, "&first=1");
    if (_debug) {
        Serial.println(command);
    }

    // Use arduinojson.org/assistant to compute the capacity.
    const size_t bufferSize = JSON_ARRAY_SIZE(1) + JSON_ARRAY_SIZE(3) + JSON_OBJECT_SIZE(1) + JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(11) + 1000;

    StreamInfo stream;
    stream.error = true;
    if (makeGetRequestWithClientId(command))
    {
        // Allocate JsonBuffer
        DynamicJsonDocument root(bufferSize);

        // Parse JSON object
        DeserializationError error = deserializeJson(root, *client);
        if (error)
        {   
            Serial.print(F("getStreamInfo deserializeJson() failed: "));
            Serial.println(error.c_str());
        }
        else
        {
            if(root["data"].size() > 0){
                // clang-format off
                JsonObject data0 = root["data"][0];
                stream.id = (char *)data0["id"].as<char*>();
                stream.userId = (char *)data0["user_id"].as<char*>();
                stream.userName = (char *)data0["user_name"].as<char*>();
                stream.gameId = (char *)data0["game_id"].as<char*>();
                stream.type = (char *)data0["type"].as<char*>();

                stream.title = (char *)data0["title"].as<char*>();
                stream.viewerCount = data0["viewer_count"].as<long>();
                stream.startedAt = (char *)data0["started_at"].as<char*>();
                stream.language = (char *)data0["language"].as<char*>();
                stream.thumbnailUrl = (char *)data0["thumbnail_url"].as<char*>();
                stream.error = false;
                // clang-format on
            } else {
                Serial.println(F("No Streams found!"));
            }
        }
    }

    closeClient();
    return stream;
}

void TwitchApi::closeClient() {
    if (client->connected()) {
      if (_debug) {
        Serial.println(F("Closing client"));
      }
      client->stop();
    }
  }