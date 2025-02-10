# Definición de variables:
1) Variables CPP dentro de la memoria del MC
2) Variables JS dentro del webserver
3) Variables PY dentro del Programa 

# Actualización de variables
## Desde Programa
Al actualizar una variable PY con la clase estado:
- Mandar la variable por serial -> set[varName] [value]
### en `main.py`
- **variable CPP**: `globals.updateVariable()`
- **variable JS**: `ws.textAll()`

## Desde Webserver
Al actualizar una variable JS:
### en `script.h` 
- **variable JS**: `updateVariable()`-> mandar el mensaje a los clientes con `socket.send()`
### en `WebserverHandler.h`
- el websocket `onWebSocketEvent` escucha el tipo `WS_EVT_DATA`
- **variable PY**: `Serial.printf()` *set[varname] [value]* 
- **variable CPP**: `globals.updateVariable()`
