//01/09/2023: <<funcionou>> codigo obtido na ide 2.1.1 em arquivo=>exemplos=>thingsboard=>0003_esp8266_esp32_send_data
//com bib ThingsBoard.h versao 0.10.1 e arduinojson versão 6.21.3
//haverá alguns warnings durante a compilação mas tudo bem.

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#define THINGSBOARD_ENABLE_PROGMEM 0  //não remover essa linha! (ao remover o nodemcu ficará reiniciando sozinho)
#include <ThingsBoard.h>

constexpr char WIFI_SSID[] = "IFAL - Rio Largo";
constexpr char WIFI_PASSWORD[] = "ifalriolargo";
constexpr char TOKEN[] = "WLMk433vZ1vkwPaoKmxl";

// IPAddress local_IP(192, 168, 0, 110);
// IPAddress gateway(192, 168, 0, 1);
// IPAddress subnet(255, 255, 255, 0);
// IPAddress primaryDNS(8, 8, 8, 8);
// IPAddress secondaryDNS(8, 8, 4, 4);

IPAddress local_IP(10, 220, 30, 120);
IPAddress gateway(10, 220, 30, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(8, 8, 8, 8);
IPAddress secondaryDNS(8, 8, 4, 4);

// bibliotecas e comandos oled ------------------------------
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels
// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
//------------------------------------------------------------------------

// dependências DHT11---------------
#include "DHT.h"
#define PINODHT D7

#define MODELODHT DHT11
DHT dht(PINODHT, MODELODHT);


int pinoSensorUmidade = A0;
int valor;
int porcentagem;
unsigned long int ultimo_tempo = 0;
int led = D0;
int aleatorio;
int contador = 0;
float temperatura;
float umidadeDoAr;
int pinBomba = D4;

constexpr char THINGSBOARD_SERVER[] = "demo.thingsboard.io";
constexpr uint16_t THINGSBOARD_PORT = 1883U;
constexpr uint32_t MAX_MESSAGE_SIZE = 128U;

constexpr char ROOT_CERT[] = R"(-----BEGIN CERTIFICATE-----
MIIFazCCA1OgAwIBAgIRAIIQz7DSQONZRGPgu2OCiwAwDQYJKoZIhvcNAQELBQAw
TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh
cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwHhcNMTUwNjA0MTEwNDM4
WhcNMzUwNjA0MTEwNDM4WjBPMQswCQYDVQQGEwJVUzEpMCcGA1UEChMgSW50ZXJu
ZXQgU2VjdXJpdHkgUmVzZWFyY2ggR3JvdXAxFTATBgNVBAMTDElTUkcgUm9vdCBY
MTCCAiIwDQYJKoZIhvcNAQEBBQADggIPADCCAgoCggIBAK3oJHP0FDfzm54rVygc
h77ct984kIxuPOZXoHj3dcKi/vVqbvYATyjb3miGbESTtrFj/RQSa78f0uoxmyF+
0TM8ukj13Xnfs7j/EvEhmkvBioZxaUpmZmyPfjxwv60pIgbz5MDmgK7iS4+3mX6U
A5/TR5d8mUgjU+g4rk8Kb4Mu0UlXjIB0ttov0DiNewNwIRt18jA8+o+u3dpjq+sW
T8KOEUt+zwvo/7V3LvSye0rgTBIlDHCNAymg4VMk7BPZ7hm/ELNKjD+Jo2FR3qyH
B5T0Y3HsLuJvW5iB4YlcNHlsdu87kGJ55tukmi8mxdAQ4Q7e2RCOFvu396j3x+UC
B5iPNgiV5+I3lg02dZ77DnKxHZu8A/lJBdiB3QW0KtZB6awBdpUKD9jf1b0SHzUv
KBds0pjBqAlkd25HN7rOrFleaJ1/ctaJxQZBKT5ZPt0m9STJEadao0xAH0ahmbWn
OlFuhjuefXKnEgV4We0+UXgVCwOPjdAvBbI+e0ocS3MFEvzG6uBQE3xDk3SzynTn
jh8BCNAw1FtxNrQHusEwMFxIt4I7mKZ9YIqioymCzLq9gwQbooMDQaHWBfEbwrbw
qHyGO0aoSCqI3Haadr8faqU9GY/rOPNk3sgrDQoo//fb4hVC1CLQJ13hef4Y53CI
rU7m2Ys6xt0nUW7/vGT1M0NPAgMBAAGjQjBAMA4GA1UdDwEB/wQEAwIBBjAPBgNV
HRMBAf8EBTADAQH/MB0GA1UdDgQWBBR5tFnme7bl5AFzgAiIyBpY9umbbjANBgkq
hkiG9w0BAQsFAAOCAgEAVR9YqbyyqFDQDLHYGmkgJykIrGF1XIpu+ILlaS/V9lZL
ubhzEFnTIZd+50xx+7LSYK05qAvqFyFWhfFQDlnrzuBZ6brJFe+GnY+EgPbk6ZGQ
3BebYhtF8GaV0nxvwuo77x/Py9auJ/GpsMiu/X1+mvoiBOv/2X/qkSsisRcOj/KK
NFtY2PwByVS5uCbMiogziUwthDyC3+6WVwW6LLv3xLfHTjuCvjHIInNzktHCgKQ5
ORAzI4JMPJ+GslWYHb4phowim57iaztXOoJwTdwJx4nLCgdNbOhdjsnvzqvHu7Ur
TkXWStAmzOVyyghqpZXjFaH3pO3JLF+l+/+sKAIuvtd7u+Nxe5AW0wdeRlN8NwdC
jNPElpzVmbUq4JUagEiuTDkHzsxHpFKVK7q4+63SM1N95R1NbdWhscdCb+ZAJzVc
oyi3B43njTOQ5yOf+1CceWxG1bQVs5ZufpsMljq4Ui0/1lvh+wjChP4kqKOJ2qxq
4RgqsahDYVvTH9w7jXbyLeiNdd8XM2w9U/t7y0Ff/9yi0GE44Za4rF2LN9d11TPA
mRGunUHBcnWEvgJBQl9nJEiU0Zsnvgc/ubhPgXRR4Xq37Z0j4r7g1SgEEzwxA57d
emyPxgcYxn/eR44/KJ4EBs+lVDR3veyJm+kXQ99b21/+jh5Xos1AnX5iItreGCc=
-----END CERTIFICATE-----
)";

