#ifndef SCRIPT_H
#define SCRIPT_H
const char *SCRIPT_JS = R"rawliteral(

const variables = {
  aws: "unknown",
  generator: "unknown",
  fpd: "unknown",
  serial: "offline",
  wifi: "offline",
  bot: "STOPPED",
  handswitch: "standby",
  duration: "short",
  pause: 30,
  delay: 0,
  count: 1
};

const socket = new WebSocket("ws://192.168.4.1/ws");

socket.onopen = () => console.log("Conectado al WebSocket");

socket.onmessage = (event) => {
  const data = JSON.parse(event.data);
  // Actualizar elementos DOM con los datos recibidos
  document.getElementById("serial").innerText = data.serial || "Offline";
  document.getElementById("wifi").innerText = data.wifi || "Offline";
  document.getElementById("aws").innerText = data.aws || "invisible";
  document.getElementById("generator").innerText = data.generator || "standby";
  document.getElementById("fpd").innerText = data.fpd || "standby";
};

socket.onclose = () => console.log("WebSocket desconectado");

function updateVariable(varName, value) {
  const payload = JSON.stringify({ varName, value });
  socket.send(payload);
}

document.addEventListener("DOMContentLoaded", () => {
  const inputPause = document.getElementById("input-pause");
  const inputDelay = document.getElementById("input-delay");
  const inputCount= document.getElementById("input-count");

  if (inputPause && inputDelay && inputCount) {
    inputPause.addEventListener("blur", handleBlurPause);
    inputDelay.addEventListener("blur", handleBlurDelay);
    inputCount.addEventListener("blur", handleBlurCount);
  }}
);

async function handleBlurPause(event) {
  const { value, id} = event.target;
  console.log(id);
  const numericValue = Number(value);
  const inputPauseElem = document.getElementById("input-pause");

  // Validar que el valor esté en el rango permitido
  if (numericValue < 5 || numericValue > 600) {
    let newValue = 0;
    if(numericValue < 5) newValue = 5;
    if(numericValue > 600) newValue = 600;
    inputPauseElem.value = newValue;
    return;
  }

  try {
    const response = await fetch(`/update?var=pause&value=${numericValue}`);
    if (response.ok) {
      variables.pause = numericValue;
      updateUI();
    } 
  } catch (error) {
    //TODO FETCH UPDATE...
  }
}

async function handleBlurDelay(event) {
  const { value } = event.target;
  const numericValue = Number(value);
  const inputDelayElem = document.getElementById("input-delay");

  // Validar que el valor esté en el rango permitido
  if (numericValue < 5 || numericValue > 600) {
    let newValue = 0;
    if(numericValue < 5) newValue = 5;
    if(numericValue > 600) newValue = 600;
    inputDelayElem.value = newValue;
    return;
  }

  try {
    const response = await fetch(`/update?var=delay&value=${numericValue}`);
    if (response.ok) {
      variables.delay = numericValue;
      updateUI();
    } 
  } catch (error) {
    //TODO FETCH UPDATE...
  }
}

async function handleBlurCount(event) {
  const { value } = event.target;
  const numericValue = Number(value);
  const inputCountElem = document.getElementById("input-count");

  // Validar que el valor esté en el rango permitido
  if (numericValue < 45 || numericValue > 0) {
    let newValue = 0;
    if(numericValue < 1) newValue = 1;
    if(numericValue > 44) newValue = 44;
    inputCountElem.value = newValue;
    return;
  }

  try {
    const response = await fetch(`/update?var=count&value=${numericValue}`);
    if (response.ok) {
      variables.count = numericValue;
      updateUI();
    } 
  } catch (error) {
    //TODO FETCH UPDATE...
  }
}


function updateUI() {
  const aws = document.getElementById("aws");
  const gen = document.getElementById("generator");
  const fpd = document.getElementById("fpd");
  const serial = document.getElementById("serial");
  const wifi = document.getElementById("wifi");
  const bot = document.getElementById("bot");
  const handswitch = document.getElementById("handswitch");

  const pause = document.getElementById("input-pause");
  const delay = document.getElementById("input-delay");
  const count = document.getElementById("input-count");

  aws.textContent = variables.aws;
  gen.textContent = variables.generator;
  fpd.textContent = variables.fpd;
  serial.textContent = variables.serial;
  wifi.textContent = variables.wifi;
  bot.textContent = variables.bot;
  handswitch.textContent = variables.handswitch;

  const pauseValue = Math.min(Math.max(variables.pause, 5), 600); // Asegura que esté en rango
  pause.value = pauseValue;
  variables.pause = pauseValue; // Asegura consistencia en caso de corrección

  const delayValue = Math.min(Math.max(variables.delay, 0), 600); // Asegura que esté en rango
  delay.value = delayValue;
  variables.delay = delayValue; // Asegura consistencia en caso de corrección

  const countValue = Math.min(Math.max(variables.delay, 0), 44); // Asegura que esté en rango
  count.value = countValue;
  variables.count = countValue; // Asegura consistencia en caso de corrección

  // remove all active class in buttons
  document.querySelectorAll("button[class^='duration-button']").forEach((button) => button.classList.remove("active"));

  //add active class to duration button
  const activedurationButton = document.getElementById(`button-${variables.duration}`);
  activedurationButton.classList.add("active");
}

)rawliteral";

#endif // SCRIPT_H
