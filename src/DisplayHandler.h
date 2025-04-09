#include "esp32-hal.h"
#ifndef DISPLAYHANDLER_H
#define DISPLAYHANDLER_H
#include <iostream>
#include <string>

#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include "image.h"
#include "qr.h"
#include "GlobalVariables.h"

// Pines para el display
#define TFT_CS 15
#define TFT_DC 2
#define TFT_RST 4
#define HEIGHT 135
#define WIDTH 240
#define HALF_X int(WIDTH / 2)
#define HALF_Y int(HEIGHT / 2)

class DisplayHandler {
 private:
  Adafruit_ST7789  tft;  // Objeto del display
  GlobalVariables &globals;

 public:
  // Constructor
  DisplayHandler(GlobalVariables &globals) : tft(TFT_CS, TFT_DC, TFT_RST), globals(globals) {}

  // Método para inicializar el display
  void begin() {
    tft.init(HEIGHT, WIDTH);         // Resolución del display de 1.14"
    tft.setRotation(3);              // Configurar orientación
    tft.fillScreen(ST77XX_BLACK);    // Fondo negro
    tft.setTextSize(2);              // Tamaño del texto
    tft.setTextColor(ST77XX_WHITE);  // Color del texto
    tft.setCursor(10, 10);           // Posición inicial del texto
    tft.println("Display inicializado.");
  }

  // Función para dibujar la imagen desde un array
  void drawImage(const uint16_t *image = image_data) {
    tft.fillScreen(ST77XX_BLACK);  // Limpiar pantalla
    int16_t pixelIndex = 0;        // Índice del array
    for (int16_t row = 0; row < IMAGE_HEIGHT; row++) {
      for (int16_t col = 0; col < IMAGE_WIDTH; col++) {
        uint16_t color = image[pixelIndex++];
        // centered image
        tft.drawPixel(col + (WIDTH / 2 - IMAGE_WIDTH / 2), row + (HEIGHT / 2 - IMAGE_HEIGHT / 2), color);
      }
    }
  }

  void drawQR(const uint16_t *image = QR) {
    tft.fillScreen(ST77XX_BLACK);  // Limpiar pantalla
    int16_t pixelIndex = 0;        // Índice del array
    for (int16_t row = 0; row < QR_HEIGHT; row++) {
      for (int16_t col = 0; col < QR_WIDTH; col++) {
        uint16_t color = image[pixelIndex++];
        // centered image
        tft.drawPixel(col + (WIDTH / 2 - QR_WIDTH / 2), row + (HEIGHT / 2 - QR_HEIGHT / 2), color);
      }
    }
  }

  void showExposure(String state, int total, int contador, int duration, int duration_actual) {
    tft.fillScreen(ST77XX_BLACK);  // Limpiar pantalla
    tft.setTextSize(3);            // Tamaño del texto
    tft.setCursor(10, 5);          // Reiniciar cursor
    if (state == "EXPOSURE")
      tft.setTextColor(ST77XX_YELLOW);
    else if (state == "DELAY")
      tft.setTextColor(ST77XX_ORANGE);
    else if (state == "WAITING")
      tft.setTextColor(ST77XX_BLUE);

    tft.println(state);
    tft.setTextColor(ST77XX_YELLOW);
    tft.println(" TIME: " + String(duration_actual) + " / " + String(duration));
    tft.setTextColor(ST77XX_WHITE);
    tft.println(" SHOT: " + String(contador) + " / " + String(total));
  }

  void drawExposure() {
    tft.fillScreen(ST77XX_YELLOW);  // Limpiar pantalla
    delay(400);
    drawImage();
  }

  // Método para mostrar texto en el display
  void showText(const String &text) {
    tft.fillScreen(ST77XX_BLACK);  // Limpiar pantalla
    tft.setTextColor(ST77XX_BLUE);
    tft.setCursor(10, 10);  // Reiniciar cursor
    tft.println(text);
  }

