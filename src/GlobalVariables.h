#ifndef GLOBALVARIABLES_H
#define GLOBALVARIABLES_H

#include "DisplayHandler.h"

class GlobalVariables {
private:
  String aws;
  String generator;
  String fpd;

  String serial;
  String wifi;

  String bot;
  String handswitch;

  String duration;
  short pause;
  short delay;
  short count;

  bool web_viewed;
  bool isExpStarting;
  bool isExpEnding;

  // objetos
  DisplayHandler& display;

public:
  GlobalVariables(DisplayHandler& display)
    : generator("unknown"), aws("unknown"), fpd("unknown"), serial("offline"), wifi("offline"), bot("STOPPED"), duration("short"),
      web_viewed(false), isExpStarting(false), isExpEnding(false), display(display), pause(30), delay(0), count(1) {}


  bool checkWebViewed() {
    return web_viewed;
  }

  void setWebViewed() {
    web_viewed = true;
  }

  bool getisExpStarting() {
    return isExpStarting;
  }

  void setisExpStarting(bool value) {
    isExpStarting = value;
  }

  bool getisExpEnding() {
    return isExpEnding;
  }

  void setisExpEnding(bool value) {
    isExpEnding = value;
  }

  void updateVariable(const String& name, const String& value) {
    // Validar y actualizar cada variable
    if (name == "generator") {
      if (value == "standby" || value == "blocked" || value == "exposure" || value == "offline" || value == "unknown") {
        generator = value;
      } else {
        Serial.println("Error: valor inválido para 'generator'");
        return;
      }
    } else if (name == "aws") {
      if (value == "visible" || value == "hidden" || value == "unknown") {
        aws = value;
      } else {
        Serial.println("Error: valor inválido para 'aws'");
        return;
      }
    } else if (name == "fpd") {
      if (value == "standby" || value == "exposure" || value == "offline" || value == "unknown") {
        fpd = value;
      } else {
        Serial.println("Error: valor inválido para 'fpd'");
        return;
      }
    } else if (name == "serial" || name == "wifi") {
      if (value == "online" || value == "offline") {
        serial = value;
      } else {
        Serial.println("Error: valor inválido para 'serial'");
        return;
      }
    } else if (name == "bot") {
      if (value == "STARTED" || value == "STOPPED" || value == "PAUSED" || value == "LOOP") {
        bot = value;
      } else {
        Serial.println("Error: valor inválido para 'bot'");
        return;
      }
    } else if (name == "handswitch") {
      if (value == "standby" || value == "exposure") {
        bot = value;
      } else {
        Serial.println("Error: valor inválido para 'bot'");
        return;
      }
    } else if (name == "duration") {
      if (value == "short" || value == "medio" || value == "largo") {
        duration = value;
      } else {
        Serial.println("Error: valor inválido para 'duration'");
        return;
      }
    } else if (name == "pause") {
      int pauseValue = value.toInt();
      if (pauseValue >= 5 && pauseValue <= 600) {
        pause = pauseValue;
      } else {
        Serial.println("Error: valor inválido para 'pause' (rango: 5-600)");
        return;
      }
    } else if (name == "delay") {
      int pauseValue = value.toInt();
      if (pauseValue >= 0 && pauseValue <= 600) {
        pause = pauseValue;
      } else {
        Serial.println("Error: valor inválido para 'pause' (rango: 0-600)");
        return;
      }
    } else if (name == "count") {
      int pauseValue = value.toInt();
      if (pauseValue >= 0 && pauseValue <= 44) {
        pause = pauseValue;
      } else {
        Serial.println("Error: valor inválido para 'count' (rango: 0-44)");
        return;
      }
    } else {
      Serial.println("Error: variable inválida '" + name + "'");
      return;
    }

    // Confirmar actualización de la variable
    Serial.println("set" + name + " " + value);
    display.showText(generateDisplayText());
  }

  String getVariable(const String& name) const {
    if (name == "aws") return aws;
    if (name == "generator") return generator;
    if (name == "fpd") return fpd;
    if (name == "serial") return serial;
    if (name == "wifi") return wifi;
    if (name == "duration") return duration;
    if (name == "bot") return bot;
    if (name == "handswitch") return handswitch;
    if (name == "pause") return String(pause);  // Conversión explícita
    if (name == "delay") return String(delay);  // Conversión explícita
    if (name == "count") return String(count);  // Conversión explícita
    return "";
  }

  String generateDisplayText() const {
    return "Generator: " + generator + "\n" + " FPD: " + " AWS: " + aws + "\n" + fpd + "\n" + " Serial: " + serial + "\n" + " Wifi: " + wifi + "\n" + " Bot: " + bot + "\n" + " Handswitch: " + handswitch + "\n";
  }
};

#endif  // GLOBALVARIABLES_H
