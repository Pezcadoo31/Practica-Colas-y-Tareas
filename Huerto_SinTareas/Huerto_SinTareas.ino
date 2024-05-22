#include <DHT.h>  // Biblioteca para manejar el sensor DHT

// Definición del pin y tipo de sensor DHT
#define DHTPIN 2
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);  // Inicialización del sensor DHT

int SENSOR;  // Variable para almacenar la lectura del sensor de humedad
int lectura; // Variable para almacenar la lectura del sensor de luz

// Definir los pines de los LEDs
#define LED_HUMIDITY 3
#define LED_TEMPERATURE 4
#define LED_LIGHT 5

void setup() {
  // Configuración inicial
  Serial.begin(9600);  // Inicializar la comunicación serial
  pinMode(LED_HUMIDITY, OUTPUT);  // Configurar el pin del LED de humedad como salida
  pinMode(LED_TEMPERATURE, OUTPUT);  // Configurar el pin del LED de temperatura como salida
  pinMode(LED_LIGHT, OUTPUT);  // Configurar el pin del LED de luz como salida
  dht.begin();  // Inicializar el sensor DHT
}

void loop() {
  // Bucle principal
  ReadHum();  // Leer y procesar la humedad
  delay(1000);  // Esperar 1 segundo
  ReadLight();  // Leer y procesar el nivel de luz
  delay(1000);  // Esperar 1 segundo
  ReadTemp();  // Leer y procesar la temperatura
}

// Función para leer la humedad del sensor en el pin A0
void ReadHum() {
  SENSOR = analogRead(A0);  // Leer el valor del sensor de humedad
  Serial.print("Presencia de agua: ");
  Serial.println(SENSOR);  // Imprimir el valor de la humedad en la consola serial

  // Encender o apagar el LED de humedad según el valor leído
  if (SENSOR > 100) {
    digitalWrite(LED_HUMIDITY, HIGH);  // Encender el LED si la humedad es mayor a 100
  } else {
    digitalWrite(LED_HUMIDITY, LOW);  // Apagar el LED si la humedad es menor o igual a 100
  }
}

// Función para leer el nivel de luz del sensor en el pin A1
void ReadLight() {
  lectura = analogRead(A1);  // Leer el valor del sensor de luz
  Serial.print("Luz: ");
  Serial.println(lectura);  // Imprimir el valor de la luz en la consola serial

  // Encender o apagar el LED de luz según el valor leído
  if (lectura < 840) {
    digitalWrite(LED_LIGHT, HIGH);  // Encender el LED si el nivel de luz es menor a 840
  } else {
    digitalWrite(LED_LIGHT, LOW);  // Apagar el LED si el nivel de luz es mayor o igual a 840
  }
}

// Función para leer la temperatura del sensor DHT11
void ReadTemp() {
  float temperature = dht.readTemperature();  // Leer la temperatura del sensor DHT11

  // Verificar si la lectura es válida
  if (isnan(temperature)) {
    Serial.println("Error al leer del sensor DHT!");  // Imprimir mensaje de error si la lectura no es válida
    return;  // Salir de la función
  }

  // Imprimir el valor de la temperatura en la consola serial
  Serial.print("Temperatura: ");
  Serial.print(temperature);
  Serial.println(" *C");

  // Encender o apagar el LED de temperatura según el valor leído
  if (temperature > 22) {
    digitalWrite(LED_TEMPERATURE, HIGH);  // Encender el LED si la temperatura es mayor a 22 grados Celsius
  } else {
    digitalWrite(LED_TEMPERATURE, LOW);  // Apagar el LED si la temperatura es menor o igual a 22 grados Celsius
  }
}

