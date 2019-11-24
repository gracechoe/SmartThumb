/* eslint-disable max-classes-per-file */
/* eslint-disable no-restricted-globals */
/* eslint-disable no-undef */
$(document).ready(() => {
  // if deployed to a site supporting SSL, use wss://
  const protocol = document.location.protocol.startsWith('https') ? 'wss://' : 'ws://';
  const webSocket = new WebSocket(protocol + location.host);

  // A class for holding the last N points of telemetry for a device
  class DeviceData {
    constructor(deviceId) {
      this.deviceId = deviceId;
      this.maxLen = 50;
      this.temperature = 0;
      this.humidity = 0;
      this.soilMoisture = 0;
    }
  }

 
  webSocket.onmessage = function onMessage(message) {
    try {
      const messageData = JSON.parse(message.data);
      console.log(messageData);

      // time and either temperature or humidity are required
      if (!messageData.MessageDate || (!messageData.IotData.temperature && !messageData.IotData.humidity)) {
        return;
      }
  
      document.getElementById('soilText').innerText = messageData.IotData.soilMoisture;
      document.getElementById('temperatureText').innerText = messageData.IotData.temperature;
      document.getElementById('humidityText').innerText = messageData.IotData.humidity;
      this.temperature = messageData.IotData.temperature;
      this.humidity = messageData.IotData.humidity;
      this.soilMoisture = messageData.IotData.soilMoisture;


    } catch (err) {
      console.error(err);
    }
  };
});