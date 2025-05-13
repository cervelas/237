#ifndef CONFIG_UDP
#define CONFIG_UDP

#define WRITE
#define DEBUG

#define WIFI_SSID "MynameisWifi"
#define WIFI_PASS "Machine2023"

#ifdef WRITE

#define REMOTE_IP "192.168.2.33"
#define REMOTE_PORT 5000
#define LOCAL_PORT 5000

#else

#define REMOTE_PORT 5001
#define LOCAL_PORT 5002

#endif

#endif
