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
  duration: 5,
  duration_actual: 0,
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
  const {message, varName, value} = data;
  if(message)  console.log("message: ", message);
  const numericKeys = ["pausa", "pausa_actual","retraso","retraso_actual", "contador","duration", "duration_actual", "total"];
  if (varName in variables) {
    variables[varName] = numericKeys.includes(varName) ? parseInt(value) : value;
    console.log(varName, value);
  } 
  updateUI();
};
// FUNCIONES DE SOCKET

function handleClick(button){
  if(["running", "paused", "standby"].includes(button)) updateBotState(button);
  if(button === "remoto") updateVariable("modo", "remoto")
}

function updateBotState(newState){
  bot = variables.bot;
  console.log(bot, newState);
  // running
  if (bot === "standby" && newState === "running") updateVariable("bot", "running")
  else if (bot === "paused" && newState === "running") updateVariable("bot", "running")
  // paused
  else if (bot === "running" && newState === "paused") updateVariable("bot", "paused")
  // stop
  else if (bot === "running" && newState === "standby") updateVariable("bot", "standby")
  else if (bot === "paused" && newState === "standby") updateVariable("bot", "standby")
}

function updateVariable(varName, value) {
  if (varName in variables){
    const payload = JSON.stringify({ varName, value });
    socket.send(payload);
    console.log("sending json", varName, value);
  }
}

document.addEventListener("DOMContentLoaded", () => {
  const inputPause = document.getElementById("input-pausa");
  const inputDelay = document.getElementById("input-delay");
  const inputTotal= document.getElementById("input-total");

  if (inputPause && inputDelay && inputTotal) {
    inputPause.addEventListener("blur", handleBlur);
    inputDelay.addEventListener("blur", handleBlur);
    inputTotal.addEventListener("blur", handleBlur);
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

  const total = document.getElementById("input-total");
  const totalValue = Math.min(Math.max(variables.total, 0), 44); // Asegura que esté en rango
  total.value = totalValue;
  variables.total = totalValue; // Asegura consistencia en caso de corrección

  const contador = document.getElementById("contador");
  contador.textContent = `${variables.contador} de ${variables.total}`
  
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
  const botLabel = document.getElementById("bot");
  botLabel.textContent = variables.bot;
  botLabel.classList.remove(...botLabel.classList);
  botLabel.classList.add("status", variables.bot);

  // STATUS HANDSWITCH
  const handswitch = document.getElementById("handswitch");
  if(variables.handswitch === "waiting"){
    handswitch.textContent = variables.handswitch + " " + variables.pausa_actual;
  }
  else if(variables.handswitch === "exposure"){
    handswitch.textContent = variables.handswitch + " " + variables.duration_actual;
  }
  else if(variables.handswitch === "delayed"){
    handswitch.textContent = variables.handswitch + " " + variables.retraso_actual;
  }
  else{
    handswitch.textContent = variables.handswitch;
  }
  handswitch.classList.remove(...handswitch.classList);
  handswitch.classList.add("status", variables.handswitch);

  // MODO
  const modoLabel = document.getElementById("modo");
  const modo = variables.modo;
  modoLabel.textContent = modo;

  // HIDE ALL PANELS
  document.querySelectorAll("div.box.panel").forEach((div) => {
    div.classList.add("hidden")
  });
  
  const panelManual = document.getElementById(`panel-manual`);
  const panelVision = document.getElementById(`panel-vision`);
  const remoteButton = document.getElementById(`button-remoto`);

  // SHOW CORRECT PANEL
  if(modo === "remoto"){
    panelManual.classList.remove("hidden");
    panelVision.classList.add("hidden");
    remoteButton.classList.add("hidden");
  }
  else{
    panelManual.classList.add("hidden");
    panelVision.classList.add("hidden");
    remoteButton.classList.remove("hidden");
  }

  // remove all active class in buttons
  document.querySelectorAll("button[class^='duration-button']").forEach((button) => {
    button.classList.remove("active");
  });

  //add active class to duration button
  let name = ""
  if (variables.duration === 5) name = "short"
  else if (variables.duration === 15) name = "medium"
  else if (variables.duration === 330) name = "long"

  const activedurationButton = document.getElementById(`button-${name}`);
  activedurationButton.classList.add("active");

  // remove all active class in buttons for actions
  document.querySelectorAll("button[class^='action-button']").forEach((button) => {
    button.classList.add("inactive-btn");
  });

  // if serial connection is closed, and modo is not remote return
  if(modo !== "remoto" && variables.serial === "offline") return;

  const button_running = document.getElementById("button-running");
  const button_paused = document.getElementById("button-paused");
  const button_standby = document.getElementById("button-standby");

  const bot = variables.bot;
  if(bot === "standby") {
    button_running.classList.remove("inactive-btn");
    button_paused.classList.remove("inactive-btn");
    button_standby.classList.remove("inactive-btn");
  }
  else if(bot === "paused"){
    button_running.classList.remove("inactive-btn");
    button_standby.classList.remove("inactive-btn");
  }
  else if(bot === "running"){
    button_paused.classList.remove("inactive-btn");
    button_standby.classList.remove("inactive-btn");
  }
}

)rawliteral";

#endif  // SCRIPT_H
