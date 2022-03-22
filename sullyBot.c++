
#include <SPI.h>
#include <WiFiNINA.h>
#include <Servo.h>


#include "arduino_secrets.h" 

char ssid[] = SECRET_SSID;        
char pass[] = SECRET_PASS;  
int keyIndex = 0;                 
Servo xaxis;
Servo yaxis;
int status = WL_IDLE_STATUS;
String data;
int xPos=0;
int yPos=0;
int xOff=0;
int yOff=0;
int pos1 =0;
int pos2=0;
int po1 =0;
int po2=0;
WiFiServer server(80);

void setup() {
  xaxis.attach(5);
  yaxis.attach(7);
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

 
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
   
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }

  // attempt to connect to WiFi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);

    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }
  server.begin();
  // you're connected now, so print out the status:
  printWifiStatus();
}


void loop() {
  // listen for incoming clients
  WiFiClient client = server.available();
  if (client) {
    Serial.println("new client");
    // an HTTP request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        data+=c;
        Serial.write(c);
     
      
        if (c == '\n' && currentLineIsBlank) {

          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");  // the connection will be closed after completion of the response
          client.println("Refresh: 5");  // refresh the page automatically every 5 sec
          client.println();
          //client.println("<!DOCTYPE html>");
          client.println("<html lang=\"en\">");
          client.println("<head>");
          client.println("<meta charset=\"UTF-8\" />");
          client.println("<meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\" />");
          client.println("<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\" />");
          client.println("<title>Sully</title>");
          client.println("<script src=\"https://ajax.googleapis.com/ajax/libs/jquery/3.3.1/jquery.min.js\"></script>");
          client.println("</head>");
          client.println("<body>");
          client.println("<h2>X Offset 0-90</h2>");
          client.println("<input");
          client.println("type=\"range\"");
          client.println("class=\"slider\"");
          client.println("min=\"0\"");
          client.println("max=\"90\"");
          client.println("id=\"xOffset\"");
          client.println("onchange=\"xoff(this.value)\"");
          client.println("/>");
          client.println("<h2>Y Offset 0-90</h2>");
          client.println("<input");
          client.println("type=\"range\"");
          client.println("class=\"slider\"");
          client.println("min=\"0\"");
          client.println("max=\"90\"");
          client.println("id=\"yOffset\"");
          client.println("onchange=\"yoff(this.value)\"");
          client.println("/>");
          client.println("<h2>Coordinate field 0-180</h2>");
          client.println("<Border style=\"width:180 height:180\">");
          client.println("<canvas ");
          client.println("style=\"width: 180px; height: 180px; background-color: blue\" HorizontalAlignment=\"Center\" VerticalAlignment=\"Center\"");
          client.println("id=\"canvas\"");
          client.println("></canvas>");
          client.println("</Border>");
          client.println("<script>");
          client.println("window.addEventListener(\"mousemove\", function (e) {");
          client.println("var height=$(window).height(); var width= $(window).width();");
          client.println("var x = Math.round((e.x*180)/width);");
          client.println("var y = Math.round((e.y*180)/height); ");
          client.println("coords(x,y);");
          client.println("});");
          client.println("$.ajaxSetup({ timeout: 1000 });");
          client.println("function xoff(gle) {");
            
          client.println("$.get(\"xOffset=\" +gle +\"&\");");
          client.println("console.log(gle);");
          client.println("{");
          client.println("Connection: close;");
          client.println("}");
          client.println("}");
          client.println("function yoff(gl) {");
            
          client.println("$.get(\"yOffset=\" +gl +\"&\");");
          client.println("console.log(gl);");
          client.println("{");
          client.println("Connection: close;");
          client.println("}");
          client.println("}");
          client.println("function coords(ngl,dk) {");
          
          client.println("$.get(\"coordinates=\" +ngl+\"%\"+dk+\"&\");");
          client.println("console.log(ngl);");
          client.println("console.log(dk);");
          client.println("{");
          client.println("Connection: close;");
          client.println("}");
          client.println("}");
          client.println("</script>");
          client.println("</body>");
          client.println("</html>");

          if(data.indexOf("GET /coordinates=")>=0){
            
            pos1 =data.indexOf('=');
            pos2=data.indexOf('%');
            xPos= data.substring(pos1+1,pos2).toInt()-xOff;
            delay(5);
            po1= data.indexOf('%');
            po2= data.indexOf('&');
            yPos= data.substring(po1+1,po2).toInt()-yOff;
            Serial.print(yPos);
            Serial.print(xPos);
            yaxis.write(yPos);
            xaxis.write(xPos);
            
          }
          if(data.indexOf("GET /yOffset=")>=0){
            pos1= data.indexOf('=');
            pos2= data.indexOf('&');
            String placeholderY = data.substring(pos1+1,pos2);
            yOff= placeholderY.toInt();
            
          }
          if(data.indexOf("GET /xOffset=")>=0){
            pos1= data.indexOf('=');
            pos2= data.indexOf('&');
            String placeholderX = data.substring(pos1+1,pos2);
            xOff= placeholderX.toInt();
            
          }
          
          client.println();
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    data ="";

    client.stop();
    Serial.println("client disconnected");
  }
}


void printWifiStatus() {

  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());


  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);


  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}
