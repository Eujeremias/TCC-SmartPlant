
// #include <WiFi.h> //lib para configuração do Wifi (para esp32)
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

// bibliotecas e comandos oled ------------------------------
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels
// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
//------------------------------------------------------------------------

#include "index.h"

const char* ssid = "IFAL - Rio Largo";
const char* password = "ifalriolargo";

int pinoSensorUmidade = A0;
int valor;
int porcentagem;
unsigned long int ultimo_tempo = 0;
//const char* ssid = "Espaço 4.0";
//const char* password = "Esp$@$2022";
int led = D0;
int aleatorio;
int contador = 0;
int porcentagemUmidade;

ESP8266WebServer server(80);

void raiz() {
  String s = pagina;
  server.send(200, "text/html", s);
}
void gerar_numero() {
  //int a = random(101);
  //int a = porcentagem;
  String x = String(aleatorio);
  server.send(200, "text/plane", x);
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  pinMode(pinoSensorUmidade, INPUT);
  pinMode(led, OUTPUT);

  Serial.println("");

  //---------OLED----------------------
  //display.begin() // o if a seguir irá executar o display.begin()
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {  // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;
  }
  delay(50);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);  //coluna, linha
  // Display static text
  display.print("Ola!!");
  display.println("Tudo bem?");
  display.println("Estamos perguntando ao planeta como ele está! :D");
  display.println("Aguarde um momento...");
  display.display();
  delay(3000);
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
  // WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS);  //PASSA OS PARÂMETROS PARA A FUNÇÃO QUE VAI SETAR O IP FIXO NO NODEMCU
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
  Serial.println("");
  Serial.print("Endereço IP: ");
  Serial.println(WiFi.localIP());

  digitalWrite(led, 1);

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

  //metódos abaixo recebe valor tipo string no 2° parâmetro
  server.on("/", raiz);
  server.on("/lerNUM", gerar_numero);

  server.begin();
}

void loop() {
  server.handleClient();

  if (millis() - ultimo_tempo >= 500) {

    valor = analogRead(pinoSensorUmidade);
    porcentagemUmidade = map(valor, 484, 1023, 100, 0);
    Serial.println(porcentagemUmidade);
    //----comandos para atualizar display OTA ---------------------------
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    //display.println(WiFi.localIP());
    //display.setCursor(0, 15);
    // display.println(contador);

    display.setCursor(0, 12);
    display.print("Temperatura: ");
    //display.print(temp);
    display.println(" *C");

    display.setCursor(0, 24);
    display.print("Umidade do ar: ");
    //display.print(umi);
    display.println(" %");

    display.setCursor(0, 36);
    display.print("Umidade solo: ");
    display.print(porcentagemUmidade);
    display.println(" %");

    aleatorio = random(101);
    display.setCursor(0, 48);
    display.print("random: ");
    display.print(aleatorio);


    display.display();

    ultimo_tempo = millis();
  }
}
