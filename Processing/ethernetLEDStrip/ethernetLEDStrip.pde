/* Arduino Pololu LED Strip Control Over Ethernet and Shield - JTC
READ ME!!! ---> THIS IS VERY IMPORTANT
  You must click "Sketch" then "Import Library" then "add library"
  in the window that opened, you must install UDP
*/
import hypermedia.net.*;

int dWidth = 480, dHeight = 320;

UDP udp;

int CMD_SET = 0;//send a command tag with goal of setting a specific light amount (0 - 60 as data input)
int CMD_PTRN = 1;//send a command tag with goal of setting a led pattern (? - ? as data input) <-- NOT FINISHED

int PTRN_RAINBOW = 0;//rainbow pattern <-- NOT FINISHED
int PTRN_LRBOUNCE = 1;//left to right bounce pattern <-- NOT FINISHED
int PTRN_INOUTBOUNCE = 2;//inwards to outwards bounce pattern <-- NOT FINISHED

void setup() {
  size(dWidth, dHeight);
  frame.setTitle("Pololu LED");
  udp = new UDP(this, 6100);
  udp.listen(true);
}

void draw() {
  
}

void keyPressed() {
  switch(key) {//NOTE: easily optimizable by converting string to integer and then multiplying by 10 to get data input
    case '1':
      command(CMD_SET, 9);
      break;
    case '2':
      command(CMD_SET, 12);
      break;
    case '3':
      command(CMD_SET, 18);
      break;
    case '4':
      command(CMD_SET, 24);
      break;
    case '5':
      command(CMD_SET, 30);
      break;
    case '6':
      command(CMD_SET, 36);
      break;
    case '7':
      command(CMD_SET, 42);
      break;
    case '8':
      command(CMD_SET, 48);
      break;
    case '9':
      command(CMD_SET, 54);
      break;
    case '0':
      command(CMD_SET, 60);
      break;
  }
}

void command(int command, int data) {
  String sData = command + " " + data;
  send(sData);
}

void send(String data) {
  String ip = "192.168.1.177";
  int port = 8888;
  println("Sent '" + data + "'");
  udp.send(data, ip, port);
}

void receive(byte[] data) {
  for(int i=0; i < data.length; i++) {
    print(char(data[i]));
  }
  println();
}
