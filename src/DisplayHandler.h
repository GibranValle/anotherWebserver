#include "esp32-hal.h"
#ifndef DISPLAYHANDLER_H
#define DISPLAYHANDLER_H

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
#define PADDING 10
#define HALF_X int(WIDTH / 2)
#define HALF_Y int(HEIGHT / 2)

class DisplayHandler {
 private:
  Adafruit_ST7789 tft;  // Objeto del display

 public:
  // Constructor
  DisplayHandler() : tft(TFT_CS, TFT_DC, TFT_RST) {}

  // Método para inicializar el display
  void begin() {
    tft.init(HEIGHT, WIDTH);         // Resolución del display de 1.14"
    tft.setRotation(1);              // Configurar orientación
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

  void mainFrame(String &wifi,
                 String &serial,
                 String &bot,
                 String &handswitch,
                 String &generator,
                 String &fpd,
                 String &calibration,
                 short   exposure,
                 short   total) {
    tft.fillScreen(ST77XX_BLACK);  // Limpiar pantalla
    tft.setTextSize(3);            // Tamaño del texto
    tft.setRotation(1);            // Configurar orientación
    int variableColor = ST77XX_WHITE;
    // 1) cuadrante TL
    tft.setCursor(PADDING, PADDING);
    tft.setTextColor(ST77XX_WHITE);
    tft.print("Wifi: ");
    tft.setTextColor(ST77XX_GREEN);
    tft.println(wifi);
    // serial
    tft.print("Serial: ");
    if (serial == ONLINE)
      variableColor = ST77XX_GREEN;
    else if (serial == OFFLINE)
      variableColor = ST77XX_RED;
    tft.setTextColor(variableColor);
    tft.println(serial);
    tft.print("Agent: ");
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
    // 2) cuadrante TR
    tft.setCursor(HALF_X + PADDING, PADDING);
    tft.setTextColor(ST77XX_WHITE);
    tft.println("Handswitch");
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
    // 3) cuadrante BL
    tft.setCursor(PADDING, HALF_Y + PADDING);
    if (generator == EXPOSURE)
      variableColor = ST77XX_YELLOW;
    else if (generator == STANDBY)
      variableColor = ST77XX_GREEN;
    else if (generator == UNKNOWN)
      variableColor = 0x3333;
    else if (generator == BLOCKED || generator == OFFLINE)
      variableColor = ST77XX_RED;
    tft.setTextColor(variableColor);
    tft.println("Generator");
    tft.setTextColor(ST77XX_YELLOW);
    tft.println(generator);
    // 4) cuadrante BR
    tft.setCursor(HALF_X + PADDING, HALF_Y + PADDING);
    tft.setTextColor(ST77XX_WHITE);
    tft.println("FPD");
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
    tft.println(calibration);
    tft.printf("%d / %d", exposure, total);
    ;
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
