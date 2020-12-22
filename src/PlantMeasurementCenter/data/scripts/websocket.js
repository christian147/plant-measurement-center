window.onload = function () {
  const StartWateringLabel = 'Start watering';
  const StopWateringLabel = 'Stop watering';

  const temperature = document.getElementById('temperature');
  const humidity = document.getElementById('humidity');
  const soilHumidity = document.getElementById('soilHumidity');
  const watering = document.getElementById('watering');

  var connection = new WebSocket(
    'ws://' + location.hostname + ':81/',
    ['arduino'],
  );

  watering.addEventListener('click', water);
  function water() {
    const data = {
      command: 'water',
    };

    const json = JSON.stringify(data);
    connection.send(json);
  }

  connection.onmessage = function (response) {
    const response = JSON.parse(response.data);
    if (response.command === 'measurements') {
      temperature.textContent = response.temperature;
      humidity.textContent = response.humidity;
      soilHumidity.textContent = response.soilHumidity;
      watering.textContent = response.isWatering ? StopWateringLabel : StartWateringLabel;
    } else if (response.command === 'water') {
      watering.textContent = response.isWatering ? StopWateringLabel : StartWateringLabel;
    }
  };
};
