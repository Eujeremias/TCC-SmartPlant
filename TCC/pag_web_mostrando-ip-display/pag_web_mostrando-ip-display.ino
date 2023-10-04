// #include <WiFi.h> //lib para configuração do Wifi (para esp32)
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

#include <ESP8266mDNS.h>
// #include <ESPmDNS.h> //lib necessária para comunicação network DO ESP32
#include <WiFiUdp.h> //lib necessária para comunicação network
#include <ArduinoOTA.h> //primeiro gravar sem ip fixo, e qd o ota estiver funcionar (ver ip na lista de portas) gravar com ip fixo

// bibliotecas e comandos oled ------------------------------
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
//------------------------------------------------------------------------

// const char* ssid = "AsaBranca";
// const char* password = "87323375";
const char* ssid = "IFAL - Rio Largo";
const char* password = "ifalriolargo";
//const char* ssid = "Espaço 4.0";
//const char* password = "Esp$@$2022";

// IPAddress local_IP(192, 168, 0, 110);
// IPAddress gateway(192, 168, 0, 1);
// IPAddress subnet(255, 255, 255, 0);
// IPAddress primaryDNS(8, 8, 8, 8);
// IPAddress secondaryDNS(8, 8, 4, 4);

/*
  IPAddress local_IP(10, 220, 30, 120);
  IPAddress gateway(10, 220, 30, 1);
  IPAddress subnet(255, 255, 255, 0);
  IPAddress primaryDNS(8, 8, 8, 8);
  IPAddress secondaryDNS(8, 8, 4, 4);
*/

int led = D0;
unsigned long int ultimo = 0;
int contador = 0;
int aleatorio;

#include "index.h" //pagina HTML com conteudo javascripts
ESP8266WebServer server(80); //Server on port 80
//============================================================
void raiz() {
  String s = pagina; //lê o conteudo HTML
  server.send(200, "text/html", s); //envia a página web
}

void gerar_numero() {
  //int a = analogRead(A0);
  aleatorio = random(100);
  String x = String(aleatorio);
  server.send(200, "text/plane", x); //Envia o valor aleatório
}
//============================================================

void setup() {
  Serial.begin(115200);
  pinMode(led, OUTPUT);
  digitalWrite(led, 0); //led apagado

  //---------OLED----------------------
  //display.begin() // o if a seguir irá executar o display.begin()
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  delay(50);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0); //coluna, linha
  // Display static text
  display.println("Ola!!");
  display.display();
  delay(1000);
  //--------------------------------
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  //---------OLED----------------------
  delay(500);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println(WiFi.localIP());
  display.display();
  delay(1000);

  //--------------------------------
  // WiFi.config(local_IP, gateway, subnet, primaryjfrttjhjkkjjjDNS, secondaryDNS); //PASSA OS PARÂMETROS PARA A FUNÇÃO QUE VAI SETAR O IP FIXO NO NODEMCU
  Serial.println("");
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(led, !digitalRead(led));
    delay(200);

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.println("Conectando...");
    display.setCursor(0, 10);
    display.println(contador);
    contador++;
    display.display();
    Serial.print(".");
  }
  digitalWrite(led, 1);

  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    //Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

  //inicio comandos OTA -------------------------------
  // Port defaults to 8266

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

  //------------ fim comandos ota-------------------------------------------------

  //----mostrar IP conectado ------------------------
  delay(500);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  // Display static text
  display.println(WiFi.localIP());
  display.display();
  delay(1000);
  //--------------------
  //tratar requisições WEBs
  server.on("/", raiz); //trata a requisição "/" na função raiz
  server.on("/lerNUM", gerar_numero);
  server.begin();
  Serial.println("Servidor HTTP iniciado");
}

void loop() {
  ArduinoOTA.handle(); //Escuta OTA
  server.handleClient(); //Escuta requisição dos clientes

  if (millis() - ultimo > 500) {
    digitalWrite(led, !digitalRead(led));
    Serial.println(contador);
    //----comandos para atualizar display OTA ---------------------------
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.println(WiFi.localIP());
    display.setCursor(0, 15);
    display.println(contador);
    display.setCursor(0, 30);
    display.println(aleatorio);
    display.setCursor(0, 45);
    display.println("Instituto Federal de Alagoas");
    display.display();
    //--------------------------------
    contador++;
    ultimo = millis();
  }
}
