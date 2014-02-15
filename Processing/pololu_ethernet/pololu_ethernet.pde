/* READ ME!!! ---> THIS IS VERY IMPORTANT
you must click "Sketch" then "Import Library" then "add library"
in the window that opened, you must install UDP
*/
import hypermedia.net.*;

UDP udp;

void setup() {
  udp = new UDP( this, 6100 );
  udp.listen( true );
}

void draw() {
  
}

void keyPressed() {
  String ip = "192.168.1.177";
  int port = 8888;
  println("start");
  udp.send("15", ip, port );
}
 
void receive( byte[] data ) {
  for(int i=0; i < data.length; i++) 
  print(char(data[i]));  
  println();
}
