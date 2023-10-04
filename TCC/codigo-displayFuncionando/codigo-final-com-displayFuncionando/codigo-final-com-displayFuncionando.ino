
// #include <WiFi.h> //lib para configuração do Wifi (para esp32)
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
// #include <ESPmDNS.h> //lib necessária para comunicação network
#include <WiFiUdp.h> //lib necessária para comunicação network

#include <ArduinoOTA.h> //primeiro gravar sem ip fixo, e qd o ota estiver funcionar (ver ip na lista de portas) gravar com ip fixo

#include <time.h>

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP8266WebServer.h>

#include "index.h"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define LOGO_HEIGHT   16
#define LOGO_WIDTH    16

const char* ssid = "IFAL - Rio Largo";
const char* password = "ifalriolargo";
//const char* ssid = "Espaço 4.0";
//const char* password = "Esp$@$2022";


  IPAddress local_IP(10, 220, 30, 120);
  IPAddress gateway(10, 220, 30, 1);
  IPAddress subnet(255, 255, 255, 0);
  IPAddress primaryDNS(8, 8, 8, 8);
  IPAddress secondaryDNS(8, 8, 4, 4);


//pinos para ESP-01:
int led_status = D0; // corresponde ao led azul e ao mesmo tempo ao TX
int pinoSensorUmidade = A0;
int valor;
int porcentagem;
unsigned long int ultimo_tempo = 0;

ESP8266WebServer server(80);

void raiz(){
  String s = pagina;
  server.send(200, "text/html", s);
}
void gerar_numero(){
  int a = porcentagem;
  String x = String(a);
  server.send(200, "text/plane", x);
}

void setup() {
  Serial.begin(115200);
  
  pinMode(led_status, OUTPUT);
  digitalWrite(led_status, 0); //led apagado
  pinMode(pinoSensorUmidade, INPUT);
  Serial.print("");

  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)){
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }

  // display.init();
  // display.flipScreenVertically();

  //WiFi.mode(WIFI_AP);
  //WiFi.softAP(ssid); //sem senha
  //WiFi.softAP(ssid, password); //com senha
  /*
    Serial.println("");
    Serial.print("Conectado a ");
    Serial.println(ssid);
    //IPAddress myIP = WiFi.softAPIP();
    Serial.print("Endereço IP: ");
    Serial.println(WiFi.softAPIP());
  */

  //---------- inicio OTA:
  //Serial.begin(115200);
  //Serial.println("Booting");
  //WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS); //PASSA OS PARÂMETROS PARA A FUNÇÃO QUE VAI SETAR O IP FIXO NO NODEMCU

  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(led_status, !digitalRead(led_status));
    delay(200);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Endereço IP: ");
  Serial.println(WiFi.localIP());

  digitalWrite(led_status, 1);


  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    //Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }
  // Port defaults to 8266
  // ArduinoOTA.setPort(8266);
  server.on("/", raiz);
  server.on("/lerNUM", gerar_numero);
  // Hostname defaults to esp8266-[ChipID]
  ArduinoOTA.setHostname("Jeremias nodemcuIrriga");

  // No authentication by default
  //ArduinoOTA.setPassword("admin");

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_FS
      type = "filesystem";
    }

    // NOTE: if updating FS this would be the place to unmount FS using FS.end()
    //Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    //Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    //Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    //Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      //Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      //Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      //Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      //Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      //Serial.println("End Failed");
    }
  });
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  //------------ fim ota-------------------------------------------------t


  for (int i = 0 ; i < 3; i++) {
    digitalWrite(led_status, 1);
    delay(20);
    digitalWrite(led_status, 0);
    delay(20);
  }

}

void telainicial()
{
  valor = analogRead(pinoSensorUmidade);
  porcentagem =map(valor, 484, 1023, 100, 0);
  Serial.println(porcentagem);
  delay(1000);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,10);
  // display.print("porcentagem");
  if(porcentagem > 60){
  display.println(WiFi.localIP());
  // display.print(porcentagem);
  // display.println("% umido");
  // display.print("Faz o L");
  display.display();
  }else{
    display.println("Status: Seco");
    display.print(porcentagem);
    display.println("% umido");
    display.print("Faz o L");
    display.display();

  }

}

void loop() {
  ArduinoOTA.handle();
  server.handleClient();
  telainicial();

  if (millis() - ultimo_tempo >= 500) {
    valor = analogRead(pinoSensorUmidade);
    porcentagem = map(valor, 484, 1023, 100, 0);
    Serial.println(porcentagem);
    ultimo_tempo = millis();
  }
  digitalWrite(led_status, 1);
  delay(700);
  digitalWrite(led_status, 0);
  delay(300);
}
