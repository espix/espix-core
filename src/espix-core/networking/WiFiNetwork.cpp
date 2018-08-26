#include "WiFiNetwork.h"

#include <Schedule.h>

#include "../applications/Application.h"
#include "../timing/TimeClient.h"

WiFiNetworkClass::WiFiNetworkClass() {
}

bool WiFiNetworkClass::isConnected() {
  return WiFi.status() == WL_CONNECTED;
}

wl_status_t WiFiNetworkClass::getStatus() {
  return WiFi.status();
}

String WiFiNetworkClass::getLocalIP() {
  return WiFi.localIP().toString();
}

void WiFiNetworkClass::connect(WiFiConnectionSetting settings[], int settingsCount,
                               NetworkConnectionCallback callback) {
  static WiFiEventHandler handler = WiFi.onStationModeGotIP([=](const WiFiEventStationModeGotIP e) {
    _connecting = false;
    handler = NULL;
    WiFi.onStationModeGotIP(NULL);
    schedule_function([=]() {
      TimeClient.begin();
      if (callback) {
        callback();
      }
    });
  });

  WiFi.mode(WIFI_STA);
  for (int i = 0; i < settingsCount; i++) {
    auto setting = settings[i];
    _wifiMulti.addAP(setting.ssid.c_str(), setting.password.c_str());
  }
  _wifiMulti.run();
  _connecting = true;
}

void WiFiNetworkClass::disconnect(bool wifiOff) {
  WiFi.disconnect(wifiOff);
}

void WiFiNetworkClass::update() {
  if (_connecting) {
    if (millis() - _lastUpdate > 200) {
      _lastUpdate = millis();
      _wifiMulti.run();
    }
  }
}

WiFiNetworkClass WiFiNetwork;
