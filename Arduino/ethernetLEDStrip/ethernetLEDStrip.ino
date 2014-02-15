/* READ ME!!! ---> THIS IS VERY IMPORTANT
 you must click "Sketch" then "Import Library" then "add library"
 in the window that opened, you must navigate to the "lib" directory
 in this project, then add the "PololuLedStrip" library.
 */
#include <PololuLedStrip.h>
#include <SPI.h>
#include <Ethernet.h>

#define LED_COUNT 60
#define CMD_SET 0
#define CMD_PTRN 1//<-- NOT FINISHED

#define PTRN_RAINBOW 0//<-- NOT FINISHED
#define PTRN_LRBOUNCE 1//<-- NOT FINISHED
#define PTRN_INOUTBOUNCE 2//<-- NOT FINISHED

PololuLedStrip<6> ledStrip;//<6> on UNO, Leonardo, and Duemilanove, <3> on Mega
rgb_color colors[LED_COUNT];
rgb_color blank[LED_COUNT];

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(10, 17, 68, 12);
unsigned int localPort = 8888;
EthernetServer server(localPort);
int w = LED_COUNT;
String input = "";
int pbCmd = -1;
int pbData1 = -1;
int pbData2 = -1;
String delimiter = " ";
String seperator = ":";
String inputString;

EthernetClient client;

rgb_color hsvToRgb(uint16_t h, uint8_t s, uint8_t v) {
  uint8_t f = (h % 60) * 255 / 60;
  uint8_t p = v * (255 - s) / 255;
  uint8_t q = v * (255 - f * s / 255) / 255;
  uint8_t t = v * (255 - (255 - f) * s / 255) / 255;
  uint8_t r = 0, g = 0, b = 0;
  switch((h / 60) % 6) {
  case 0: 
    r = v; 
    g = t; 
    b = p; 
    break;
  case 1: 
    r = q; 
    g = v; 
    b = p; 
    break;
  case 2: 
    r = p; 
    g = v; 
    b = t; 
    break;
  case 3: 
    r = p; 
    g = q; 
    b = v; 
    break;
  case 4: 
    r = t; 
    g = p; 
    b = v; 
    break;
  case 5: 
    r = v; 
    g = p; 
    b = q; 
    break;
  }
  return (rgb_color) {
    r, g, b  };
}

rgb_color rgbColor(uint16_t r, uint16_t g, uint16_t b) {
  return (rgb_color) {
    r, g, b  };
}

void setup() {
  for(uint16_t i = 0; i < LED_COUNT; i++) {
    blank[i] = rgbColor(0, 0, 0);
  }
  ledStrip.write(blank, LED_COUNT);
  Serial.begin(9600);
  Ethernet.begin(mac, ip);
  server.begin();
  
  Serial.println("Listening for clients");

  for(uint16_t i = 0; i < LED_COUNT; i++) {
    colors[i] = rgbColor(255, 255, 0);
  }
  delay(10);
}

void loop() {
  client = server.available();
  char inputCharArray[100];
  if (client) {
    int i = 0;
    while(client.available() > 0){
      inputCharArray[i] = client.read();
      i++;
    }
    inputString = String(inputCharArray);
    //Serial.print("Received: ");
    Serial.println(inputCharArray);
    procInput(inputString);
    updateLED();
  }
  else 
  {
  }
}

void procInput(String input) {
  pbCmd = cmdFromString(inputString);
  pbData1 = data1FromString(inputString);
  pbData2 = data2FromString(inputString);
  procCmd(pbCmd, pbData1, pbData2);
  /*Serial.print("Command received: ");
  Serial.println(pbCmd);
  Serial.print("Data received: ");
  Serial.println(pbData);*/
}

void procCmd(int cmd, int data1, int data2) {
  switch(cmd) {
  case CMD_SET:
    setLED(data1, data2, rgbColor(255,0,255));
    break;
  case CMD_PTRN:
    rainbow(data1, data2);
    break;
  }
}

void rainbow(int data1, int numberOfLoops)
{
  for(int j = 0; j < numberOfLoops; j++){
  uint16_t time = millis() >> 2;
  for(uint16_t i = 0; i < LED_COUNT; i++)
  {
    byte x = (time >> 2) - (i << 3);
    colors[i] = hsvToRgb((uint32_t)x * 359 / 256, 255, 255);
  }
  updateLED();
  }
}

int cmdFromString(String input) {
  String cmd = input.substring(0, input.indexOf(delimiter));
  return cmd.toInt();
}

int data1FromString(String input) {
  String data = input.substring(input.indexOf(delimiter) + 1, input.indexOf(seperator));
  return data.toInt();
}

int data2FromString(String input) {
  String data = input.substring(input.indexOf(seperator) + 1, input.length());
  return data.toInt();
}

void setLED(int startPosition, int endPosition, rgb_color rgbcolor) {
  for(uint16_t i = 0; i < LED_COUNT; i++) {
    colors[i] = rgbColor(0, 0, 0);
  }
  for(uint16_t i = startPosition; i < endPosition; i++) {
    if(i < w) {
      colors[i] = rgbcolor;
    } 
    else {
      colors[i] = rgbColor(0, 0, 0);
    }
  }
}

void updateLED() {
  ledStrip.write(colors, LED_COUNT);
}

