#include <WiFi.h> // Biblioteca para conexão com Wi-Fi (usada com ESP32)
#include "AdafruitIO_WiFi.h" // Biblioteca da plataforma Adafruit IO (envio de dados)
#include <DHT.h> // Biblioteca para comunicação com o sensor DHT (temperatura e umidade)

const int dhtPin = 14; // Define o pino onde o sensor DHT está conectado (GPIO 14)
DHT dht(dhtPin, DHT22); // Cria um objeto "dht" do tipo DHT, especificando o pino e o modelo do sensor (DHT22)


#define WIFI_SSID "Wokwi-GUEST" // Nome da rede Wi-Fi
#define WIFI_PASS "" // Senha da rede Wi-Fi (vazia porque o Wokwi não exige senha)

#define IO_USERNAME "Nome do Usuário" // Nome de usuário da Adafruit IO
#define IO_KEY "Chave" // Chave de acesso (IO Key) da Adafruit IO


AdafruitIO_WiFi io(IO_USERNAME, IO_KEY, WIFI_SSID, WIFI_PASS); // Cria o objeto responsável por conectar o ESP à plataforma Adafruit IO usando Wi-Fi

AdafruitIO_Feed * suhu = io.feed("temperature"); // Feed para envio da temperatura
AdafruitIO_Feed * humid = io.feed("humidity"); // Feed para envio da umidade

unsigned long previousTime = 0; // Armazena o tempo da última leitura
unsigned long delayTime = 1000; // Define o intervalo entre as leituras (em milissegundos, ou seja, 1 segundo)

void setup()
{
  Serial.begin(115200); // Inicia a comunicação serial com o computador (para depuração)
  io.connect(); // Tenta conectar à Adafruit IO

  // Enquanto não estiver conectado à Adafruit IO
  while (io.status() < AIO_CONNECTED)
  {
    delay(1000); // Espera 1 segundo
    Serial.println("*************");
    Serial.println("******Connecting to Wifi-id******"); // Mensagem indicando que está tentando conectar
  }

  Serial.println("Connected to WiFi"); // Conectado com sucesso
  dht.begin(); // Inicializa o sensor DHT
}


void loop()
{
  io.run(); // Mantém a conexão com a Adafruit IO ativa

  
  float temperature = dht.readTemperature(); // Lê a temperatura do sensor (em graus Celsius)
  float humidity = dht.readHumidity(); // Lê a umidade relativa do ar (em %)


  if (isnan(temperature) || isnan(humidity)) // Se alguma leitura for inválida (NaN)
  {
    Serial.println("Failed to read from DHT sensor!"); // Mostra mensagem de erro
    return; // Sai da função loop() e tenta novamente no próximo ciclo
  }

  
  if (millis() - previousTime >= delayTime) { // Se já passou o tempo definido

    suhu->save(temperature); // Envia a temperatura para a Adafruit IO
    humid->save(humidity); // Envia a umidade para a Adafruit IO

    Serial.print("Temperature Sent is: ");
    Serial.print(temperature);
    Serial.print(" °C\t");

    Serial.print("Humidity Sent is: ");
    Serial.print(humidity);
    Serial.println("%");

    previousTime = millis(); // Armazena o tempo atual para controle do próximo envio
  }
}