#include "esp32-hal.h"
#ifndef DISPLAYHANDLER_H
#define DISPLAYHANDLER_H

#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include "image.h"
#include "qr.h"

// Pines para el display
#define TFT_CS 15
#define TFT_DC 2
#define TFT_RST 4
#define HEIGHT 135
#define WIDTH 240

class DisplayHandler
{
private:
    Adafruit_ST7789 tft; // Objeto del display

public:
    // Constructor
    DisplayHandler()
        : tft(TFT_CS, TFT_DC, TFT_RST) {}

    // Método para inicializar el display
    void begin()
    {
        tft.init(HEIGHT, WIDTH);        // Resolución del display de 1.14"
        tft.setRotation(1);             // Configurar orientación
        tft.fillScreen(ST77XX_BLACK);   // Fondo negro
        tft.setTextSize(2);             // Tamaño del texto
        tft.setTextColor(ST77XX_WHITE); // Color del texto
        tft.setCursor(10, 10);          // Posición inicial del texto
        tft.println("Display inicializado.");
    }

    // Función para dibujar la imagen desde un array
    void drawImage(const uint16_t *image = image_data)
    {
        tft.fillScreen(ST77XX_BLACK); // Limpiar pantalla
        int16_t pixelIndex = 0;       // Índice del array
        for (int16_t row = 0; row < IMAGE_HEIGHT; row++)
        {
            for (int16_t col = 0; col < IMAGE_WIDTH; col++)
            {
                uint16_t color = image[pixelIndex++];
                // centered image
                tft.drawPixel(col + (WIDTH / 2 - IMAGE_WIDTH / 2), row + (HEIGHT / 2 - IMAGE_HEIGHT / 2), color);
            }
        }
    }

    void drawQR(const uint16_t *image = QR)
    {
        tft.fillScreen(ST77XX_BLACK); // Limpiar pantalla
        int16_t pixelIndex = 0;       // Índice del array
        for (int16_t row = 0; row < QR_HEIGHT; row++)
        {
            for (int16_t col = 0; col < QR_WIDTH; col++)
            {
                uint16_t color = image[pixelIndex++];
                // centered image
                tft.drawPixel(col + (WIDTH / 2 - QR_WIDTH / 2), row + (HEIGHT / 2 - QR_HEIGHT / 2), color);
            }
        }
    }

    void drawExposure()
    {
        tft.fillScreen(ST77XX_YELLOW); // Limpiar pantalla
        delay(400);
        drawImage();
    }

    // Método para mostrar texto en el display
    void showText(const String &text)
    {
        tft.fillScreen(ST77XX_BLACK); // Limpiar pantalla
        tft.setTextColor(ST77XX_BLUE);
        tft.setCursor(10, 10); // Reiniciar cursor
        tft.println(text);
    }

    void showArray(const char *elements[], size_t elementCount, const char *color = "info")
    {
        int lineHeight = 16;                                 // Altura de cada línea en píxeles
        int startY = 10;                                     // Desplazamiento vertical inicial
        int startX = 10;                                     // Desplazamiento horizontal
        int maxLines = (tft.height() - startY) / lineHeight; // Calcular el máximo de líneas visibles
        int lastLine = elementCount;                         // Última linea
        int yOffset = startY;
        tft.fillScreen(ST77XX_BLACK); // Limpiar pantalla

        if (color == "info")
        {
            tft.setTextColor(ST77XX_WHITE);
        }
        else if (color == "success")
        {
            tft.setTextColor(ST77XX_GREEN);
        }
        else if (color == "errir")
        {
            tft.setTextColor(ST77XX_RED);
        }

        for (size_t currentLine = 0; currentLine <= maxLines; ++currentLine)
        {
            if (currentLine > lastLine + 1)
                break;
            tft.setCursor(startX, yOffset);   // Configurar posición para cada elemento
            tft.print(elements[currentLine]); // Imprimir el elemento
            yOffset += lineHeight;            // Moverse 16 píxeles hacia abajo
        }
    }
};

#endif // DISPLAYHANDLER_H
