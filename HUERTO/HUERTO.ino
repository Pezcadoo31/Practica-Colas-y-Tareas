#include <Arduino_FreeRTOS.h>
#include <queue.h>
#include <DHT.h>

// Definir el pin y tipo de sensor DHT
#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

int SENSOR;
int lectura;

// Definir los pines de los LEDs
#define LED_HUMIDITY 3
#define LED_TEMPERATURE 4
#define LED_LIGHT 5

// Variables globales para almacenar la temperatura y la humedad
volatile float temperature = 0;
volatile float humidity = 0;

// Declaración de colas para temperatura, humedad y luz
QueueHandle_t tempQueue;
QueueHandle_t humQueue;
QueueHandle_t lightQueue;

void setup() {
  // Inicializar el puerto serial
  Serial.begin(9600);

  // Inicializar el sensor DHT
  dht.begin();

  // Configurar los pines de los LEDs como salida
  pinMode(LED_HUMIDITY, OUTPUT);
  pinMode(LED_TEMPERATURE, OUTPUT);
  pinMode(LED_LIGHT, OUTPUT);

  // Crear las colas
  tempQueue = xQueueCreate(10, sizeof(float));
  humQueue = xQueueCreate(10, sizeof(float));
  lightQueue = xQueueCreate(10, sizeof(int));

  // Verificar si las colas se crearon correctamente
  if (tempQueue != NULL && humQueue != NULL && lightQueue != NULL) {
    // Crear las tareas
    xTaskCreate(TaskReadTemperature, "ReadTemp", 128, NULL, 1, NULL);
    xTaskCreate(TaskReadHumidity, "ReadHum", 128, NULL, 1, NULL);
    xTaskCreate(TaskLightLevel, "LightLevel", 128, NULL, 1, NULL);
    xTaskCreate(TaskSerialOutput, "SerialOut", 128, NULL, 2, NULL);
    xTaskCreate(TaskControlLEDs, "ControlLEDs", 128, NULL, 2, NULL);
  } else {
    // Error al crear las colas
    Serial.println("Error al crear las colas");
  }
}

void loop() {
  // El loop está vacío ya que FreeRTOS gestiona las tareas
}

// Tarea para leer la temperatura del sensor DHT11
void TaskReadTemperature(void *pvParameters) {
  (void) pvParameters;
  
  for (;;) {
    // Leer la temperatura
    float temp = dht.readTemperature();
    
    // Si la lectura es válida, enviar a la cola de temperatura
    if (!isnan(temp)) {
      xQueueSend(tempQueue, &temp, portMAX_DELAY);
    } else {
      Serial.println("Error al leer temperatura");
    }

    // Esperar 2 segundos antes de leer nuevamente
    vTaskDelay(2000 / portTICK_PERIOD_MS);
  }
}

// Tarea para leer la humedad del sensor DHT11
void TaskReadHumidity(void *pvParameters) {
  (void) pvParameters;
  
  for (;;) {
    // Leer la humedad
    SENSOR = analogRead(A0);

    // Si la lectura es válida, enviar a la cola de humedad
    if (!isnan(SENSOR)) {
      xQueueSend(humQueue, &SENSOR, portMAX_DELAY);
    } else {
      Serial.println("Error al leer humedad");
    }

    // Esperar 2 segundos antes de leer nuevamente
    vTaskDelay(2000 / portTICK_PERIOD_MS);
  }
}

// Tarea para imprimir el nivel de luz
void TaskLightLevel(void *pvParameters) {
  (void) pvParameters;

  lectura = analogRead(A1);

  for (;;) {
    // Alternar entre "mucha luz" y "poca luz"
    lectura = !lectura;
    xQueueSend(lightQueue, &lectura, portMAX_DELAY);

    if (lightLevel) {
      Serial.println("Mucha luz");
    } else {
      Serial.println("Poca luz");
    }

    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

// Tarea para enviar datos al puerto serial
void TaskSerialOutput(void *pvParameters) {
  (void) pvParameters;

  float tempValue;
  float humValue;
  int lightValue;

  for (;;) {
    // Recibir y imprimir los valores de temperatura y humedad en el puerto serial
    if (xQueueReceive(tempQueue, &tempValue, portMAX_DELAY) == pdPASS) {
      Serial.print("Temperatura: ");
      Serial.print(tempValue);
      Serial.println(" *C");
    }

    if (xQueueReceive(humQueue, &humValue, portMAX_DELAY) == pdPASS) {
      Serial.print("Humedad: ");
      Serial.print(humValue);
      Serial.println(" %");
    }

    if (xQueueReceive(lightQueue, &lightValue, portMAX_DELAY) == pdPASS) {
      if (lightValue) {
        Serial.println("Mucha luz");
      } else {
        Serial.println("Poca luz");
      }
    }

    vTaskDelay(2000 / portTICK_PERIOD_MS);
  }
}

// Tarea para controlar los LEDs
void TaskControlLEDs(void *pvParameters) {
  (void) pvParameters;

  float tempValue;
  float humValue;
  int lightValue;

  for (;;) {
    // Recibir los valores de las colas y controlar los LEDs
    if (xQueueReceive(tempQueue, &tempValue, portMAX_DELAY) == pdPASS) {
      if (tempValue > 22) {  // Umbral de temperatura alta
        digitalWrite(LED_TEMPERATURE, HIGH);
      } else {
        digitalWrite(LED_TEMPERATURE, LOW);
      }
    }

    if (xQueueReceive(humQueue, &humValue, portMAX_DELAY) == pdPASS) {
      if (humValue > 100) {  // Umbral de humedad baja
        digitalWrite(LED_HUMIDITY, HIGH);
      } else {
        digitalWrite(LED_HUMIDITY, LOW);
      }
    }

    if (xQueueReceive(lightQueue, &lightValue, portMAX_DELAY) == pdPASS) {
      if (lightValue < 840) {  // Poca luz
        digitalWrite(LED_LIGHT, HIGH);
      } else {
        digitalWrite(LED_LIGHT, LOW);
      }
    }

    vTaskDelay(200 / portTICK_PERIOD_MS);  // Ajustar este delay según sea necesario
  }
}
