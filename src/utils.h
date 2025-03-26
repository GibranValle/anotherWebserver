#ifndef UTILS_H
#define UTILS_H

#include <Arduino.h>
#include <ArduinoJson.h>

// ✅ Función para serializar mensajes JSON
String serialize(String message, String varName = "", String value = "") {
  JsonDocument doc;
  doc["message"] = message;
  doc["varName"] = varName;
  doc["value"]   = value;

  String jsonString;
  serializeJson(doc, jsonString);
  return jsonString;
}

#endif  // UTILS_H
