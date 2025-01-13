#ifndef INDEX_HTML_H
#define INDEX_HTML_H

#include "script.h"

const char *INDEX_HTML_TEMPLATE = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
  <head>
    <meta charset="UTF-8" />
    <meta name="viewport" content="width=device-width, initial-scale=1.0" />
    <title>Robochuy</title>
    <link rel="stylesheet" type="text/css" href="/styles.css">
    <link rel="icon" href="/favicon.ico" type="image/x-icon">

  </head>
  <body>
    <div class="full-box">Robochuy</div>

    <!-- Conexiones Panel -->
    <div class="box">
      <div class="title">Conexiones</div>
      <div>
        <span class="label-status">Serial:</span>
        <span class="status offline" id="serial">Offline</span>
      </div>

      <div>
        <span class="label-status">Wifi:</span>
        <span class="status online" id="wifi">Online</span>
      </div>
    </div>

    <!-- Computer Vision Panel -->
    <div class="box">
      <div class="title">Computer Vision</div>
      <div class="row">
        <span class="label-status">AWS:</span>
        <span class="status visible" id="aws">visible</span>
      </div>
      <div class="row">
        <span class="label-status">MU:</span>
        <span class="status exposure" id="generator">exposure</span>
      </div>
      <div class="row">
        <span class="label-status">MCU:</span>
        <span class="status standby" id="fpd">standby</span>
      </div>
    </div>

    <!-- Panel Manual-->
    <div class="box" id="panel-manual">
      <div class="title">Control manual por tiempo</div>
      <div class="box-wrap">
        <span>Disparos: </span>
        <button
          id="button-short"
          class="counter-button"
          onclick="updateVariable('count', 1)"
        >
          1
        </button>
        <input
          type="number"
          min="1"
          max="44"
          id="input-count"
          class="counter"
          value="44"
        />
        <button
          id="button-largo"
          class="counter-button"
          onclick="updateVariable('count', 44)"
        >
          44
        </button>
      </div>

      <div class="box-wrap">
        <button
          id="button-short"
          class="duration-button"
          onclick="updateVariable('duration', 'short')"
        >
          Corto 5s
        </button>
        <button
          id="button-medio"
          class="duration-button"
          onclick="updateVariable('duration', 'medio')"
        >
          Medio 15s
        </button>
        <button
          id="button-largo"
          class="duration-button"
          onclick="updateVariable('duration', 'largo')"
        >
          Largo 5m
        </button>
      </div>

      <div class="box-wrap">
        <span>Retardo 1er disparo[secs]:</span>
        <input type="number" min="5" max="600" id="input-delay" value="0" />
      </div>

      <div class="box-wrap">
        <span>Pausa entre disparos [secs]:</span>
        <input type="number" min="5" max="600" id="input-pause" value="30" />
      </div>

      <div class="box-wrap">
        <span>Status handswitch:</span>
        <span class="status exposure" id="handswitch">exposure</span>
      </div>

      <div class="box-wrap">
        <button class="green-btn" onclick="updateVariable('bot', 'STARTED')">
          &#9658;
        </button>
        <button class="red-btn" onclick="updateVariable('bot', 'STOPPED')">
          &#9209;
        </button>
      </div>
    </div>
    <script src="script.js"></script>
  </body>
</html>
)rawliteral";

#endif // INDEX_HTML_H
