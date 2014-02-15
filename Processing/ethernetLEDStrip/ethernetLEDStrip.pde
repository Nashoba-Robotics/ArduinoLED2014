/* Arduino Pololu LED Strip Control Over Ethernet and Shield - JTC
READ ME!!! ---> THIS IS VERY IMPORTANT
  You must click "Sketch" then "Import Library" then "add library"
  in the window that opened, you must install UDP

WARNING ---> I (jtaylorchang) did not have an Arduino nor an LED strip with me at the time
  that I am writing this code. Therefore I must provide warning that this code is not tested
  and should be used at your own risk, not that there is much of a risk. Possible threat of
  bugs in current version
*/
import hypermedia.net.*;

int dWidth = 480, dHeight = 320;

UDP udp;

int CMD_SET = 0;//send a command tag with goal of setting a specific light amount (0 - 60 as data input)
int CMD_PATTERN = 1;//send a command tag with goal of setting a led pattern (? - ? as data input) <-- NOT FINISHED

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
    case '0':
      command(CMD_SET, 0);
      break;
    case '1':
      command(CMD_SET, 10);
      break;
    case '2':
      command(CMD_SET, 20);
      break;
    case '3':
      command(CMD_SET, 30);
      break;
    case '4':
      command(CMD_SET, 40);
      break;
    case '5':
      command(CMD_SET, 50);
      break;
    case '6':
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
