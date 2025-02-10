#include <ArduinoJson.h>

#include "DisplayHandler.h"
#include "ESP32TimerManager.h"
#include "GlobalVariables.h"
#include "UARTHandler.h"
#include "WebServerHandler.h"
#include "utils.h"  // ✅ Ahora importamos utils.h

#define BAUDRATE 9600
#define LED 13
#define RELAY 12

// Instancias de temporizadores
ESP32TimerManager timer0(0);

// Instancias globales
DisplayHandler   display;
GlobalVariables  globals(display);
UARTHandler      uartHandler;
WebServerHandler webServer(globals);

// ISR personalizada para el primer temporizador
bool IRAM_ATTR timer0ISR(void *arg) {
  //Serial.println("timeout");
  return true;
}

void setup() {
  pinMode(LED, OUTPUT);
  pinMode(RELAY, OUTPUT);
  digitalWrite(LED, LOW);
  digitalWrite(RELAY, LOW);
  Serial.begin(BAUDRATE);

  display.begin();
  timer0.begin(5 * 1000, timer0ISR);
  uartHandler.begin(BAUDRATE);
  webServer.begin();

  // initial QR before connected device
  display.drawQR();
}

void loop() {
  webServer.loop();
  uartHandler.handleUART();

  // SERIAL LOGIC
  if (uartHandler.hasData()) {
    String command = uartHandler.read();
    command.trim();
    display.showText(command);

    if (command.startsWith("set")) {
      int    splitIdx = command.indexOf(' ');
      String var      = command.substring(3, splitIdx);
      String value    = command.substring(splitIdx + 1);
      Serial.println(var + "->" + value);
      String message = var + " changed";

      webServer.getWebSocketHandler().broadcast(serialize(message, var, value));
      globals.updateVariable(var, value);
    }

    else if (command.startsWith("exp")) {
      Serial.println("Under exposure");
      display.drawExposure();
    }

    else if (command.startsWith("S")) {
      String message = "Short exposure";
      webServer.getWebSocketHandler().broadcast(serialize(message, "duration", SHORT));
      webServer.getWebSocketHandler().broadcast(serialize(message, "bot", EXPOSURE));
      webServer.getWebSocketHandler().broadcast(serialize(message, "handswitch", EXPOSURE));

      Serial.println(message);
      globals.updateVariable("duration", SHORT);
      globals.updateVariable("bot", EXPOSURE);
      globals.updateVariable("duration", SHORT);

      globals.showMainFrame();
    }

    else if (command.startsWith("M")) {
      String message = "Medium exposure";
      webServer.getWebSocketHandler().broadcast(serialize(message, "duration", MEDIUM));
      webServer.getWebSocketHandler().broadcast(serialize(message, "bot", EXPOSURE));
      webServer.getWebSocketHandler().broadcast(serialize(message, "handswitch", EXPOSURE));

      Serial.println(message);
      globals.updateVariable("duration", MEDIUM);
      globals.showMainFrame();
    }

    else if (command.startsWith("L")) {
      String message = "Long exposure";
      webServer.getWebSocketHandler().broadcast(serialize(message, "duration", LONG));
      webServer.getWebSocketHandler().broadcast(serialize(message, "bot", EXPOSURE));
      webServer.getWebSocketHandler().broadcast(serialize(message, "handswitch", EXPOSURE));

      Serial.println(message);
      globals.updateVariable("duration", LONG);
      globals.showMainFrame();
    }

    else if (command.startsWith("X")) {
      String message = "End exposure";
      webServer.getWebSocketHandler().broadcast(serialize(message, "bot", STANDBY));
      webServer.getWebSocketHandler().broadcast(serialize(message, "handswitch", STANDBY));
      Serial.println(message);
    }

    else if (command.startsWith("var")) {
      Serial.println("all variables");
      display.showText(globals.generateDisplayText());
    }

    else if (command.startsWith("?")) {
      Serial.println("Usa: set<var> <value> (ejemplo: setmode manual)");
      Serial.println("generator: [standby, blocked, exposure, offline, unknown])");            // 5
      Serial.println("fpd: [standby, exposure, offline, unknown, calibrating, calibrated])");  // 6
      Serial.println("serial: [online, offline, error])");                                     // 3
      Serial.println("wifi: [online, offline, # of clients])");
      Serial.println("bot: [running, paused, waiting, delayed, standby, offline, unknown])");  // 7
      Serial.println("handswitch: [standby, exposure, offline, unknown, waiting])");           // 5
      Serial.println("modo: [manual, semi, auto])");                                           // 3
      Serial.println("duration: [short, medium, long])");                                      // 3
      Serial.println("pausa: [5-600])");
      Serial.println("delay: [5-600])");
      Serial.println("contador: [1-100])");
      Serial.println("total: [1-100])");
      Serial.println("exp: para ver la animación");
      Serial.println("var: para ver todas las variables");
      Serial.println("S: para empezar disparo corto [5s]");
      Serial.println("M: para empezar disparo medio [15s]");
      Serial.println("L: para empezar disparo largo [330s]");
      Serial.println("X: para terminar disparo");
    } else {
      Serial.println("Comando no válido. Usa: set<var> <value> (ejemplo: setmode manual) o <?>");
    }
  }
}
