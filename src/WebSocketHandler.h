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
      //Serial.printf("Cliente WebSocket conectado, ID: %u\n", client->id());
      // Actualizar todas las variables al conectar
      handler->updateAllVariables(client);
      
      int    connected = handler->getConnectedClientsCount();
      String message   = "Websocket connected";
      String varName   = "wifi";
      String value     = OFFLINE;
      if (connected > 0) value = ONLINE;
      client->text(serialize(message, varName, value));
      handler->globals.updateVariable(varName, value);
      handler->globals.sendVariable(varName, value);

      varName = "handswitch";
      value   = "standby";
      client->text(serialize(message, varName, value));
      handler->globals.updateVariable(varName, value);
    } else if (type == WS_EVT_DISCONNECT) {
      int    connected = handler->getConnectedClientsCount();
      String message   = "Websocket disconnected";
      String varName   = "wifi";
      String value     = OFFLINE;
      if (connected == 0) value = OFFLINE;
      client->text(serialize(message, varName, value));
      handler->globals.sendVariable(varName, value);

      //Serial.printf("Cliente WebSocket desconectado, ID: %u\n", client->id());
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

  int getConnectedClientsCount() {
    int count = 1;
    for (uint8_t i = 0; i < ws.count(); ++i) {
      AsyncWebSocketClient* client = ws.client(i);
      if (client && client->status() == WS_CONNECTED) {
        count++;
      }
    }
    return count;
  }

  void updateAllVariables(AsyncWebSocketClient* client) {
    if (!client || client->status() != WS_CONNECTED) return;  // Añadido control de cliente

    std::map<String, String> allVariables = globals.getAllVariables();
    for (const auto& pair : allVariables) {
      String message = "update";
      client->text(serialize(message, pair.first, pair.second));
    }
  }
};

WebSocketHandler* WebSocketHandler::instance = nullptr;

#endif  // WEBSOCKETHANDLER_H
