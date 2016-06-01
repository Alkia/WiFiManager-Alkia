/**************************************************************
   WiFiManager is a library for the ESP8266/Arduino platform
   (https://github.com/esp8266/Arduino) to enable easy
   configuration and reconfiguration of WiFi credentials using a Captive Portal
   inspired by:
   http://www.esp8266.com/viewtopic.php?f=29&t=2520
   https://github.com/chriscook8/esp-arduino-apboot
   https://github.com/esp8266/Arduino/tree/esp8266/hardware/esp8266com/esp8266/libraries/DNSServer/examples/CaptivePortalAdvanced
   Built by AlexT https://github.com/tzapu
   Licensed under MIT license
 **************************************************************/

#ifndef WiFiManager_h
#define WiFiManager_h

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <memory>

extern "C" {
  #include "user_interface.h"
}

const char HTTP_HEAD[] PROGMEM            = "<!DOCTYPE html><html lang=\"en\"><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1, user-scalable=no\"/><title>{v}</title>";
const char HTTP_STYLE[] PROGMEM           = "<style>.c{text-align: center;} div,input{padding:5px;font-size:1em;} input{width:95%;} body{text-align: center;font-family:verdana;} button{border:0;border-radius:0.3rem;background-color:#1fa3ec;color:#fff;line-height:2.4rem;font-size:1.2rem;width:100%;} .q{float: right;width: 64px;text-align: right;} .l{background: url(\"data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAACAAAAAgCAMAAABEpIrGAAAALVBMVEX///8EBwfBwsLw8PAzNjaCg4NTVVUjJiZDRUUUFxdiZGSho6OSk5Pg4eFydHTCjaf3AAAAZElEQVQ4je2NSw7AIAhEBamKn97/uMXEGBvozkWb9C2Zx4xzWykBhFAeYp9gkLyZE0zIMno9n4g19hmdY39scwqVkOXaxph0ZCXQcqxSpgQpONa59wkRDOL93eAXvimwlbPbwwVAegLS1HGfZAAAAABJRU5ErkJggg==\") no-repeat left center;background-size: 1em;}</style>";
const char HTTP_SCRIPT[] PROGMEM          = "<script>function c(l){document.getElementById('s').value=l.innerText||l.textContent;document.getElementById('p').focus();}</script>";
const char HTTP_HEAD_END[] PROGMEM        = "</head><body><center><img src='data:image/png;base64,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'/></center><div style='text-align:left;display:inline-block;min-width:260px;'>";
const char HTTP_PORTAL_OPTIONS[] PROGMEM  = "<form action=\"/wifi\" method=\"get\"><button>Configure WiFi</button></form><br/><form action=\"/0wifi\" method=\"get\"><button>Configure WiFi (No Scan)</button></form><br/><form action=\"/i\" method=\"get\"><button>Info</button></form><br/><form action=\"/r\" method=\"post\"><button>Reset</button></form>";
const char HTTP_ITEM[] PROGMEM            = "<div><a href='#p' onclick='c(this)'>{v}</a>&nbsp;<span class='q {i}'>{r}%</span></div>";
const char HTTP_FORM_START[] PROGMEM      = "<form method='get' action='wifisave'><input id='s' name='s' length=32 placeholder='SSID'><br/><input id='p' name='p' length=64 type='password' placeholder='password'><br/>";
const char HTTP_FORM_PARAM[] PROGMEM      = "<br/><input id='{i}' name='{n}' length={l} placeholder='{p}' value='{v}' {c}>";
const char HTTP_FORM_END[] PROGMEM        = "<br/><button type='submit'>save</button></form>";
const char HTTP_SCAN_LINK[] PROGMEM       = "<br/><div class=\"c\"><a href=\"/wifi\">Scan</a></div>";
const char HTTP_SAVED[] PROGMEM           = "<div>Credentials Saved<br />Trying to connect...<br /><br />If it fails reconnect to the wifi THE-BOX to try again<br /><br />Note: This WiFi THE-BOX only appears when the box cannot connect with the credential provided<br /><br />Now you can connect back to your favourite WiFi!</div>";
const char HTTP_END[] PROGMEM             = "</div></body></html>";

#define WIFI_MANAGER_MAX_PARAMS 10

class WiFiManagerParameter {
  public:
    WiFiManagerParameter(const char *custom);
    WiFiManagerParameter(const char *id, const char *placeholder, const char *defaultValue, int length);
    WiFiManagerParameter(const char *id, const char *placeholder, const char *defaultValue, int length, const char *custom);

    const char *getID();
    const char *getValue();
    const char *getPlaceholder();
    int         getValueLength();
    const char *getCustomHTML();
  private:
    const char *_id;
    const char *_placeholder;
    char       *_value;
    int         _length;
    const char *_customHTML;

