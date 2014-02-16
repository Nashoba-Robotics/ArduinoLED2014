/* READ ME!!! ---> THIS IS VERY IMPORTANT
 you must click "Sketch" then "Import Library" then "add library"
 in the window that opened, you must navigate to the "lib" directory
 in this project, then add the "PololuLedStrip" library.
 */
#include <PololuLedStrip.h>
#include <SPI.h>
#include <Ethernet.h>

#define LED_COUNT 60
#define CMD_OFF 0
#define CMD_SET 1
#define CMD_PTRN 2

#define PTRN_RAINBOW 0
#define PTRN_LRBOUNCE 1//<-- NOT FINISHED
#define PTRN_INOUTBOUNCE 2//<-- NOT FINISHED

PololuLedStrip<6> ledStrip;//<6> on UNO, Leonardo, and Duemilanove, <3> on Mega
rgb_color colors[LED_COUNT];
rgb_color blank[LED_COUNT];

byte mac[] = { 
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
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
String state = "rainbow";
int moveSpeed = 10;

int startPosition = 0;
int endPosition = LED_COUNT;
rgb_color color = rgbColor(255,255,255);

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
    r, g, b               };
}

rgb_color rgbColor(uint16_t r, uint16_t g, uint16_t b) {
  return (rgb_color) {
    r, g, b                      };
}

void setup() {
  for(uint16_t i = 0; i < LED_COUNT; i++) {
    blank[i] = rgbColor(0, 0, 0);
  }
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
  if (client) {
    String inputString = communicate();
    pbCmd = inputString.substring(0, input.indexOf(delimiter)).toInt();
    pbData1 = inputString.substring(input.indexOf(delimiter) + 1, input.indexOf(seperator)).toInt();
    pbData2 = inputString.substring(input.indexOf(seperator) + 1, input.length()).toInt();
    procCmd(pbCmd, pbData1, pbData2);
  }
  if(state == "off")
  {
    setLED(0, LED_COUNT, rgbColor(0, 0, 0));
  }
  else{
    if(state == "rainbow")
    {
      rainbow();
    }
    else{
      if(state == "on")
      {
        setLED(startPosition, endPosition, color);
      }  
    }
  }
  updateLED();
}

String communicate() {
  char inputCharArray[100];
  int i = 0;
  while(client.available() > 0){
    inputCharArray[i] = client.read();
    i++;
  }
  inputString = String(inputCharArray);
  Serial.println(inputCharArray);
  updateLED();
  return input;
}

void procCmd(int cmd, int data1, int data2) {
  switch(cmd) {
  case CMD_OFF:
    state = "off";
  case CMD_SET:
    startPosition = data1;
    endPosition = data2;
    color = rgbColor(255,0,255);
    state = "on";
    break;
  case CMD_PTRN:
    if(data1 == PTRN_RAINBOW)
    {
      state = "rainbow";
      moveSpeed = data2;
    }
    break;
  }
}

void rainbow()
{
  uint16_t time = millis()*moveSpeed >> 2;
  for(uint16_t i = 0; i < LED_COUNT; i++)
  {
    byte x = (time >> 2) - (i << 3);
    colors[i] = hsvToRgb((uint32_t)x * 359 / 256, 255, 255);
  }
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











