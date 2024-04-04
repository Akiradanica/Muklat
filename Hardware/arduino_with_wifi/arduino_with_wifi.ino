#include <SPI.h>
#include <WiFiNINA.h>
#include "wifi_pass.h" 
#include <WiFiUdp.h>

//edit wifi pass at wifi_pass.h
char ssid[] = SECRET_SSID;        
char pass[] = SECRET_PASS;    
//int keyIndex = 0;    

IPAddress staticIP(192, 168, 1, 100); // Static IP address you want to assign to the Nano 33 IoT
IPAddress gateway(192, 168, 1, 1);     // IP address of your gateway/router
IPAddress subnet(255, 255, 255, 0);    // Subnet mask

int status = WL_IDLE_STATUS;
WiFiServer server(80);

void setup() 
{
  Serial.begin(9600);      

  pinMode(9, OUTPUT);      // set the LED pin mode

  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) 
  {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }
    WiFi.config(staticIP, gateway, subnet);
  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) 
  {
    Serial.println("Please upgrade the firmware");
  }

  // attempt to connect to WiFi network:
  while (status != WL_CONNECTED) 
  {
    Serial.println("Attempting to connect to " );
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);
    delay(10000);
  }

  server.begin();                           // start the web server on port 80
  printWifiStatus();                        // you're connected now, so print out the status
}

void loop()
{
  wifiStatus();
}

void wifiStatus() 
{
  WiFiClient client = server.available();   // listen for incoming clients

  if (client) 
  {                            
    Serial.println("new client");           
    String currentLine = "";                
    while (client.connected()) 
    {            // loop while the client's connected
      if (client.available()) 
      {             
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out to the serial monitor
        if (c == '\n') 
        { // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) 
          {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            // the content of the HTTP response follows the header:
            client.print("Click <a href=\"/H\">here</a> turn the LED on pin 9 on<br>");
            client.print("Click <a href=\"/L\">here</a> turn the LED on pin 9 off<br>");

            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          } 
          
          else 
          {    // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        } 
        else if (c != '\r') 
        {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }

        // Check to see if the client request was "GET /H" or "GET /L":
        if (currentLine.endsWith("GET /H")) 
        {
          digitalWrite(9, HIGH);               // GET /H turns the LED on
        }
        if (currentLine.endsWith("GET /L")) 
        {
          digitalWrite(9, LOW);                // GET /L turns the LED off
        }
      }
    }
    // close the connection:
    client.stop();
    Serial.println("client disconnected");
  }
}

void printWifiStatus() 
{
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
  // print where to go in a browser:
  Serial.print("To see this page in action, open a browser to http://");
  Serial.println(ip);
}