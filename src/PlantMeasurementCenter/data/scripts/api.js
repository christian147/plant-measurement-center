window.onload = function () {
  const StartWateringLabel = 'Start watering';
  const StopWateringLabel = 'Stop watering';
  const waterApiEndpoint = '/api/water';

  const temperature = document.getElementById('temperature');
  const humidity = document.getElementById('humidity');
  const soilHumidity = document.getElementById('soilHumidity');
  const watering = document.getElementById('watering');
  var refreshIn = 10000;

  getMeasurements();
  reloadData();

  watering.addEventListener('click', isWatering);
  function isWatering() {
    var xmlhttp = new XMLHttpRequest();
    xmlhttp.onreadystatechange = function () {
      if (xmlhttp.readyState == XMLHttpRequest.DONE && xmlhttp.status == 200) {
        var isWatering = JSON.parse(xmlhttp.responseText);
        watering.textContent = isWatering
          ? StopWateringLabel
          : StartWateringLabel;
      }
    };
    xmlhttp.open('POST', waterApiEndpoint, true);
    xmlhttp.send();
  }

  function getMeasurements() {
    var xmlhttp = new XMLHttpRequest();
    xmlhttp.onreadystatechange = function () {
      if (xmlhttp.readyState == XMLHttpRequest.DONE && xmlhttp.status == 200) {
        var measurements = JSON.parse(xmlhttp.responseText);
        temperature.textContent = measurements.temperature;
        humidity.textContent = measurements.humidity;
        soilHumidity.textContent = measurements.soilHumidity;
        refreshIn = measurements.refreshIn;
        watering.textContent = measurements.isWatering
          ? StopWateringLabel
          : StartWateringLabel;
      }
    };
    xmlhttp.open('GET', '/api/measurements', true);
    xmlhttp.send();
  }

  function reloadData() {
    setTimeout(function () {
      getMeasurements();
      reloadData();
    }, refreshIn);
  }
};