    void init(const char *id, const char *placeholder, const char *defaultValue, int length, const char *custom);

    friend class WiFiManager;
};


class WiFiManager
{
  public:
    WiFiManager();

    boolean       autoConnect();
    boolean       autoConnect(char const *apName, char const *apPassword = NULL);

    //if you want to always start the config portal, without trying to connect first
    boolean       startConfigPortal(char const *apName, char const *apPassword = NULL);

    // get the AP name of the config portal, so it can be used in the callback
    String        getConfigPortalSSID();

    void          resetSettings();

    //sets timeout before webserver loop ends and exits even if there has been no setup.
    //usefully for devices that failed to connect at some point and got stuck in a webserver loop
    //in seconds setConfigPortalTimeout is a new name for setTimeout
    void          setConfigPortalTimeout(unsigned long seconds);
    void          setTimeout(unsigned long seconds);

    //sets timeout for which to attempt connecting, usefull if you get a lot of failed connects
    void          setConnectTimeout(unsigned long seconds);


    void          setDebugOutput(boolean debug);
    //defaults to not showing anything under 8% signal quality if called
    void          setMinimumSignalQuality(int quality = 8);
    //sets a custom ip /gateway /subnet configuration
    void          setAPStaticIPConfig(IPAddress ip, IPAddress gw, IPAddress sn);
    //sets config for a static IP
    void          setSTAStaticIPConfig(IPAddress ip, IPAddress gw, IPAddress sn);
    //called when AP mode and config portal is started
    void          setAPCallback( void (*func)(WiFiManager*) );
    //called when settings have been changed and connection was successful
    void          setSaveConfigCallback( void (*func)(void) );
    //adds a custom parameter
    void          addParameter(WiFiManagerParameter *p);
    //if this is set, it will exit after config, even if connection is unsucessful.
    void          setBreakAfterConfig(boolean shouldBreak);
    //if this is set, try WPS setup when starting (this will delay config portal for up to 2 mins)
    //TODO
    //if this is set, customise style
    void          setCustomHeadElement(const char* element);
    //if this is true, remove duplicated Access Points - defaut true
    void          setRemoveDuplicateAPs(boolean removeDuplicates);

  private:
    std::unique_ptr<DNSServer>        dnsServer;
    std::unique_ptr<ESP8266WebServer> server;

    //const int     WM_DONE                 = 0;
    //const int     WM_WAIT                 = 10;

    //const String  HTTP_HEAD = "<!DOCTYPE html><html lang=\"en\"><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"/><title>{v}</title>";

    void          setupConfigPortal();
    void          startWPS();

    const char*   _apName                 = "no-net";
    const char*   _apPassword             = NULL;
    String        _ssid                   = "";
    String        _pass                   = "";
    unsigned long _configPortalTimeout    = 0;
    unsigned long _connectTimeout         = 0;
    unsigned long _configPortalStart      = 0;

    IPAddress     _ap_static_ip;
    IPAddress     _ap_static_gw;
    IPAddress     _ap_static_sn;
    IPAddress     _sta_static_ip;
    IPAddress     _sta_static_gw;
    IPAddress     _sta_static_sn;

    int           _paramsCount            = 0;
    int           _minimumQuality         = -1;
    boolean       _removeDuplicateAPs     = true;
    boolean       _shouldBreakAfterConfig = false;
    boolean       _tryWPS                 = false;

    const char*   _customHeadElement      = "";

    //String        getEEPROMString(int start, int len);
    //void          setEEPROMString(int start, int len, String string);

    int           status = WL_IDLE_STATUS;
    int           connectWifi(String ssid, String pass);
    uint8_t       waitForConnectResult();

    void          handleRoot();
    void          handleWifi(boolean scan);
    void          handleWifiSave();
    void          handleInfo();
    void          handleReset();
    void          handleNotFound();
    void          handle204();
    boolean       captivePortal();

    // DNS server
    const byte    DNS_PORT = 53;

    //helpers
    int           getRSSIasQuality(int RSSI);
    boolean       isIp(String str);
    String        toStringIp(IPAddress ip);

    boolean       connect;
    boolean       _debug = true;

    void (*_apcallback)(WiFiManager*) = NULL;
    void (*_savecallback)(void) = NULL;

    WiFiManagerParameter* _params[WIFI_MANAGER_MAX_PARAMS];

    template <typename Generic>
    void          DEBUG_WM(Generic text);

    template <class T>
    auto optionalIPFromString(T *obj, const char *s) -> decltype(  obj->fromString(s)  ) {
      return  obj->fromString(s);
    }
    auto optionalIPFromString(...) -> bool {
      DEBUG_WM("NO fromString METHOD ON IPAddress, you need ESP8266 core 2.1.0 or newer for Custom IP configuration to work.");
      return false;
    }
};

#endif
