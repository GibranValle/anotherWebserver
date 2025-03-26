#include <ArduinoJson.h>

#include "DisplayHandler.h"
#include "ESP32TimerManager.h"
#include "GlobalVariables.h"
#include "UARTHandler.h"
#include "WebServerHandler.h"
#include "utils.h"  // ✅ Ahora importamos utils.h

#define BAUDRATE 115200
#define LED 13
#define RELAY 12

// Instancias de temporizadores
ESP32TimerManager timer0(0);
ESP32TimerManager timer1(1);
ESP32TimerManager timer2(2);

// Instancias globales
GlobalVariables  globals;
DisplayHandler   display(globals);
UARTHandler      uartHandler;
WebServerHandler webServer(globals);

bool ticked  = false;
bool ticked1 = false;
bool ticked2 = false;
bool qr      = true;

String last_state = "";

bool IRAM_ATTR timer0ISR(void *arg) {
  ticked = true;
  return true;
}

bool IRAM_ATTR timer1ISR(void *arg) {
  ticked1 = true;
  return true;
}

bool IRAM_ATTR timer2ISR(void *arg) {
  ticked2 = true;
  return true;
}

void updateGlobalVariable(String varName, String value) {
  globals.updateVariable(varName, value);
  webServer.getWebSocketHandler().broadcast(serialize("update", varName, value));
}

void setup() {
  pinMode(LED, OUTPUT);
  pinMode(RELAY, OUTPUT);
  digitalWrite(LED, LOW);
  digitalWrite(RELAY, LOW);
  Serial.begin(BAUDRATE);

  display.begin();
  timer0.begin(1000, timer0ISR);
  timer1.begin(500, timer1ISR);
  timer2.begin(5000, timer2ISR);

  uartHandler.begin(BAUDRATE);
  webServer.begin();
}

