#include "Arduino.h"

#ifndef HEADER_WIFIINFO
  #define HEADER_WIFIINFO
   
  class WifiInfo{
    public:
      String ssid(void);
      String password(void);
  };
   
#endif
