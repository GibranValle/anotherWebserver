#ifndef WEBSOCKETHANDLER_H
#define WEBSOCKETHANDLER_H

#include <ArduinoJson.h>
#include <AsyncWebSocket.h>

#include "GlobalVariables.h"
#include "utils.h"  // ✅ Ahora importamos utils.h

class WebSocketHandler {
 private:
  AsyncWebSocket   ws;
  GlobalVariables& globals;

  static WebSocketHandler* instance;

  // ✅ Manejador de eventos WebSocket
  static void onWebSocketEvent(
      AsyncWebSocket* server, AsyncWebSocketClient* client, AwsEventType type, void* arg, uint8_t* data, size_t len) {
    if (!instance) return;
    WebSocketHandler* handler = instance;

    if (type == WS_EVT_CONNECT) {
      Serial.printf("Cliente WebSocket conectado, ID: %u\n", client->id());
      String message = "Websocket connected";
      String varName = "";
      String value   = "";
      client->text(serialize(message, varName, value));
    } else if (type == WS_EVT_DISCONNECT) {
      Serial.printf("Cliente WebSocket desconectado, ID: %u\n", client->id());
    } else if (type == WS_EVT_DATA) {
      String message = "";
      for (size_t i = 0; i < len; i++) {
        message += (char)data[i];
      }

      JsonDocument         doc;
      DeserializationError error = deserializeJson(doc, message);
      if (error) {
        Serial.printf("Error al parsear JSON: %s\n", error.c_str());
        client->text("{\"error\":\"Formato JSON inválido\"}");
        return;
      }

      if (doc["varName"].is<String>() && doc["value"].is<String>()) {
        String varName = doc["varName"];
        String value   = doc["value"];
        Serial.printf("set%s %s\n", varName.c_str(), value.c_str());
        handler->globals.updateVariable(varName, value);
        String message = "update";
        client->text(serialize(message, varName, value));
      }
    }
  }

 public:
  WebSocketHandler(GlobalVariables& globals) : ws("/ws"), globals(globals) {
    instance = this;
    ws.onEvent(onWebSocketEvent);
  }

  void attachToServer(AsyncWebServer& server) {
    server.addHandler(&ws);
  }

  void broadcast(const String& message) {
    ws.textAll(message);
  }

  void loop() {
    ws.cleanupClients();
  }
};

WebSocketHandler* WebSocketHandler::instance = nullptr;

#endif  // WEBSOCKETHANDLER_H