void loop() {
  webServer.loop();
  uartHandler.handleUART();

  // THIS IS CONTROLLED WITH MCU
  if (ticked) {
    ticked      = false;
    String modo = globals.getVariable("modo");
    String bot  = globals.getVariable("bot");
    if (bot != RUNNING || modo != REMOTO) return;

    String hs       = globals.getVariable("handswitch");
    int    total    = globals.getVariable("total").toInt();
    int    contador = globals.getVariable("contador").toInt();

    webServer.getWebSocketHandler().broadcast(serialize("total: " + String(total) + " ,contador: " + String(contador)));

    // NO EXPOSURES LEFT, END LOOP
    if (total - contador == 0) {
      updateGlobalVariable("bot", STANDBY);
      updateGlobalVariable("handswitch", STANDBY);
      updateGlobalVariable("contador", "0");
      return;
    }

    // FROM STANDBY TO EXPOSURE
    if (hs == STANDBY) {
      // CHECK DELAY FIRST SHOT
      if (contador > 0) {
        updateGlobalVariable("handswitch", EXPOSURE);
        return;
      }
      // CHECK DELAY
      int retraso = globals.getVariable("retraso").toInt();
      if (retraso == 0) {
        updateGlobalVariable("handswitch", EXPOSURE);
        return;
      }
      // DELAY OVER
      int retraso_actual = globals.getVariable("retraso_actual").toInt();
      if (retraso == 0 || retraso - retraso_actual == 0) {
        updateGlobalVariable("handswitch", EXPOSURE);
        updateGlobalVariable("retraso_actual", "0");
        return;
      }
      // INCREMENT
      updateGlobalVariable("retraso_actual", String(retraso_actual + 1));
      return;
    }

    // FROM EXPOSURE TO STANDBY
    else if (hs == EXPOSURE) {
      int duration        = globals.getVariable("duration").toInt();
      int duration_actual = globals.getVariable("duration_actual").toInt();
      if (duration - duration_actual == 0) {
        updateGlobalVariable("duration_actual", "0");
        // END OF EXPOSURE INCREMENT COUNTER
        updateGlobalVariable("contador", String(contador + 1));
        updateGlobalVariable("handswitch", WAITING);
        return;
      }
      // INCREMENT
      updateGlobalVariable("duration_actual", String(duration_actual + 1));
      return;
    }

    // FROM WAITING TO EXPOSURE
    else if (hs == WAITING) {
      int pausa        = globals.getVariable("pausa").toInt();
      int pausa_actual = globals.getVariable("pausa_actual").toInt();
      webServer.getWebSocketHandler().broadcast(
          serialize("pausa: " + String(pausa) + " ,pausa_actual: " + String(pausa_actual)));

      if (pausa - pausa_actual == 0) {
        updateGlobalVariable("handswitch", EXPOSURE);
        updateGlobalVariable("pausa_actual", "0");
        updateGlobalVariable("duration_actual", "0");
        return;
      }
      // INCREMENT
      updateGlobalVariable("pausa_actual", String(pausa_actual + 1));
      return;
    }

    /*
      int total    = globals.getVariable("total").toInt();
      int retraso  = globals.getVariable("retraso").toInt();
      int duration = globals.getVariable("duration").toInt();
      int contador = globals.getVariable("contador").toInt();
      
      webServer.getWebSocketHandler().broadcast(serialize("total: " + String(total) + " ,contador: " + String(contador) +
      " ,retraso: " + String(retraso) +
      " ,duration: " + String(duration)));
      */
  }

  // LOOP FOR PINS
  if (ticked1) {
    ticked1         = false;
    String hs       = globals.getVariable("handswitch");
    String modo     = globals.getVariable("modo");
    int    total    = globals.getVariable("total").toInt();
    int    contador = globals.getVariable("contador").toInt();

    if (hs == EXPOSURE && last_state != EXPOSURE) {
      last_state = EXPOSURE;
      digitalWrite(LED, HIGH);
      digitalWrite(RELAY, HIGH);
    } else if (hs == STANDBY) {
      last_state = STANDBY;
      digitalWrite(LED, LOW);
      digitalWrite(RELAY, LOW);
    }
  }

  // UPDATE DISPLAY EVERY 5s swapping
  if (ticked2) {
    ticked2     = false;
    String wifi = globals.getVariable("wifi");
    String hs   = globals.getVariable("handswitch");

    if (wifi == ONLINE || hs == EXPOSURE) return;
    if (qr) {
      display.drawQR();
      qr = false;
      return;
    }
    if (!qr) {
      qr = true;
      display.showAPInfo();
      return;
    }
  }

  // SERIAL LOGIC
  if (uartHandler.hasData()) {
    String command = uartHandler.read();
    command.trim();

    if (command.startsWith("set")) {
      int    splitIdx = command.indexOf(' ');
      String var      = command.substring(3, splitIdx);
      String value    = command.substring(splitIdx + 1);
      Serial.println(var + "->" + value);
      String message = var + " changed";
      webServer.getWebSocketHandler().broadcast(serialize(message, var, value));
      globals.updateVariable(var, value);
    }

    else if (command.startsWith("getAll")) {
      std::map<String, String> allVars = globals.getAllVariables();
      // Ejemplo: imprimir todas las variables
      for (const auto &pair : allVars) {
        Serial.println(pair.first + ": " + pair.second);
      }
    }

    else if (command.startsWith("exp")) {
      Serial.println("Under exposure icon");
      display.drawExposure();
    }

    else if (command.startsWith("T")) {
      String message = "T";
      Serial.println(message);
      webServer.getWebSocketHandler().broadcast(serialize(message, "serial", ONLINE));
      globals.updateVariable("serial", ONLINE);
      String wifi = globals.getVariable("wifi");
      Serial.println("setwifi" + ' ' + wifi);
    }

    else if (command.startsWith("E")) {
      String message = "E";
      Serial.println(message);
      webServer.getWebSocketHandler().broadcast(serialize(message, "serial", OFFLINE));
      globals.updateVariable("serial", OFFLINE);
    }

    else if (command.startsWith("S")) {
      String message = "S";
      Serial.println(message);
      updateGlobalVariable("duración", SHORT);
      updateGlobalVariable("handswitch", EXPOSURE);
    }

    else if (command.startsWith("M")) {
      String message = "M";
      Serial.println(message);
      updateGlobalVariable("duración", MEDIUM);
      updateGlobalVariable("handswitch", EXPOSURE);
    }

    else if (command.startsWith("L")) {
      String message = "L";
      Serial.println(message);
      updateGlobalVariable("duración", LONG);
      updateGlobalVariable("handswitch", EXPOSURE);
    }

    else if (command.startsWith("X")) {
      String message = "X";
      webServer.getWebSocketHandler().broadcast(serialize(message, "bot", STANDBY));
      webServer.getWebSocketHandler().broadcast(serialize(message, "handswitch", STANDBY));
      globals.updateVariable("bot", STANDBY);
      globals.updateVariable("handswitch", STANDBY);
      Serial.println(message);
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
      Serial.println("duración: [5-330])");                                                    // 3
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
