/* READ ME!!! ---> THIS IS VERY IMPORTANT
you must click "Sketch" then "Import Library" then "add library"
in the window that opened, you must navigate to the "lib" directory
in this project, then add the "PololuLedStrip" library.
*/
#include <PololuLedStrip.h>
#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>

PololuLedStrip<3> ledStrip;//6 on reg, 3 on mega
#define LED_COUNT 60
rgb_color colors[LED_COUNT];
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
IPAddress ip(192, 168, 1, 177);
unsigned int localPort = 8888;
char packetBuffer[UDP_TX_PACKET_MAX_SIZE];
char replyBuffer[] = "acknowledged";
EthernetUDP Udp;
int w = 60;

rgb_color hsvToRgb(uint16_t h, uint8_t s, uint8_t v) {
  uint8_t f = (h % 60) * 255 / 60;
  uint8_t p = v * (255 - s) / 255;
  uint8_t q = v * (255 - f * s / 255) / 255;
  uint8_t t = v * (255 - (255 - f) * s / 255) / 255;
  uint8_t r = 0, g = 0, b = 0;
  switch((h / 60) % 6) {
    case 0: r = v; g = t; b = p; break;
    case 1: r = q; g = v; b = p; break;
    case 2: r = p; g = v; b = t; break;
    case 3: r = p; g = q; b = v; break;
    case 4: r = t; g = p; b = v; break;
    case 5: r = v; g = p; b = q; break;
  }
  return (rgb_color) {r, g, b};
}

void setup() {
  Ethernet.begin(mac, ip);
  Udp.begin(localPort);
  for(uint16_t i = 0; i < LED_COUNT; i++) {
    colors[i] = hsvToRgb(100, 255, 255);
  }
  ledStrip.write(colors, LED_COUNT);
  delay(50);
}

void loop() {
  int packetSize = Udp.parsePacket();
  if(packetSize) {
    Udp.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE);
    String pbs(packetBuffer);
    int pbi = pbs.toInt();
    w = pbi;
    if(w < 0) {
      w = 0;
    } else if(w >= LED_COUNT) {
      w = LED_COUNT;
    }
    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    Udp.write("Setting to: ");
    Udp.write(packetBuffer);
    Udp.endPacket();
    for(uint16_t i = 0; i < LED_COUNT; i++) {
      if(i < w) {
        colors[i] = hsvToRgb(100, 255, 255);
      } else {
        colors[i] = hsvToRgb(0, 0, 0);
      }
    }
    ledStrip.write(colors, LED_COUNT);
  }
  delay(20);
}
