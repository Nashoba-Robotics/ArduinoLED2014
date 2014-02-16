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
#define PTRN_GRADIENT 1
#define PTRN_LRBOUNCE 2//<-- NOT FINISHED
#define PTRN_INOUTBOUNCE 3//<-- NOT FINISHED

PololuLedStrip<6> ledStrip;//<6> on UNO, Leonardo, and Duemilanove, <3> on Mega
rgb_color colors[LED_COUNT];
rgb_color blank[LED_COUNT];

//Network stuff:
byte mac[] = { 
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(10, 17, 68, 12);
unsigned int localPort = 8888;
EthernetServer server(localPort);

//Communication stuff:
String input = "";
int pbCmd = -1;
int pbData1 = -1;
int pbData2 = -1;
int pbData3 = -1;
String delimiter1 = " ";
String delimiter2 = ":";
String delimiter3 = "-";

String inputString;

//State stuff:
String state = "lrbounce";

//Moving pattern stuff:
int moveSpeed = 1;

//Set stuff:
int startPosition = 10;
int endPosition = 60;
rgb_color color = rgbColor(255,255,255);

//Bounce stuff:
int bouncePosition = 30;
int length = 10;
boolean bounceDirection = true;

//Gradient stuff:
int grad0[3] = {//start color
  255, 255, 0
};
int grad50[3] = {//middle color
  255, 0, 255
};
int grad100[3] = {//end color
  0, 255, 255
};
float gradChange1[3] = {
  (float) (grad50[0] - grad0[0]) / LED_COUNT, (float) (grad50[1] - grad0[1]) / LED_COUNT, (float) (grad50[2] - grad0[2]) / LED_COUNT
};
float gradChange2[3] = {
  (float) (grad100[0] - grad50[0]) / LED_COUNT, (float) (grad100[1] - grad50[1]) / LED_COUNT, (float) (grad100[2] - grad50[2]) / LED_COUNT
};

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
    r, g, b                                                                   };
}

rgb_color rgbColor(uint16_t r, uint16_t g, uint16_t b) {
  return (rgb_color) {
    r, g, b                                                                          };
}

void setup() {
  for(uint16_t i = 0; i < LED_COUNT; i++) {
    blank[i] = rgbColor(0, 0, 0);
  }
  clearLED();
  Serial.begin(9600);
  Ethernet.begin(mac, ip);
  server.begin();

  Serial.println("Listening for clients");

  delay(10);
}

void loop() {
  client = server.available();
  if (client) {
    String inputString = communicate();
    pbCmd = inputString.substring(0, input.indexOf(delimiter1)).toInt();
    pbData1 = inputString.substring(input.indexOf(delimiter1) + 1, input.indexOf(delimiter2)).toInt();
    pbData2 = inputString.substring(input.indexOf(delimiter2) + 1, input.indexOf(delimiter3)).toInt();
    pbData3 = inputString.substring(input.indexOf(delimiter3) + 1, input.length()).toInt();
    procCmd(pbCmd, pbData1, pbData2, pbData3);
  }
  if(state == "off")
  {
    setLED(0, LED_COUNT, rgbColor(0, 0, 0));
  }  
  else{
    if(state == "set")
    {
      setLED(startPosition, endPosition, color);
    }  
    else{
      if(state == "rainbow")
      {
        rainbow();
      }
      else{
        if(state == "gradient")
        {
          gradient();
        }
        else{
          if(state == "lrbounce")
          {
            leftRightBounce();
          }
        }
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

void procCmd(int cmd, int data1, int data2, int data3) {
  switch(cmd) {
  case CMD_OFF:
    state = "off";
  case CMD_SET:
    startPosition = data1;
    endPosition = data2;
    color = rgbColor(255,0,255);
    state = "set";
    break;
  case CMD_PTRN:
    if(data1 == PTRN_RAINBOW)
    {
      state = "rainbow";
      moveSpeed = data2;
    }
    else{
      if(data1 == PTRN_GRADIENT)
      {
        state = "gradient";
        startPosition = data2;
        endPosition = data3;
      }
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

void gradient()
{
  for(uint16_t i = 0; i < LED_COUNT; i++) {
    colors[i] = rgbColor(0, 0, 0);
  }
  for(uint16_t i = startPosition; i < endPosition; i++) {
    if(i < LED_COUNT) {
      if(i < endPosition / 2) {
        colors[i] = rgbColor((int) (grad0[0] + i * gradChange1[0] * 2), (int) (grad0[1] + i * gradChange1[1] * 2), (int) (grad0[2] + i * gradChange1[2] * 2));
      } 
      else if(i >= endPosition / 2) {
        colors[i] = rgbColor((int) (grad50[0] + (i - endPosition / 2) * gradChange2[0] * 2), (int) (grad50[1] + (i - endPosition / 2) * gradChange2[1] * 2), (int) (grad50[2] + (i - endPosition / 2) * gradChange2[2] * 2));
      }
    } 
  }
}

void leftRightBounce()
{
  for(uint16_t i = 0; i < LED_COUNT; i++) {
    colors[i] = rgbColor(0, 0, 0);
  }
  for(uint16_t i = bouncePosition; i < (length+bouncePosition); i++) {
    colors[i] = rgbColor(255,255,255);
  }
  if(bouncePosition + length == LED_COUNT)
  {
    bounceDirection = !bounceDirection;
    Serial.println("bounce far!");
  }
  if(bouncePosition == 0)
  {
    bounceDirection = !bounceDirection;
    Serial.println("bounce close!");
  }
  if(bounceDirection)
  {
    bouncePosition++;
    Serial.print("up");
    Serial.println(bouncePosition);
  }
  else  {
    bouncePosition--;
    Serial.print("down");
    Serial.println(bouncePosition);

  }
}

void setLED(int startPosition, int endPosition, rgb_color rgbcolor) {
  for(uint16_t i = 0; i < LED_COUNT; i++) {
    colors[i] = rgbColor(0, 0, 0);
  }
  for(uint16_t i = bouncePosition; i < (length+bouncePosition); i++) {
    if(i < LED_COUNT) {
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

void clearLED() {
  ledStrip.write(blank, LED_COUNT);
}


