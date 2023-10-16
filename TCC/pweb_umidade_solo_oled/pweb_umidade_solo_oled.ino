
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

// importanto arquivo HTML -------------------------------------------
#include "index.h"
//------------------------------------------------------------------------

// dependência thingsboard
#include "ThingsBoard.h"
#define THINGSBOARD_ENABLE_PROGMEM 0  //não remover essa linha! (ao remover o nodemcu ficará reiniciando sozinho)

constexpr char WIFI_SSID[] = "IFAL - Rio Largo";
constexpr char WIFI_PASSWORD[] = "ifalriolargo";
constexpr char TOKEN[] = "WLMk433vZ1vkwPaoKmxl";

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

void InitWiFi() {
  Serial.println("Connecting to AP ...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to AP");
}

bool reconnect() {
  const wl_status_t status = WiFi.status();  // Check to ensure we aren't connected yet
  if (status == WL_CONNECTED) {
    return true;
  }
  InitWiFi();  // If we aren't establish a new connection to the given WiFi network
  return true;
}

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
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  pinMode(pinoSensorUmidade, INPUT);
  pinMode(led, OUTPUT);
  InitWiFi();

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
  // server.handleClient();

    delay(500);
    valor = analogRead(pinoSensorUmidade);
    porcentagemUmidade = map(valor, 484, 1023, 100, 0);
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

    // mostrando no thingsboard

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

    tb.sendTelemetryInt("Valor aleatório", aleatorio);
    Serial.print("Valor aleatorio: ");
    Serial.println(aleatorio);
    tb.loop();

}
