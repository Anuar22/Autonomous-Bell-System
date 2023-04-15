#include <Adafruit_GFX.h>      // include Adafruit graphics library
#include <Adafruit_ST7735.h>   // include Adafruit ST7735 TFT library
#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <Arduino.h>
#ifdef ESP32
  #include <WiFi.h>
  #include <AsyncTCP.h>
#else
  #include <ESP8266WiFi.h>
  #include <ESPAsyncTCP.h>
#endif
#include <ESPAsyncWebServer.h>



#define relay  D6
#define conn   D0 //connected to my WiFi
#define discon D1 //not connected
// ST7735 TFT module connections
#define TFT_RST   D4     // TFT RST pin is connected to NodeMCU pin D4 (GPIO2)
#define TFT_CS    D3     // TFT CS  pin is connected to NodeMCU pin D3 (GPIO0)
#define TFT_DC    D2     // TFT DC  pin is connected to NodeMCU pin D2 (GPIO4)
// initialize ST7735 TFT library with hardware SPI module

// SCK (CLK) ---> NodeMCU pin D5 (GPIO14)
// MOSI(DIN) ---> NodeMCU pin D7 (GPIO13)
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);
 
float p = 3.1415926;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

const char* ssid = "Anuar's phone";
const char* password = "vally2023";

WiFiUDP ntpUDP;
//NTPClient timeClient(ntpUDP, "ke.pool.ntp.org", 10800, 60000);
NTPClient timeClient(ntpUDP);

String formattedDate;
String dayStamp;
String timeStamp;



AsyncWebServer server(80);

const char* input_parameter1 = "input_string";
const char* input_parameter2 = "input_integer";
const char* input_parameter3 = "input_float";

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html><head>
  <title>AUTOMATIC BELL SYSTEM</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    html {font-family: Times New Roman; display: inline-block; text-align: center;}
    h2 {font-size: 3.0rem; color: #FF0000;font-family: Cursive;}
  </style>
  </head><body>
  <h2>Autonomous Bell System</h2> 
  <form action="/get">
    Enter date: <input type="date" name="input_string">
    <input type="submit" value="Submit"> 
  </form><br>
  <form action="/get">
     school breakfast: <input type="time" name="input_integer">
    <input type="submit" value="Submit">
  </form><br>
  <form action="/get">
    LUNCH BREAK: <input type="time" name="input_float">
    <input type="submit" value="Submit">
  </form>
</body></html>)rawliteral";

void notFound(AsyncWebServerRequest *request) {
  request->send(404, "date/plain", "Not found");
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connecting...");
    return;
  }
  Serial.println();
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
   server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html);
  });

  server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String input_message;
    String input_parameter;

    if (request->hasParam(input_parameter1)) {
      input_message = request->getParam(input_parameter1)->value();
      input_parameter = input_parameter1;
    }
    else if (request->hasParam(input_parameter2)) {
      input_message = request->getParam(input_parameter2)->value();
      input_parameter = input_parameter2;
    }

    else if (request->hasParam(input_parameter3)) {
      input_message = request->getParam(input_parameter3)->value();
      input_parameter = input_parameter3;
    }
    else {
      input_message = "No message sent";
      input_parameter = "none";
    }
    Serial.println(input_message);
    request->send(200, "text/html", "HTTP GET request sent to your ESP on input field ("+ input_parameter + ") with value: " + input_message + "<br><a href=\"/\">Return to Home Page</a>");
  });
  server.onNotFound(notFound);
  server.begin();
  // put your setup code here, to run once:
  
  
  pinMode(relay,OUTPUT);
  pinMode(conn,OUTPUT);
  pinMode(discon,OUTPUT);
  
  WiFi.begin( ssid, password );

  
 tft.setTextColor(ST7735_BLACK);
  tft.setCursor(20,30);
  tft.setTextSize(2);
  tft.println("Connecting...");
  
  while ( WiFi.status() != WL_CONNECTED ) {
    digitalWrite(discon, HIGH);
    delay ( 700 );
  }

  digitalWrite(discon, LOW);
  digitalWrite(conn, HIGH);
  timeClient.begin();
  timeClient.setTimeOffset(10800);
  
  tft.initR(INITR_BLACKTAB);   // initialize a ST7735S chip, black tab
 tft.setRotation(1);
  uint16_t time = millis();
  tft.fillScreen(ST7735_BLACK);
  time = millis() - time;
  
  delay(500);
  
  // large block of text
  tft.fillScreen(ST7735_BLACK);
//  testdrawtext("Lorem ipsum dolor sit amet, consectetur adipiscing elit. Curabitur adipiscing ante sed nibh tincidunt feugiat. Maecenas enim massa, fringilla sed malesuada et, malesuada sit amet turpis. Sed porttitor neque ut ante pretium vitae malesuada nunc bibendum. Nullam aliquet ultrices massa eu hendrerit. Ut sed nisi lorem. In vestibulum purus a tortor imperdiet posuere. ", ST7735_WHITE);
  delay(1000);

}

void loop() {
  // put your main code here, to run repeatedly:
  
  timeClient.update();
  tft.setTextColor(ST7735_YELLOW);
  tft.setCursor(20,30);
  tft.setTextSize(2);
  tft.println(timeClient.getFormattedTime());
  tft.setCursor(25, 80);
  tft.setTextSize(2);
  tft.println(daysOfTheWeek[timeClient.getDay()]);
  
   
  //digitalWrite(relay, HIGH);
  delay(500);
  tft.fillScreen(ST7735_BLACK);

}