  void showMainFrame() {
    tft.fillScreen(ST77XX_BLACK);  // Limpiar pantalla

    std::map<String, String> allVars      = globals.getAllVariables();
    String                   serial       = allVars["serial"];
    String                   wifi         = allVars["wifi"];
    String                   generator    = allVars["generator"];
    String                   fpd          = allVars["fpd"];
    String                   handswitch   = allVars["handswitch"];
    String                   bot          = allVars["bot"];
    String                   calibration  = allVars["calibration"];
    int                      calibrations = allVars["calibrations"].toInt();
    String                   duration     = allVars["duration"];
    String                   str_pausa    = allVars["pausa"];
    String                   str_retraso  = allVars["retraso"];
    String                   str_contador = allVars["contador"];
    String                   str_total    = allVars["total"];
    // Convertirlas a int usando toInt()
    int pausa    = str_pausa.toInt();
    int retraso  = str_retraso.toInt();
    int contador = str_contador.toInt();
    int total    = str_total.toInt();

    tft.setTextSize(2);  // Tamaño del texto
    int variableColor = ST77XX_WHITE;
    // 1) Linea 1 WIFI
    tft.setCursor(0, 10);
    tft.setTextColor(ST77XX_WHITE);
    tft.print(" Wifi:       ");
    if (wifi == ONLINE)
      variableColor = ST77XX_GREEN;
    else if (wifi == OFFLINE)
      variableColor = ST77XX_RED;
    tft.setTextColor(variableColor);
    tft.println(wifi);
    // Linea 2 Serial
    tft.setTextColor(ST77XX_WHITE);
    tft.print(" Serial:     ");
    if (serial == ONLINE)
      variableColor = ST77XX_GREEN;
    else if (serial == OFFLINE)
      variableColor = ST77XX_RED;
    tft.setTextColor(variableColor);
    tft.println(serial);
    // Linea 3 Agent
    tft.setTextColor(ST77XX_WHITE);
    tft.print(" Agent:     ");
    if (bot == WAITING || bot == PAUSED || bot == DELAYED)
      variableColor = ST77XX_YELLOW;
    else if (bot == STANDBY)
      variableColor = ST77XX_CYAN;
    else if (bot == RUNNING)
      variableColor = ST77XX_GREEN;
    else if (bot == WAITING)
      variableColor = ST77XX_ORANGE;
    else if (bot == UNKNOWN)
      variableColor = 0x3333;
    else if (bot == OFFLINE)
      variableColor = ST77XX_RED;
    tft.setTextColor(variableColor);
    tft.println(bot);
    // Linea 4 handswitch
    tft.setTextColor(ST77XX_WHITE);
    tft.print(" Handswitch:");
    if (handswitch == EXPOSURE)
      variableColor = ST77XX_YELLOW;
    else if (handswitch == STANDBY)
      variableColor = ST77XX_GREEN;
    else if (handswitch == WAITING)
      variableColor = ST77XX_ORANGE;
    else if (handswitch == UNKNOWN)
      variableColor = 0x3333;
    else if (handswitch == OFFLINE)
      variableColor = ST77XX_RED;
    tft.setTextColor(variableColor);
    tft.println(handswitch);
    // Linea 5 Generator
    tft.setTextColor(ST77XX_WHITE);
    tft.print(" Generator: ");
    if (generator == EXPOSURE)
      variableColor = ST77XX_YELLOW;
    else if (generator == STANDBY)
      variableColor = ST77XX_GREEN;
    else if (generator == UNKNOWN)
      variableColor = 0x3333;
    else if (generator == BLOCKED || generator == OFFLINE)
      variableColor = ST77XX_RED;
    tft.setTextColor(variableColor);
    tft.println(generator);
    // Linea 6 FPD
    tft.setTextColor(ST77XX_WHITE);
    tft.print(" FPD:    ");
    if (fpd == EXPOSURE)
      variableColor = ST77XX_YELLOW;
    else if (fpd == CALIBRATED)
      variableColor = ST77XX_GREEN;
    else if (fpd == CALIBRATING || fpd == STANDBY)
      variableColor = ST77XX_CYAN;
    else if (fpd == UNKNOWN)
      variableColor = 0x3333;
    else if (fpd == OFFLINE)
      variableColor = ST77XX_RED;
    tft.setTextColor(variableColor);
    tft.println(fpd);
    // Linea 7 Calibration
    tft.setTextColor(ST77XX_WHITE);
    tft.print(" Calib:    ");
    tft.setTextColor(ST77XX_CYAN);
    tft.println(calibration);
    // Linea 8 Contador
    tft.printf("  %d / %d", contador, total);
    ;
  }

  void showAPInfo() {
    tft.fillScreen(ST77XX_BLACK);  // Limpiar pantalla
    tft.setTextSize(2);            // Tamaño del texto
    tft.setRotation(1);            // Configurar orientación
    tft.setCursor(0, 10);
    tft.setTextColor(ST77XX_GREEN);
    tft.println(" Conectarse a:");
    tft.setTextColor(ST77XX_WHITE);
    tft.println(" SSD:    ESP32_AP");
    tft.println(" PW:     12345678");

    tft.setTextColor(ST77XX_GREEN);
    tft.println(" Ingresar a:");
    tft.setTextColor(ST77XX_WHITE);
    tft.println(" IP:  192.168.4.1");
    tft.setTextColor(ST77XX_CYAN);
    tft.println(" Desde el navegador!");
  }

  void showArray(const char *elements[], size_t elementCount, const char *color = "info") {
    int lineHeight = 16;                                    // Altura de cada línea en píxeles
    int startY     = 10;                                    // Desplazamiento vertical inicial
    int startX     = 10;                                    // Desplazamiento horizontal
    int maxLines   = (tft.height() - startY) / lineHeight;  // Calcular el máximo de líneas visibles
    int lastLine   = elementCount;                          // Última linea
    int yOffset    = startY;
    tft.fillScreen(ST77XX_BLACK);  // Limpiar pantalla

    if (color == "info") {
      tft.setTextColor(ST77XX_WHITE);
    } else if (color == "success") {
      tft.setTextColor(ST77XX_GREEN);
    } else if (color == "errir") {
      tft.setTextColor(ST77XX_RED);
    }

    for (size_t currentLine = 0; currentLine <= maxLines; ++currentLine) {
      if (currentLine > lastLine + 1) break;
      tft.setCursor(startX, yOffset);    // Configurar posición para cada elemento
      tft.print(elements[currentLine]);  // Imprimir el elemento
      yOffset += lineHeight;             // Moverse 16 píxeles hacia abajo
    }
  }
};

#endif  // DISPLAYHANDLER_H
