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
    const size_t bufferSize = JSON_ARRAY_SIZE(1) + JSON_OBJECT_SIZE(1) + JSON_OBJECT_SIZE(9) + 1000;

    UserData user;
    if (makeGetRequestWithClientId(command))
    {
        // Allocate JsonBuffer
        // Use arduinojson.org/assistant to compute the capacity.
        DynamicJsonBuffer jsonBuffer(bufferSize);

        // Parse JSON object
        JsonObject &root = jsonBuffer.parseObject(*client);
        if (root.success())
        {
            // clang-format off
            JsonObject &data0 = root["data"][0];
            user.id = (char *)data0["id"].as<char*>();
            user.login = (char *)data0["login"].as<char*>();
            user.displayName = (char *)data0["display_name"].as<char*>();
            user.type = (char *)data0["type"].as<char*>();
            user.broadCasterType = (char *)data0["broadcaster_type"].as<char*>();
            user.description = (char *)data0["description"].as<char*>();
            user.profileImageUrl = (char *)data0["profile_image_url"].as<char*>();
            user.offlineImageUrl = (char *)data0["offline_image_url"].as<char*>();
            user.viewCount = data0["view_count"].as<long>();
            // clang-format on
        } else {
            Serial.println(F("Parsing failed!")); 
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
    const size_t bufferSize = JSON_ARRAY_SIZE(1) + JSON_OBJECT_SIZE(1) + JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(5);

    FollowerData follower;
    if (makeGetRequestWithClientId(command))
    {
        // Allocate JsonBuffer
        // Use arduinojson.org/assistant to compute the capacity.
        DynamicJsonBuffer jsonBuffer(bufferSize);

        // Parse JSON object
        JsonObject &root = jsonBuffer.parseObject(*client);
        if (root.success())
        {
            // clang-format off
            follower.total = root["total"].as<long>();
            JsonObject &data0 = root["data"][0];
            follower.fromId = (char *)data0["from_id"].as<char*>();
            follower.fromName = (char *)data0["from_name"].as<char*>();
            follower.toId = (char *)data0["to_id"].as<char*>();
            follower.toName = (char *)data0["to_name"].as<char*>();
            follower.followedAt = (char *)data0["followed_at"].as<char*>();
            // clang-format on
        } else {
            Serial.println(F("Parsing failed!"));
        }
    }

    closeClient();
    return follower;
}

void TwitchApi::closeClient() {
    if (client->connected()) {
      if (_debug) {
        Serial.println(F("Closing client"));
      }
      client->stop();
    }
  }