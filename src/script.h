#ifndef SCRIPT_H
#define SCRIPT_H
const char *SCRIPT_JS = R"rawliteral(

const variables = {
  generator: "unknown",
  fpd: "unknown",
  calibration: "unknown",
  serial: "offline",
  wifi: "offline",
  bot: "standby",
  handswitch: "unknown",
  modo: "manual",
  duración: 5,
  duración_actual: 0,
  pausa: 30,
  pausa_actual: 0,
  retraso: 0,
  retraso_actual: 0,
  contador: 1,
  total: 1,
};

// FUNCIONES DE SOCKET
const socket = new WebSocket("ws://192.168.4.1/ws");

socket.onclose = () => console.log("WebSocket desconectado");

socket.onopen = () => {
  console.log("Conectado al WebSocket")
};

socket.onmessage = (event) => {
  const data = JSON.parse(event.data);
  if (!data) return;
  const {message, varName, value} = data
  const numericKeys = ["pausa", "pausa_actual","retraso","retraso_actual", "contador","duración", "duración_actual", "total"];
  if (varName in variables) {
    variables[varName] = numericKeys.includes(varName) ? parseInt(value) : value;
  } 
  updateUI();
};
// FUNCIONES DE SOCKET

function handleClick(button){
  bot = variables.bot;
  if(button === "play"){
    if(["paused", "waiting", "standby"].includes(bot)){
      updateVariable("bot", "running");
    }
  }

  else if(button === "pausa"){
    if(["running", "waiting", "delayed"].includes(bot)){
      updateVariable("bot", "paused");
    }
  }

  else if(button === "stop"){
    if(["running", "waiting", "delayed", "paused"].includes(bot)){
      updateVariable("bot", "standby");
    }
  }
}

function updateVariable(varName, value) {
  if (varName in variables){
    const payload = JSON.stringify({ varName, value });
    console.log("sending socket ", payload);
    socket.send(payload);
  }
}

document.addEventListener("DOMContentLoaded", () => {
  const inputPause = document.getElementById("input-pausa");
  const inputDelay = document.getElementById("input-delay");
  const inputCount= document.getElementById("input-contador");

  if (inputPause && inputDelay && inputCount) {
    inputPause.addEventListener("blur", handleBlur);
    inputDelay.addEventListener("blur", handleBlur);
    inputCount.addEventListener("blur", handleBlur);
  }}
);

async function handleBlur(event) {
  const { value, id, min, max} = event.target;
  const numericValue = Number(value);
  const element = document.getElementById(id);
  const varName = id.replace("input-", "");

  const minimum = parseInt(min);
  const maximum = parseInt(max);

  let newValue = numericValue;
  // Validar que el valor esté en el rango permitido
  if(numericValue < minimum) newValue = minimum;
  if(numericValue > maximum) newValue = maximum;
  updateVariable(varName, newValue.toString());
};

function updateUI() {
  // MODIFICA INPUTS SIN MODIFICAR CLASES
  const pausa = document.getElementById("input-pausa");
  const pauseValue = Math.min(Math.max(variables.pausa, 5), 600); // Asegura que esté en rango
  pausa.value = pauseValue;
  variables.pausa = pauseValue; // Asegura consistencia en caso de corrección

  const delay = document.getElementById("input-delay");
  const delayValue = Math.min(Math.max(variables.retraso, 0), 600); // Asegura que esté en rango
  delay.value = delayValue;
  variables.retraso = delayValue; // Asegura consistencia en caso de corrección

  const contador = document.getElementById("input-contador");
  const countValue = Math.min(Math.max(variables.contador, 0), 44); // Asegura que esté en rango
  contador.value = countValue;
  variables.contador = countValue; // Asegura consistencia en caso de corrección

  // MODIFICA STATUS CON CLASES
  // STATUS GEN
  const gen = document.getElementById("generator");
  gen.classList.remove(...gen.classList);
  gen.classList.add("status", variables.generator);
  gen.textContent = variables.generator;

  // STATUS FPD
  const fpd = document.getElementById("fpd");
  fpd.classList.remove(...fpd.classList);
  fpd.classList.add("status", variables.fpd);
  fpd.textContent = variables.fpd;

  // STATUS SERIAL
  const serial = document.getElementById("serial");
  serial.textContent = variables.serial;
  serial.classList.remove(...serial.classList);
  serial.classList.add("status", variables.serial);

  // STATUS WIFI
  const wifi = document.getElementById("wifi");
  wifi.textContent = variables.wifi;
  wifi.classList.remove(...wifi.classList);
  wifi.classList.add("status", variables.wifi);

  //STATUS BOT
  const bot = document.getElementById("bot");
  bot.textContent = variables.bot;
  bot.classList.remove(...bot.classList);
  bot.classList.add("status", variables.bot);

  // STATUS HANDSWITCH
  const handswitch = document.getElementById("handswitch");
  if(variables.handswitch === "waiting"){
    handswitch.textContent = variables.handswitch + " " + variables.pausa_actual;
  }
  else if(variables.handswitch === "exposure"){
    handswitch.textContent = variables.handswitch + " " + variables.duración_actual;
  }
  else if(variables.handswitch === "delayed"){
    handswitch.textContent = variables.handswitch + " " + variables.retraso_actual;
  }
  else{
    handswitch.textContent = variables.handswitch;
  }
  handswitch.classList.remove(...handswitch.classList);
  handswitch.classList.add("status", variables.handswitch);

  // remove all hidden class in buttons
  document.querySelectorAll("div.box.mode").forEach((div) => {
    div.classList.remove("hidden")
  });

  //add hidden class to manual if other is selected
  if(variables.modo !== "manual"){
    const modoDiv = document.getElementById(`panel-manual`);
    modoDiv.classList.add("hidden");
  }

  // remove all active class in buttons
  document.querySelectorAll("button[class^='duration-button']").forEach((button) => {
    button.classList.remove("active");
  });

  //add active class to duration button
  let name = ""
  if (variables.duración === 5) name = "short"
  else if (variables.duración === 15) name = "medium"
  else if (variables.duración === 330) name = "long"

  const activedurationButton = document.getElementById(`button-${name}`);
  activedurationButton.classList.add("active");
}

)rawliteral";

#endif  // SCRIPT_H
