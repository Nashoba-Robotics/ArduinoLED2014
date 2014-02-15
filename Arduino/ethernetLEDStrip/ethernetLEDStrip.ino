/* READ ME!!! ---> THIS IS VERY IMPORTANT
you must click "Sketch" then "Import Library" then "add library"
in the window that opened, you must navigate to the "lib" directory
in this project, then add the "PololuLedStrip" library.
*/
#include <PololuLedStrip.h>
#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>

#define LED_COUNT 60
#define CMD_SET 0
#define CMD_PTRN 1//<-- NOT FINISHED

#define PTRN_RAINBOW 0//<-- NOT FINISHED
#define PTRN_LRBOUNCE 1//<-- NOT FINISHED
#define PTRN_INOUTBOUNCE 2//<-- NOT FINISHED

PololuLedStrip<3> ledStrip;//<6> on UNO, Leonardo, and Duemilanove, <3> on Mega
rgb_color colors[LED_COUNT];
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
IPAddress ip(192, 168, 1, 177);
unsigned int localPort = 8888;
char packetBuffer[UDP_TX_PACKET_MAX_SIZE];
EthernetUDP Udp;
int w = LED_COUNT;
String pbs = "";
int pbCmd = -1;
int pbData = -1;
String delimiter = " ";

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

rgb_color rgbColor(uint16_t r, uint16_t g, uint16_t b) {
  return (rgb_color) {r, g, b};
}

void setup() {
  Ethernet.begin(mac, ip);
  Udp.begin(localPort);
  for(uint16_t i = 0; i < LED_COUNT; i++) {
    colors[i] = rgbColor(84, 255, 0);
  }
  delay(10);
}

void loop() {
  int packetSize = Udp.parsePacket();
  if(packetSize) {
    Udp.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE);
    pbs = String(packetBuffer);
    procInput(pbs);
    updateLED();
  }
  delay(20);
}

void procInput(String input) {
  pbCmd = cmdFromString(pbs);
  pbData = dataFromString(pbs);
  procCmd(pbCmd, pbData);
  Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
  Udp.write("Command received: ");
  Udp.write(pbCmd);
  Udp.write("Data received: ");
  Udp.write(pbData);
  Udp.endPacket();
}

void procCmd(int cmd, int data) {
  switch(cmd) {
    case CMD_SET:
      w = data;
      if(w < 0) {
        w = 0;
      } else if(w >= LED_COUNT) {
        w = LED_COUNT;
      }
      for(uint16_t i = 0; i < LED_COUNT; i++) {
        if(i < w) {
          colors[i] = rgbColor(84, 255, 0);//used to use hsvToRgb
        } else {
          colors[i] = rgbColor(0, 0, 0);
        }
      }
      break;
    case CMD_PTRN:
      break;
  }
}

int cmdFromString(String input) {
  String cmd = input.substring(0, input.indexOf(delimiter));
  return cmd.toInt();
}

int dataFromString(String input) {
  String data = input.substring(input.indexOf(delimiter) + 1, input.length());
  return data.toInt();
}

void updateLED() {
  ledStrip.write(colors, LED_COUNT);
}