WiFiClient espClient;
ThingsBoard tb(espClient, MAX_MESSAGE_SIZE);

ESP8266WebServer server(80);

// importanto arquivo HTML -------------------------------------------
#include "index.h"
//------------------------------------------------------------------------

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

void mostrarUmidadeSolo() {
  valor = analogRead(pinoSensorUmidade);
  porcentagem = map(valor, 484, 1023, 100, 0);
  String y = String(porcentagem);
  server.send(200, "text/plane", y);
}

void mostrarTemperatura() {
  temperatura = dht.readTemperature();
  String z = String(temperatura);
  server.send(200, "text/plane", z);
}

void mostrarUmidadeAr() {
  umidadeDoAr = dht.readHumidity();
  String a = String(umidadeDoAr);
  server.send(200, "text/plane", a);
}

void InitWiFi() {
  Serial.println("Connecting to AP ...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to AP");
  Serial.println(WiFi.localIP());
}

bool reconnect() {
  const wl_status_t status = WiFi.status();  // Check to ensure we aren't connected yet
  if (status == WL_CONNECTED) {
    return true;
  }
  InitWiFi();  // If we aren't establish a new connection to the given WiFi network
  return true;
}

void setup() {
  pinMode(D0, OUTPUT);
  pinMode(pinBomba, OUTPUT);
  //Inicializando
  dht.begin();


  //Relacionado ao WIFI------------------------------------------------
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }
  // Hostname defaults to esp8266-[ChipID]
  ArduinoOTA.setHostname("GereMais");

  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else {  // U_FS
      type = "filesystem";
    }

    // NOTE: if updating FS this would be the place to unmount FS using FS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  //----------------------------------------------


  randomSeed(analogRead(0));
  Serial.begin(115200);
  pinMode(pinoSensorUmidade, INPUT);
  delay(1000);
  InitWiFi();

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
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

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
  WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS);  //PASSA OS PARÂMETROS PARA A FUNÇÃO QUE VAI SETAR O IP FIXO NO NODEMCU
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
  server.on("/lerUmidade", mostrarUmidadeSolo);
  server.on("/lerTemperatura", mostrarTemperatura);
  server.on("/lerUmidadeAr", mostrarUmidadeAr);

  server.begin();
}

void loop() {
  server.handleClient();

  if (millis() - ultimo_tempo > 500) {
    digitalWrite(D0, !digitalRead(D0));
    digitalWrite(pinBomba, !digitalRead(pinBomba));
        

    temperatura = dht.readTemperature();
    umidadeDoAr = dht.readHumidity();

    valor = analogRead(pinoSensorUmidade);
    porcentagem = map(valor, 484, 1023, 100, 0);
    Serial.println(porcentagem);

    aleatorio = random(101);
    //delay(1000);
    // Serial.println(porcentagemUmidade);
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
    display.print(temperatura);
    display.println(" *C");

    display.setCursor(0, 24);
    display.print("Umidade do ar: ");
    display.print(umidadeDoAr);
    display.println(" %");

    display.setCursor(0, 36);
    display.print("Umidade solo: ");
    display.print(porcentagem);
    display.println(" %");

    aleatorio = random(101);
    display.setCursor(0, 48);
    display.print("random: ");
    display.print(aleatorio);
    display.display();

    if (!reconnect()) {
      return;
    }

    if (!tb.connected()) {  // Reconnect to the ThingsBoard server, if a connection was disrupted or has not yet been established
      Serial.printf("Connecting to: (%s) with token (%s)\n", THINGSBOARD_SERVER, TOKEN);
      if (!tb.connect(THINGSBOARD_SERVER, TOKEN, THINGSBOARD_PORT)) {
        Serial.println("Failed to connect");
        return;
      }
    }

    // Uploads new telemetry to ThingsBoard using HTTP.
    Serial.println("Sending temperature data...");
    tb.sendTelemetryInt("Valor aleatorio", random(0, 100));
    tb.sendTelemetryInt("Umidade do solo", porcentagem);
    tb.sendTelemetryFloat("Umidade do Ar", umidadeDoAr);
    tb.sendTelemetryFloat("Temperatura", temperatura);
    Serial.print("Random number: ");
    Serial.println(random(0, 100));
    // Serial.println("Sending humidity data...");
    // tb.sendTelemetryFloat("Umidade", random(40, 90));


    tb.loop();

    ultimo_tempo = millis();
  }
  ArduinoOTA.handle();
}
