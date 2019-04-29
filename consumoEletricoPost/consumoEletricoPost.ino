#include <SPI.h>
#include <Ethernet.h>

#include "EmonLib.h"

EnergyMonitor SCT013;
 
int pinSCT = A0;   //Pino analógico conectado ao SCT-013
 
int tensao = 110;
int potencia;


// Enter a MAC address for your controller below.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
// if you don't want to use DNS (and reduce your sketch size)
// use the numeric IP instead of the name for the server:
//IPAddress server(74,125,232,128);  // numeric IP for Google (no DNS)
char server[] = "35.173.69.207";    // name address for Google (using DNS)

// Set the static IP address to use if the DHCP fails to assign
IPAddress ip(192,168,0,100);
IPAddress gateway(192,168,0,1);
IPAddress subnet(255, 255, 255, 0);
 
// Initialize the Ethernet client library
// with the IP address and port of the server 
// that you want to connect to (port 80 is default for HTTP):
EthernetClient client;
String varEstabelecimento = "1";
String varPredio = "1";
String varSala = "7";
//String varKWH = "0";
//String temp = ("estabelecimento="+varEstabelecimento+"&predio="+varPredio+"&sala="+varSala+"&kwh="+varKWH+");  


void setup()
{
  //SCT013.current(pinSCT, 6.0606);
  SCT013.current(pinSCT, 8.48484);
 
    Serial.begin(9600);

  // Open serial communications and wait for port to open:
  Serial.begin(9600);
   while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

  // start the Ethernet connection:
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // no point in carrying on, so do nothing forevermore:
    // try to congifure using IP address instead of DHCP:
    Ethernet.begin(mac, ip);
  }
  // give the Ethernet shield a second to initialize:
  delay(1000);
  Serial.println("connecting... and sending");
}
 
void loop()
{
    double Irms = SCT013.calcIrms(1480);   // Calcula o valor da Corrente
    
    potencia = Irms * tensao;          // Calcula o valor da Potencia Instantanea    

    Serial.print("Corrente = ");
    Serial.print(Irms);
    Serial.println(" A");
    
    Serial.print("Potencia = ");
    Serial.print(potencia);
    Serial.println(" W");


int varKWH = potencia;
String temp = ("estabelecimento="+varEstabelecimento+"&predio="+varPredio+"&sala="+varSala+"&kwh="+varKWH);  

// if you get a connection, report back via serial:
  if ((client.connect(server, 80)) && (potencia > 1)) {
    delay(1000);
    Serial.println("connected");
    Serial.println("Sending to Server: ");                    
    client.println("POST /consumos HTTP/1.1");           
    Serial.print("POST /consumos HTTP/1.1");           
    client.println("Host: marcmatias.pythonanywhere.com");
    client.println("Content-Type: application/x-www-form-urlencoded; charset=UTF-8");
    client.println("Connection: close");
    client.println("User-Agent: Arduino/1.0");
    client.print("Content-Length: ");
    client.println(temp.length());
    client.println();
    client.print(temp);
    client.println(); 
    Serial.println("\n");
  } 
  else {
    // kf you didn't get a connection to the server:
    Serial.println("not sending\n");
    delay(1000);
  }

  // if there are incoming bytes available 
  // from the server, read them and print them:
  if (client.available()) {
    char c = client.read();
    Serial.print(c);
  }

//  // if the server's disconnected, stop the client:
//  if (!client.connected()) {
//    Serial.println();
//    Serial.println("disconnecting.");
//    client.stop();
//
//    // do nothing forevermore:
//    while(true);
//  }
}
