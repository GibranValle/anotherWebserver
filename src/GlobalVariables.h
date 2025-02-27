#ifndef GLOBALVARIABLES_H
#define GLOBALVARIABLES_H

#include <map>

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

// Definiciones para duración
#define SHORT "short"
#define MEDIUM "medium"
#define LONG "long"

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
      {"calibration", UNKNOWN},
      {"calibrations", "[]"},
      {"duration", SHORT}};

  // Variables numéricas
  std::map<String, int> numVariables{{"pausa", 30}, {"retraso", 0}, {"contador", 1}, {"total", 1}};

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
    static const std::map<String, std::initializer_list<String>> validValues = {
        {"generator", {STANDBY, BLOCKED, EXPOSURE, OFFLINE, UNKNOWN}},            // 5
        {"fpd", {STANDBY, EXPOSURE, OFFLINE, UNKNOWN, CALIBRATING, CALIBRATED}},  // 6
        {"bot", {RUNNING, STANDBY, WAITING, PAUSED, DELAYED, OFFLINE, UNKNOWN}},  // 7
        {"handswitch", {EXPOSURE, STANDBY, OFFLINE, UNKNOWN, WAITING}},           // 5
        {"modo", {MANUAL, SEMI, AUTO}},                                           // 3
        {"duration", {SHORT, MEDIUM, LONG}},                                      // 3
        {"serial", {ONLINE, OFFLINE, ERROR}}                                      // 3
        // Se ha removido "wifi" para que acepte cualquier string
    };

    auto it = validValues.find(name);
    if (it != validValues.end()) {
      // La variable aparece en validValues; comprobar si el valor está en la lista
      return std::find(it->second.begin(), it->second.end(), value) != it->second.end();
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
        Serial.println("Error: valor inválido para '" + name + "'");
        return;
      }
    } else if (numVariables.count(name)) {
      // Para variables de tipo numérico
      int intValue = value.toInt();
      if ((name == "pausa" && intValue >= 5 && intValue <= 600) ||
          (name == "retraso" && intValue >= 0 && intValue <= 600) ||
          (name == "contador" && intValue >= 0 && intValue <= 44)) {
        numVariables[name] = intValue;
      } else {
        Serial.println("Error: valor inválido para '" + name + "'");
        return;
      }
    } else {
      // Variable no reconocida
      Serial.println("Error: variable inválida '" + name + "'");
      return;
    }
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
