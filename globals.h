enum CloudMode {
  IoTHub,
  EventHub
};

enum LedState {
  Off,
  On
};

enum DisplayMode {
  NoDisplay,
  LedMatrix
};

struct SensorData{
  float temperature;
  float humidity;
  int pressure;
  int light;
};

struct CloudConfig {
  CloudMode cloudMode = IoTHub;
  unsigned int publishRateInSeconds = 60; // defaults to once a minute
  unsigned int sasExpiryDate = 1737504000;  // Expires Wed, 22 Jan 2025 00:00:00 GMT
  const char *host;
  char *key;
  const char *id;
  const char *geo;
  unsigned long lastPublishTime = 0;
  String fullSas;
  String endPoint;
};

struct DeviceConfig {
  int WifiIndex = 0;
  unsigned long LastWifiTime = 0;
  int WiFiConnectAttempts = 0;
  int wifiPairs = 0;
  const char ** ssid;
  const char **pwd;
  DisplayMode displayMode = NoDisplay;    // DisplayMode enumeration: NoDisplay or LedMatrix
  unsigned int deepSleepSeconds = 0;      // Number of seconds for the ESP8266 chip to deepsleep for.  GPIO16 needs to be tied to RST to wake from deepSleep http://esp8266.github.io/Arduino/versions/2.0.0/doc/libraries.html
};


