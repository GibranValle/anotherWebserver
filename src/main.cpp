#include "UARTHandler.h"
#include "DisplayHandler.h"
#include "GlobalVariables.h"
#include "WebServerHandler.h"
#include "ESP32TimerManager.h"
#include <ArduinoJson.h>

#define LED 13
#define RELAY 12

// Instancias de temporizadores
ESP32TimerManager timer0(0);
ESP32TimerManager timer1(1);

String serialize(String message, String varName = "", String value = "")
{
  JsonDocument doc;
  doc["mensaje"] = message;
  doc["varName"] = varName;
  doc["value"] = value;

  String jsonString;
  serializeJson(doc, jsonString);
  return jsonString;
}

// ISR personalizada para el primer temporizador
bool IRAM_ATTR timer0ISR(void *arg)
{
  Serial.println("timeout");
  return true;
}

// Instancias globales
DisplayHandler display;
GlobalVariables globals(display);
UARTHandler uartHandler;
WebServerHandler webServer;

bool isQR = true;
bool waitExpDone = false;

void setup()
{
  // Configura el pin D13 como salida
  pinMode(LED, OUTPUT);
  pinMode(RELAY, OUTPUT);

  // Establece el pin D13 en alto
  digitalWrite(LED, LOW);
  digitalWrite(RELAY, LOW);
  Serial.begin(115200);

  // Inicializar display para que esté disponible en las otras clases, máxima prioridad
  display.begin();

  // Configura el temporizador para que se dispare cada 1000 ms (1 segundo)
  timer0.begin(5 * 1000, timer0ISR); // Temporizador 0 cada 1000 ms

  // Inicializar UART con interrupciones, segunda prioridad.
  uartHandler.begin(115200);

  // Inicializar WebServer en modo AP
  webServer.begin();

  display.drawQR();
}

void loop()
{
  webServer.loop();

  // Manejar datos desde el UART
  uartHandler.handleUART();

  // Procesar datos recibidos desde UART
  if (uartHandler.hasData())
  {
    String command = uartHandler.read();
    command.trim();

    if (command.startsWith("set"))
    {
      int splitIdx = command.indexOf(' ');
      String var = command.substring(3, splitIdx);
      String value = command.substring(splitIdx + 1);
      globals.updateVariable(var, value);
    }

    else if (command.startsWith("exp"))
    {
      Serial.println("Under exposure");
      display.drawExposure();
    }

    // START EXPOSURE
    else if (command.startsWith("S"))
    {
      String message = "Short exposure";
      ws.textAll(serialize(message, "duration", "short"));
      ws.textAll(serialize(message, "bot", "exposure"));
      ws.textAll(serialize(message, "handswitch", "exposure"));

      Serial.println(message);
      globals.updateVariable("duration", "short");
      globals.setisExpEnding(false);
      globals.setisExpStarting(true);
    }

    // START EXPOSURE
    else if (command.startsWith("M"))
    {
      String message = "Medium exposure";
      ws.textAll(serialize(message, "duration", "medio"));
      ws.textAll(serialize(message, "bot", "exposure"));
      ws.textAll(serialize(message, "handswitch", "exposure"));

      Serial.println(message);
      globals.updateVariable("duration", "medio");
      globals.setisExpEnding(false);
      globals.setisExpStarting(true);
    }

    // START EXPOSURE
    else if (command.startsWith("L"))
    {
      String message = "Long exposure";
      ws.textAll(serialize(message, "duration", "largo"));
      ws.textAll(serialize(message, "bot", "exposure"));
      ws.textAll(serialize(message, "handswitch", "exposure"));

      Serial.println(message);
      globals.updateVariable("duration", "largo");
      globals.setisExpEnding(false);
      globals.setisExpStarting(true);
    }

    // START EXPOSURE
    else if (command.startsWith("X"))
    {
      String message = "End exposure";
      ws.textAll(serialize(message, "bot", "standby"));
      ws.textAll(serialize(message, "handswitch", "standby"));
      Serial.println(message);
      globals.setisExpStarting(false);
      globals.setisExpEnding(false);
      globals.setisExpEnding(true);
    }

    else if (command.startsWith("var"))
    {
      Serial.println("all variables");
      display.showText(globals.generateDisplayText());
    }

    else if (command.startsWith("?"))
    {
      Serial.println("Usa: set<var> <value> (ejemplo: setmode manual)");
      Serial.println("aws: [visible, no visible])");
      Serial.println("generator: [standby, blocked, exposure, offline, unknown])");
      Serial.println("fpd: [standby, exposure, offline, unknown])");
      Serial.println("serial: [online, offline])");
      Serial.println("wifi: [online, offline])");
      Serial.println("bot: [STARTED, STOPPED, PAUSED, LOOP])");
      Serial.println("handswitch: [standy, exposure])");
      Serial.println("duration: [short, medio, largo])");
      Serial.println("pause: [5-600])");
      Serial.println("delay: [5-600])");
      Serial.println("count: [1-44])");
      Serial.println("exp: para ver la animación");
      Serial.println("var: para ver todas las variables");
    }
    else
    {
      Serial.println("Comando no válido. Usa: set<var> <value> (ejemplo: setmode manual) o <?>");
    }
  }
}
