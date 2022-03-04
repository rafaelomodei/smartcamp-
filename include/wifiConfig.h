#ifndef _WIFI_CONFIG_H
#define _WIFI_CONFIG_H

#ifndef _ENV_H
    #include "env.h"
#endif

#include <Firebase_ESP_Client.h>
#include <WiFiManager.h>

void fcsUploadCallback(CFS_UploadStatusInfo info);
void configModeCallback(WiFiManager *myWiFiManager);
void saveConfigCallback();
void wifiStartConfigPortal(WiFiManager *wifiManager, const char *AP_NAME);
void wifiConnect();
int wifiAutoConnect(WiFiManager *wifiManager, const char *AP_NAME);

#endif