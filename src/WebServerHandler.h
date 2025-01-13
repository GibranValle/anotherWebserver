#ifndef WEBSERVERHANDLER_H
#define WEBSERVERHANDLER_H

#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <AsyncWebSocket.h>
#include "html.h"
#include "styles.h"
#include "script.h"

#include <ArduinoJson.h>

// Configuración del Access Point
#define AP_SSID "ESP32_AP"
#define AP_PASSWORD "12345678"

// Declarar el WebSocket como externo
extern AsyncWebSocket ws;

// Clase para manejar el servidor web
class WebServerHandler
{
private:
  AsyncWebServer server; // Servidor web

  // Callback para mensajes del WebSocket
  static void onWebSocketEvent(AsyncWebSocket *server, AsyncWebSocketClient *client,
                               AwsEventType type, void *arg, uint8_t *data, size_t len)
  {
    if (type == WS_EVT_CONNECT)
    {
      Serial.printf("Cliente WebSocket conectado, ID: %u\n", client->id());
      JsonDocument doc;
      doc["mensaje"] = "Bienvenido al WebSocket";
      doc["estado"] = "exitoso";

      String jsonString;
      serializeJson(doc, jsonString);
      client->text(jsonString);
    }
    else if (type == WS_EVT_DISCONNECT)
    {
      Serial.printf("Cliente WebSocket desconectado, ID: %u\n", client->id());
    }
    else if (type == WS_EVT_DATA)
    {
      // Recibe el mensaje del cliente
      String message = "";
      for (size_t i = 0; i < len; i++)
      {
        message += (char)data[i];
      }
      JsonDocument doc;
      String parsedMessage;
      DeserializationError error = deserializeJson(doc, message);

      if (error)
      {
        Serial.printf("Error al parsear JSON: %s\n", error.c_str());
        client->text("{\"error\":\"Formato JSON inválido\"}");
        return;
      }

      Serial.printf("Mensaje recibido: %s\n", message.c_str());

      if (doc.containsKey("varName") && doc.containsKey("value"))
      {
        JsonDocument doc2;
        doc2["varName"] = doc["varName"];
        doc2["value"] = doc["value"];
        String jsonString;
        serializeJson(doc, jsonString);
        // Envía una respuesta al cliente
        client->text(jsonString);
      }
    }
  }

public:
  WebServerHandler() : server(80) {}

  void begin()
  {
    // Configura el Access Point
    bool apStarted = WiFi.softAP(AP_SSID, AP_PASSWORD);

    if (apStarted)
    {
      Serial.println("Punto de acceso iniciado");
      Serial.print("SSID: ");
      Serial.println(AP_SSID);
      Serial.print("Dirección IP: ");
      Serial.println(WiFi.softAPIP());
    }
    else
    {
      Serial.println("Error al iniciar el punto de acceso");
    }

    // Ruta para servir la página HTML
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(200, "text/html", INDEX_HTML_TEMPLATE); });

    // Ruta para servir los estilos CSS
    server.on("/styles.css", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(200, "text/css", STYLES_CSS); });

    // Ruta para servir el script JavaScript
    server.on("/script.js", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(200, "application/javascript", SCRIPT_JS); });

    // SIN CONTENIDO
    server.on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(204); });

    // Configura el WebSocket
    ws.onEvent(onWebSocketEvent); // Registra el callback de eventos
    server.addHandler(&ws);       // Agrega el WebSocket al servidor

    // Inicia el servidor web
    server.begin();
    Serial.println("Servidor web iniciado");
  }

  // Maneja las conexiones WebSocket
  void loop()
  {
    ws.cleanupClients(); // Limpia clientes desconectados
  }
};

// Inicializa el WebSocket como una variable global
AsyncWebSocket ws("/ws");

#endif // WEBSERVERHANDLER_H
