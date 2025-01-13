#ifndef UARTHANDLER_H
#define UARTHANDLER_H

#include <Arduino.h>
#include <driver/uart.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>

class UARTHandler
{
public:
  UARTHandler(size_t bufferSize = 1024)
      : bufferSize(bufferSize > 0 ? bufferSize : 1024), dataReady(false), rxIndex(0)
  {
    buffer = new char[this->bufferSize];
    portMUX_INITIALIZE(&mux);
  }

  ~UARTHandler()
  {
    uart_driver_delete(UART_NUM_0);
    delete[] buffer;
  }

  void begin(int baudRate = 115200, int txPin = UART_PIN_NO_CHANGE, int rxPin = UART_PIN_NO_CHANGE)
  {
    uart_config_t uartConfig = {
        .baud_rate = baudRate,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB};

    uart_param_config(UART_NUM_0, &uartConfig);
    uart_set_pin(UART_NUM_0, txPin, rxPin, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_driver_install(UART_NUM_0, bufferSize, 0, 0, NULL, 0);

    esp_err_t result = uart_enable_rx_intr(UART_NUM_0);
    if (result == ESP_OK)
    {
      Serial.println("Interrupción RX habilitada.");
    }
    else
    {
      Serial.println("Error al habilitar interrupción RX.");
    }
  }

  void handleUART()
  {
    uint8_t data;
    while (uart_read_bytes(UART_NUM_0, &data, 1, 10 / portTICK_PERIOD_MS) > 0)
    {
      if (rxIndex < bufferSize - 1)
      {
        buffer[rxIndex++] = data;
        dataReady = true;
      }
      else
      {
        Serial.println("Buffer lleno. Datos descartados.");
      }
    }
  }

  bool hasData() const
  {
    return dataReady;
  }

  String read()
  {
    String result = "";
    portENTER_CRITICAL(&mux);
    for (size_t i = 0; i < rxIndex; ++i)
    {
      result += buffer[i];
    }
    rxIndex = 0;
    dataReady = false;
    portEXIT_CRITICAL(&mux);
    return result;
  }

  void write(const String &message)
  {
    uart_write_bytes(UART_NUM_0, message.c_str(), message.length());
  }

private:
  size_t bufferSize;
  char *buffer;
  volatile size_t rxIndex;
  volatile bool dataReady;
  portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED; // Mutex para secciones críticas
};

#endif // UARTHANDLER_H
