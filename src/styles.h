#ifndef STYLES_H
#define STYLES_H

const char *STYLES_CSS = R"rawliteral(
html,
body {
  box-sizing: border-box;
  display: flex;
  margin: 0;
}

html {
  flex-direction: column;
  font-family: Arial, sans-serif;
  background-color: #333;
  color: #fff;
  justify-content: center;
  align-items: center;
  font-size: 16px;
}

body {
  display: flex;
  flex-direction: column;
  flex: 1 1 100px;
  width: 100%;
  padding: 20px;
  gap: 20px;
  max-width: 800px;
  align-items: center;
  justify-content: center;
}

button {
  flex: 1 1 80px;
  background-color: #1565c0;
  border: none;
  color: white;
  padding: 8px;
  margin: 5px;
  text-align: center;
  display: inline-block;
  font-size: 14px;
  font-weight: 700;
  border-radius: 5px;
  cursor: pointer;
  min-width: 50px;
  max-width: 200px;
}

button:hover {
  background-color: #1976d2;
}

h3 {
  margin-top: 5px;
  margin-bottom: 5px;
}

.full-box {
  width: 100%;
  font-weight: bold;
  text-align: center;
  font-size: 22pt;
}

.box {
  flex: 1 1 100px;
  background: #444;
  border-radius: 5px;
  padding: 10px;
  display: flex;
  flex-direction: column;
  gap: 10px;
  max-width: 600px;
  min-width: 320px;
}

.box-wrap {
  display: flex;
  flex-wrap: wrap;
  justify-content: space-between;
  align-items: center;
  gap: 5px;
}

.title {
  text-align: center;
  font-size: 20px;
  font-weight: 600;
  border: 1px solid #3e3c3c;
  border-width: 0px 0px 2px 0px;
  padding-bottom: 5px;
  margin-bottom: 3px;
}

.label-status {
  display: inline-block;
  min-width: 50px;
  font-weight: bold;
  text-align: end;
}

.status {
  font-weight: bold;
  border-radius: 10px;
  padding: 5px 10px;
  display: inline-block;
  min-width: 80px;
  text-align: center;
}

.offline,
.blocked,
.delayed {
  color: rgb(255, 0, 0);
  background-color: rgb(100, 20, 20);
}

.online,
.running {
  color: rgb(0, 255, 0);
  background-color: rgb(47, 100, 20);
}

.inactive, .unknown {
  color: rgb(109, 113, 109);
  background-color: rgb(55, 55, 55);
}

.visible, 
.waiting,
.standby{
  color: rgb(3, 120, 255);
  background-color: rgb(9, 8, 63);
}

.exposure,
.calibrating,
.paused {
  color: #fff308;
  background-color: #766f09;
}

/* BUTTONS */
.green-btn {
  background-color: #388e3c;
}

.green-btn:hover {
  background-color: #43a047;
}

.yellow-btn {
  background-color:#bfb30a;
}

.yellow-btn:hover {
  background-color: #fff308;
}

.red-btn {
  background-color: #d32f2f;
}

.red-btn:hover {
  background-color: #b32727;
}

.green-btn,
.red-btn, 
.yellow-btn {
  flex: 1 1 50px;
  max-height: 30px;
  padding: 0px;
  margin: 0px;
  min-height: 25px;
  max-width: 40%;
}

.inactive-btn{
  color: rgb(109, 113, 109);
  background-color: rgb(55,55,55) !important;
}

.inactive-btn:hover{
  background-color: rgb(66, 66, 66) !important;
}

.duration-button {
  background-color: #76797d;
  color: rgb(89, 86, 86);
  opacity: 0.7;
  transition: opacity 0.3s, background-color 0.3s;
}

.counter-button {
  font-weight: bold;
  min-width: 10px;
  max-width: 40px;
}

.full-button{
  font-weight: bold;
  min-width: 100%;
}
/* BUTTONS */

/* VISIBILITY */
.hidden {
  display: none;
}

.duration-button.active {
  opacity: 1;
  background-color: #43a047;
  color: white;
}
/* VISIBILITY */

/* inputs */

input[type="number"] {
  flex: 1 1 30px;
  background-color: #444;
  border: 1px solid #555;
  color: white;
  padding: 8px;
  text-align: center;
  font-size: 14px;
  font-weight: 700;
  border-radius: 5px;
  max-width: 80px;
  transition: border-color 0.3s, box-shadow 0.3s;
}

input[type="number"]:focus {
  border-color: #43a047;
  box-shadow: 0 0 8px #43a047;
  outline: none;
}
/* inputs */

)rawliteral";

#endif  // STYLES_H
