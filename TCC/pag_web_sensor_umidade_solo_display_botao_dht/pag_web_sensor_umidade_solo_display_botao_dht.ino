// #include <WiFi.h> //lib para configuração do Wifi (para esp32)
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

#include <ESP8266mDNS.h>
// #include <ESPmDNS.h> //lib necessária para comunicação network DO ESP32
#include <WiFiUdp.h>     //lib necessária para comunicação network
#include <ArduinoOTA.h>  //primeiro gravar sem ip fixo, e qd o ota estiver funcionar (ver ip na lista de portas) gravar com ip fixo

// bibliotecas e comandos oled ------------------------------
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels
// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
//------------------------------------------------------------------------

//Sensor DHTT11 - dependencias
#include "DHT.h"
#define PINODHT D7
#define MODELODHT DHT11
DHT dht(PINODHT, MODELODHT);
float temp;
float umi;

//=============================
#define THINGSBOARD_ENABLE_PROGMEM 0  //não remover essa linha! (ao remover o nodemcu ficará reiniciando sozinho)
#include "ThingsBoard.h"
// #include "pubsubclient.h"
#define WIFI_AP "IFAL - Rio Largo"
#define WIFI_PASSWORD "ifalriolargo"
#define TOKEN "WLMk433vZ1vkwPaoKmxl"

const char* mqtServer = "demo.thingsboard.io";
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

// const char* mqtUser = "nmais";
// const char* mqtPassword = "2023";
// const char* mqtClient = "MaisEnsino"


WiFiClient espClient;
ThingsBoard tb(espClient);

// int status = WL_IDLE_STATUS;
// #define THINGSBOARD_SERVER "demo.thingsboard.io"


//============Thingsboard==========


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


// IPAddress local_IP(10, 220, 30, 116);
// IPAddress gateway(10, 220, 30, 1);
// IPAddress subnet(255, 255, 255, 0);
// IPAddress primaryDNS(8, 8, 8, 8);
// IPAddress secondaryDNS(8, 8, 4, 4);


int led = D0;
unsigned long int ultimo = 0;
int contador = 0;
int aleatorio;

//Início umidade
const int pinoSensorUmidade = A0;
int valorUmidade;
int porcentagemUmidade;
//Fim umidade variáveis

//Início luminosidade - variáveis
int sensorLuminosidade;
int valorLuminosidade;
int porcentagemLuminosidade;
//Fim luminosidade - variáveis

#include "index.h"            //pagina HTML com conteudo javascripts
ESP8266WebServer server(80);  //Server on port 80
//============================================================
void raiz() {
  String s = pagina;                 //l  ê o conteudo HTML
  server.send(200, "text/html", s);  //envia a página web
}

void gerar_numero() {
  //int a = analogRead(A0);
  aleatorio = random(100);
  String x = String(aleatorio);
  server.send(200, "text/plane", x);  //Envia o valor aleatório
}

void umidade() {
  // aleatorio = random(20);
  valorUmidade = analogRead(pinoSensorUmidade);
  porcentagemUmidade = map(valorUmidade, 640, 1024, 100, 0);
  //delay(1000);
  String umidadeString = String(porcentagemUmidade);
  server.send(200, "text/plane", umidadeString);
}

void umidadeTempo() {
  umi = dht.readHumidity();
  String umidadeTempoString = String(umi);
  server.send(200, "text/plane", umidadeTempoString);
  float tempC = dht.readTemperature();



  // if (isnan(umi)) {
  //   display.println("Falha na leitura do sensor DHT!");
  // } else {
  //   delay(50);
  //   display.clearDisplay();
  //   display.setTextSize(1);
  //   display.setTextColor(WHITE);
  //   display.setCursor(0, 0);  //coluna, linha
  //   //==========umidade tempo========//
  //   display.print("Umidade: ");
  //   display.print(umi);
  //   display.println("%");
  //   display.print("Temperatura: ");
  //   display.println(tempC);
  //   display.display();
  //   delay(1000);
  // }
}

void temperatura() {
  temp = dht.readTemperature();
  String temperaturaString = String(temp);
  server.send(200, "text/plane", temperaturaString);
  /*
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
    display.print("Temperatura: ");
    display.println(temp);
    display.println("Instituto Federal de Alagoas");
    display.display();
    //--------------------------------
    contador++;
    ultimo = millis();
  }
  */
}
//============================================================

void setup() {
  Serial.begin(115200);
  dht.begin();
  pinMode(led, OUTPUT);
  pinMode(pinoSensorUmidade, INPUT);
  digitalWrite(led, 0);  //led apagado

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

  //inicio comandos OTA -------------------------------
  // Port defaults to 8266
  // ArduinoOTA.setPort(8266);

  // Hostname defaults to esp8266-[ChipID]

  // No authentication by default
  //ArduinoOTA.setPassword("admin");

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else {  // U_FS
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
  server.on("/", raiz);  //trata a requisição "/" na função raiz
  server.on("/lerNUM", gerar_numero);
  server.on("/lerUmidade", umidade);
  server.on("/lerUmidadeTempo", umidadeTempo);
  server.on("/lerTemperatura", temperatura);
  server.begin();
  Serial.println("Servidor HTTP iniciado");
}

void loop() {

  valorUmidade = analogRead(pinoSensorUmidade);
  porcentagemUmidade = map(valorUmidade, 640, 1024, 100, 0);

  ArduinoOTA.handle();    //Escuta OTA
  server.handleClient();  //Escuta requisição dos clientes

  if (millis() - ultimo > 500) {
    temperatura();
    umidadeTempo();

    digitalWrite(led, !digitalRead(led));
    Serial.println(contador);
    //----comandos para atualizar display OTA ---------------------------
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    //display.println(WiFi.localIP());
    //display.setCursor(0, 15);
    // display.println(contador);

    display.setCursor(0, 15);
    display.print("Temperatura: ");
    display.print(temp);
    display.println(" *C");

    display.setCursor(0, 30);
    display.print("Umidade do ar: ");
    display.print(umi);
    display.println(" %");

    display.setCursor(0, 45);
    display.print("Umidade solo: ");
    display.print(porcentagemUmidade);
    display.println(" %");
    display.display();

    tb.sendTelemetryInt("Porcentagem umidade", porcentagemUmidade);
    Serial.print("Umidade do solo: ");
    Serial.println(porcentagemUmidade);
    //display.setCursor(0, 45);
    //display.println("Instituto Federal de Alagoas");
    tb.loop();

    //--------------------------------
    contador++;
    ultimo = millis();
  }
}
