#ifndef ESP32TIMERMANAGER_H
#define ESP32TIMERMANAGER_H

#include <Arduino.h>
#include <ESP32TimerInterrupt.h>

class ESP32TimerManager
{
private:
  ESP32Timer timer;    // Instancia del temporizador
  bool timerActive;    // Estado del temporizador (activo/inactivo)
  uint32_t intervalUs; // Intervalo del temporizador en microsegundos

  // Función ISR asignada al temporizador
  static bool IRAM_ATTR defaultISR(void *arg)
  {
    Serial.println("Temporizador activado (ISR predeterminado)");
    return true;
  }

public:
  // Constructor
  ESP32TimerManager(uint8_t timerNumber)
      : timer(timerNumber), timerActive(false), intervalUs(0) {}

  // Configura el temporizador
  bool begin(uint32_t intervalMs, bool (*isrCallback)(void *) = defaultISR)
  {
    intervalUs = intervalMs * 1000; // Convertir a microsegundos

    if (timer.attachInterruptInterval(intervalUs, isrCallback))
    {
      Serial.print(F("Temporizador iniciado en intervalo: "));
      Serial.print(intervalMs);
      Serial.println(F(" ms"));
      timerActive = true;
      return true;
    }

    Serial.println(F("Error al configurar el temporizador."));
    return false;
  }

  // Detiene el temporizador
  void stop()
  {
    if (timerActive)
    {
      timer.stopTimer();
      timerActive = false;
      Serial.println(F("Temporizador detenido."));
    }
  }

  // Reinicia el temporizador
  void restart()
  {
    if (!timerActive)
    {
      timer.restartTimer();
      timerActive = true;
      Serial.println(F("Temporizador reiniciado."));
    }
  }

  // Cambia el intervalo del temporizador
  void setInterval(uint32_t intervalMs)
  {
    intervalUs = intervalMs * 1000;
    if (timerActive)
    {
      timer.stopTimer();
      timer.attachInterruptInterval(intervalUs, defaultISR);
      timer.restartTimer();
    }
    Serial.print(F("Intervalo actualizado a: "));
    Serial.print(intervalMs);
    Serial.println(F(" ms"));
  }

  // Destructor
  ~ESP32TimerManager()
  {
    stop();
  }
};

#endif // ESP32TIMERMANAGER_H
