/*   Código do projeto sensor de temperatura da família DHT
    
     Monte seus próprios projetos sem dificuldade com programação! 
     ACESSE: https://flaviobabos.com.br/
*/

#include "DHT.h"
#define PINODHT D7

// Defina o modelo da família do seu DHT
#define MODELODHT DHT11

DHT dht(PINODHT, MODELODHT);

void setup() {
  Serial.begin(9600);
  
  // Inicialize o sensor
  dht.begin();

  Serial.println("Efetuando leituras!");

}

void loop() {
  
  // Aguarde alguns segundos entre as medidas
  delay(2000);

  // Lendo a umidade
  float Fdht  = dht.readHumidity();
  // Lendo a temperatura na escala Celsius
  float tempC = dht.readTemperature();
  // Lendo a temperatura na escala Fahrenheit
  float tempF = dht.readTemperature(true);

  // Verificando se as leituras falharam
  if (isnan(umi) || isnan(tempC) || isnan(tempF)) {
    Serial.println("Falha na leitura do seu sensor DHT!");
  } else {
    Serial.print("Umidade: ");
    Serial.print(umi);
    Serial.print("%");

    Serial.print("  |  "); 

    Serial.print("Temperatura: ");
    Serial.print(tempC);
    Serial.print("°C ~ ");
    Serial.print(tempF);
    Serial.println("°F");
  }
}