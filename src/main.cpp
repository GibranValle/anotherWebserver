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

void start_exposure() {
  String message = "start exposure";
  webServer.getWebSocketHandler().broadcast(serialize(message, "bot", RUNNING));
  webServer.getWebSocketHandler().broadcast(serialize(message, "handswitch", EXPOSURE));
  globals.updateVariable("handswitch", EXPOSURE);
  globals.updateVariable("bot", RUNNING);
  display.showMainFrame();
}

void start_delayed_exposure() {
  int retraso = globals.getVariable("retraso").toInt();

  if (retraso == 0) start_exposure();

  int retraso_actual = globals.getVariable("retraso_actual").toInt();
  if (retraso - retraso_actual == 0) {
    start_exposure();
    return;
  }

  String message = "delayed exposure";
  webServer.getWebSocketHandler().broadcast(serialize(message, "bot", RUNNING));
  webServer.getWebSocketHandler().broadcast(serialize(message, "handswitch", DELAYED));
  globals.updateVariable("handswitch", DELAYED);
  globals.updateVariable("bot", RUNNING);
}

void end_exposure() {
  String message = "end exposure";
  webServer.getWebSocketHandler().broadcast(serialize(message, "bot", STANDBY));
  webServer.getWebSocketHandler().broadcast(serialize(message, "handswitch", STANDBY));
  globals.updateVariable("handswitch", STANDBY);
  globals.updateVariable("bot", STANDBY);
}

void loop() {
  webServer.loop();
  uartHandler.handleUART();

  // LOOP FOR WIFI ONLINE AND SERIAL OFFLINE
  // THIS IS CONTROLLED WITH MCU
  if (ticked) {
    ticked        = false;
    String modo   = globals.getVariable("modo");
    String bot    = globals.getVariable("bot");
    String serial = globals.getVariable("serial");
    String wifi   = globals.getVariable("wifi");
    String hs     = globals.getVariable("handswitch");
    String message;

    //uint32_t interval = timer0.getInterval();
    //timer0.stop();
    //timer0.setInterval(5 * 1000, timer0ISR);
    if (wifi == ONLINE || hs == EXPOSURE) {
      display.showMainFrame();
      message = "modo: " + modo + ", bot: " + bot + ", hs:" + hs;
      webServer.getWebSocketHandler().broadcast(serialize(message));
    }

    //0)
    if (bot == RUNNING && hs == STANDBY) {
      // RESET 
      globals.updateVariable("retraso_actual", "0");
      globals.updateVariable("duración_actual", "0");
      globals.updateVariable("pausa_actual", "0");
      globals.updateVariable("contador", "0");
      webServer.getWebSocketHandler().broadcast(serialize("", "retraso_actual", "0"));
      webServer.getWebSocketHandler().broadcast(serialize("", "duración_actual", "0"));
      webServer.getWebSocketHandler().broadcast(serialize("", "pausa_actual", "0"));
      webServer.getWebSocketHandler().broadcast(serialize("", "contador", "0"));

      start_delayed_exposure();
      return;
    }

    // 1)
    if (hs == DELAYED) {
      int retraso        = globals.getVariable("retraso").toInt();
      int retraso_actual = globals.getVariable("retraso_actual").toInt();
      if (retraso == 0 || (retraso > 0 && retraso - retraso_actual == 0)) {
        start_exposure();
        return;
      }
      String newValue = String(retraso_actual + 1);
      globals.updateVariable("retraso_actual", newValue);
      webServer.getWebSocketHandler().broadcast(serialize("", "retraso_actual", newValue));
    }

    // 2)
    if (hs == EXPOSURE) {
      int duration  = globals.getVariable("duración").toInt();
      int segundero = globals.getVariable("duración_actual").toInt();
      if (duration - segundero == 0) {
        globals.updateVariable("handswitch", WAITING);
        webServer.getWebSocketHandler().broadcast(serialize("", "handswitch", WAITING));
        return;
      }
      String newValue = String(segundero + 1);
      globals.updateVariable("duración_actual", String(segundero + 1));
      webServer.getWebSocketHandler().broadcast(serialize("", "duración_actual", newValue));
    }

    // 3)
    if (hs == WAITING) {
      int pausa        = globals.getVariable("pausa").toInt();
      int pausa_actual = globals.getVariable("pausa_actual").toInt();
      int contador = globals.getVariable("contador").toInt();
      int total    = globals.getVariable("total").toInt();
      // 4) PENDING EXPOSURES??
      // 4.1 No -> end
      if (total == 1 or (total > 1 && total - contador == 0)) {
        end_exposure();
        return;
      }

      // 4.2 yes -> next exposure
      if (pausa - pausa_actual == 0) {
        String newValue = String(contador + 1);
        globals.updateVariable("contador", newValue);
        webServer.getWebSocketHandler().broadcast(serialize("", "contador", newValue));
        start_delayed_exposure();
        return;
      }
      String newValue = String(pausa_actual + 1);
      globals.updateVariable("pausa_actual", newValue);
      webServer.getWebSocketHandler().broadcast(serialize("", "pausa_actual", newValue));
    }
  }

  // LOOP FOR PINS
  if (ticked1) {
    ticked1   = false;
    String hs = globals.getVariable("handswitch");

    if (hs == EXPOSURE) {
      digitalWrite(LED, HIGH);
      digitalWrite(RELAY, HIGH);
    } else {
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
      webServer.getWebSocketHandler().broadcast(serialize(message, "duración", SHORT));
      globals.updateVariable("duración", SHORT);
      start_delayed_exposure();
    }

    else if (command.startsWith("M")) {
      String message = "M";
      Serial.println(message);
      webServer.getWebSocketHandler().broadcast(serialize(message, "duración", MEDIUM));
      globals.updateVariable("duración", MEDIUM);
      start_delayed_exposure();
    }

    else if (command.startsWith("L")) {
      String message = "L";
      Serial.println(message);
      webServer.getWebSocketHandler().broadcast(serialize(message, "duración", LONG));
      globals.updateVariable("duración", LONG);
      start_delayed_exposure();
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
