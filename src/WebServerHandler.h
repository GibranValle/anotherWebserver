#ifndef WEBSERVERHANDLER_H
#define WEBSERVERHANDLER_H

#include <DNSServer.h>
#include <ESPAsyncWebServer.h>
#include <WiFi.h>

#include "GlobalVariables.h"
#include "WebSocketHandler.h"

// Incluye los archivos HTML, CSS y JS
#include "html.h"
#include "script.h"
#include "styles.h"

#define AP_SSID "ESP32_AP"
#define AP_PASSWORD "12345678"

class WebServerHandler {
 private:
  AsyncWebServer   server;
  GlobalVariables& globals;
  DNSServer        dnsServer;
  WebSocketHandler wsHandler;

  static WebServerHandler* instance;

  // ✅ Redirección de peticiones desconocidas al portal cautivo
  static void handleRedirect(AsyncWebServerRequest* request) {
    request->redirect("http://192.168.4.1");
  }

 public:
  WebServerHandler(GlobalVariables& globals) : server(80), globals(globals), wsHandler(globals) {
    instance = this;
  }

  void begin() {
    WiFi.softAP(AP_SSID, AP_PASSWORD);
    WiFi.softAPConfig(IPAddress(192, 168, 4, 1), IPAddress(192, 168, 4, 1), IPAddress(255, 255, 255, 0));

    // ✅ Configuración del servidor DNS para Captive Portal
    dnsServer.start(53, "*", WiFi.softAPIP());

    // ✅ Servir archivos HTML, CSS y JS
    server.on(
        "/", HTTP_GET, [](AsyncWebServerRequest* request) { request->send(200, "text/html", INDEX_HTML_TEMPLATE); });
    server.on(
        "/styles.css", HTTP_GET, [](AsyncWebServerRequest* request) { request->send(200, "text/css", STYLES_CSS); });
    server.on("/script.js", HTTP_GET, [](AsyncWebServerRequest* request) {
      request->send(200, "application/javascript", SCRIPT_JS);
    });

    server.on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest* request) { request->send(204); });

    // ✅ Redirección para Captive Portal
    server.onNotFound(handleRedirect);

    // ✅ Conectar WebSocket al servidor
    wsHandler.attachToServer(server);

    server.begin();
    Serial.println("Servidor web iniciado");
  }

  void loop() {
    wsHandler.loop();                // Mantener WebSockets activos
    dnsServer.processNextRequest();  // Procesar peticiones DNS
  }

  WebSocketHandler& getWebSocketHandler() {
    return wsHandler;
  }
};

WebServerHandler* WebServerHandler::instance = nullptr;

#endif  // WEBSERVERHANDLER_H
