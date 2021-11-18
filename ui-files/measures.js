
let currentInterval = undefined;

async function renderMeasurementValues () {
    const measurements = await endpoints.getMeasurementValues();

    const temperatureCurrentEl = document.getElementById('measure_temp_current');
    temperatureCurrentEl.innerHTML = measurements.temperature.current.value;
    const temperatureCurrentTimestampEl = document.getElementById('measure_temp_current_timestamp');;
    temperatureCurrentTimestampEl.innerHTML = measurements.temperature.current.timestamp;


}



async function renderMeasurementConfig () {
    // const config = await endpoints.getMeasurementsConfig();

    currentInterval = 5;
}




let poolingIntervalID = undefined;

function defineMeasurementPooling () {
    poolingIntervalID = setInterval(renderMeasurementValues, currentInterval * 1000);

}


function onUpdateCurrentInterval () {
    // AJAX request
    currentInterval = Math.floor(Math.random()*10);
    
    clearInterval(poolingIntervalID);
    defineMeasurementPooling();
}



renderMeasurementValues();
renderMeasurementConfig();
defineMeasurementPooling();
