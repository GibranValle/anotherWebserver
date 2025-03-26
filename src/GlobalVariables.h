#ifndef GLOBALVARIABLES_H
#define GLOBALVARIABLES_H

#include <map>
#include <set>

// Definiciones para valores de estado
#define STANDBY "standby"
#define BLOCKED "blocked"
#define EXPOSURE "exposure"
#define OFFLINE "offline"
#define ONLINE "online"
#define ERROR "error"
#define UNKNOWN "unknown"
#define CALIBRATING "calibrating"
#define CALIBRATED "calibrated"
#define RUNNING "running"
#define WAITING "waiting"
#define PAUSED "paused"
#define DELAYED "delayed"
#define MANUAL "manual"
#define SEMI "semi"
#define AUTO "auto"
#define REMOTO "remoto"

// Definiciones para duración
#define SHORT "5"
#define MEDIUM "15"
#define LONG "330"

/*
Descripción de status del bot:
RUNNING -> botón play pulsado 
PAUSED -> botón paused pulsado
STANDBY -> botón detener pulsado

Descripción de status del HS
STANDBY -> sin acciones ejecutando
EXPOSURE -> RELAY Y LED ACTIVO
OFFLINE -> sin conexión serial
UNKNOWN -> estado inicial hasta que tenga conexión
WAITING -> en espera del proximo disparo
DELAYED -> en espera del temporizador de retraso
PAUSED -> bot pausado por el usuario
*/

class GlobalVariables {
 private:
  // Variables de conexión y estado
  std::map<String, String> variables{
      {"serial", OFFLINE},
      {"wifi", OFFLINE},  // wifi se inicializa en OFFLINE pero luego puede recibir cualquier valor
      {"generator", UNKNOWN},
      {"fpd", UNKNOWN},
      {"handswitch", UNKNOWN},
      {"bot", STANDBY},
      {"modo", MANUAL},
      {"calibration", UNKNOWN}};

  // Variables numéricas
  std::map<String, int> numVariables{{"duration", 5},
                                     {"duration_actual", 0},
                                     {"pausa", 15},
                                     {"pausa_actual", 0},
                                     {"retraso", 0},
                                     {"retraso_actual", 0},
                                     {"contador", 0},
                                     {"total", 1},
                                     {"calibrations", 0}};

 public:
  GlobalVariables() {}

  /**
   * @brief Verifica si el valor proporcionado es válido para la variable dada.
   *
   * Si la variable aparece en el mapa 'validValues',
   * se comprueba que el valor esté dentro de la lista de valores permitidos.
   * Si no aparece, no se aplican restricciones.
   */
  bool isValidValue(const String &name, const String &value) const {
    static const std::map<String, std::set<String>> validValues = {
        {"generator", {STANDBY, BLOCKED, EXPOSURE, OFFLINE, UNKNOWN}},            // 5
        {"fpd", {STANDBY, EXPOSURE, OFFLINE, UNKNOWN, CALIBRATING, CALIBRATED}},  // 6
        {"bot", {RUNNING, STANDBY, PAUSED}},                                      // 3
        {"handswitch",
         {
             EXPOSURE,
             STANDBY,
             OFFLINE,
             UNKNOWN,
             WAITING,
             PAUSED,
             DELAYED,
         }},                                     // 7
        {"modo", {MANUAL, SEMI, AUTO, REMOTO}},  // 3
        {"serial", {ONLINE, OFFLINE, ERROR}}     // 3
                                                 // Se ha removido "wifi" para que acepte cualquier string
    };
    auto it = validValues.find(name);
    if (it != validValues.end()) {
      // La variable aparece en validValues; comprobar si el valor está en la lista
      return it->second.find(value) != it->second.end();
    }
    // Si la variable no está en validValues, no tiene restricciones
    return true;
  }

  /**
   * @brief Actualiza la variable con el nuevo valor, si es válido.
   *
   * @param name Nombre de la variable a actualizar.
   * @param value Nuevo valor para la variable.
   */
  void updateVariable(const String &name, const String &value) {
    if (variables.count(name)) {
      // Para variables de tipo String
      if (isValidValue(name, value)) {
        variables[name] = value;
      } else {
        Serial.println("Error: " + name + value);
        Serial.println("Error: valor inválido para '" + name + "'");
        return;
      }
    } else if (numVariables.count(name)) {
      // Para variables de tipo numérico
      int intValue = value.toInt();
      if ((name == "pausa" && intValue >= 5 && intValue <= 600) ||
          (name == "pausa_actual" && intValue >= 0 && intValue <= 600) ||
          (name == "duration" && intValue >= 0 && intValue <= 330) ||
          (name == "duration_actual" && intValue >= 0 && intValue <= 330) ||
          (name == "retraso" && intValue >= 0 && intValue <= 600) ||
          (name == "retraso_actual" && intValue >= 0 && intValue <= 600) ||
          (name == "calibrations" && intValue >= 0 && intValue <= 20) ||
          (name == "total" && intValue >= 0 && intValue <= 44) ||
          (name == "contador" && intValue >= 0 && intValue <= 44)) {
        numVariables[name] = intValue;
      } else {
        Serial.println("Error: valor invalido para '" + name + "'");
        return;
      }
    } else {
      // Variable no reconocida
      Serial.println("Error: variable invalida para '" + name + "'");
      return;
    }
  }

  void sendVariable(const String &name, const String &value) {
    if (!variables.count(name) && !numVariables.count(name)) return;
    Serial.println("set" + name + ' ' + value);
  }

  std::map<String, String> getAllVariables() {
    std::map<String, String> allVars;

    // Agregar variables de estado (String)
    for (const auto &pair : variables) {
      allVars[pair.first] = pair.second;
    }

    // Agregar variables numéricas (convertidas a String)
    for (const auto &pair : numVariables) {
      allVars[pair.first] = String(pair.second);
    }

    return allVars;
  }

  /**
   * @brief Devuelve el valor de la variable solicitada como String.
   *
   * @param name Nombre de la variable que se desea consultar.
   * @return El valor de la variable en formato String o cadena vacía si no existe.
   */
  String getVariable(const String &name) const {
    if (variables.count(name)) {
      return variables.at(name);
    }
    if (numVariables.count(name)) {
      return String(numVariables.at(name));
    }
    return "";
  }
};

#endif  // GLOBALVARIABLES_H
