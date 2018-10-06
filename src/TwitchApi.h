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

#ifndef TwitchApi_h
#define TwitchApi_h

#include <Arduino.h>
#include <ArduinoJson.h>
#include <Client.h>

#define TWITCH_HOST "api.twitch.tv"
// Fingerprint correct as of October 6th 2018
#define TWITCH_FINGERPRINT "BC 73 A5 9C 6E EE 38 43 A6 37 FC 32 CF 08 16 DC CF F1 5A 66"
#define TWITCH_TIMEOUT 1500

struct UserData
{
    char *id;
    char *login;
    char *displayName;
    char *type;
    char *broadCasterType;
    char *description;
    char *profileImageUrl;
    char *offlineImageUrl;
    long viewCount;
};

class TwitchApi
{
  public:
    TwitchApi(Client &client, char *clientId);
    bool makeGetRequestWithClientId(char *command);
    UserData getUserData(char *loginName);
    int portNumber = 443;
    //bool _checkFingerPrint = true; //Fail request if fingerprint doesnt match
    bool _debug = false;
    Client *client;

  private:
    char *_clientId;
};

#endif