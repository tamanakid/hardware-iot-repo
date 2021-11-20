
let currentInterval = undefined;

let isFirstCall = true;
let isAJAXCallInProgress = false;

async function renderMeasurementValues () {
    if (!isAJAXCallInProgress) {
        isAJAXCallInProgress = true;

        let loaderTemperatureEl;
        if (isFirstCall) {
            loaderTemperatureEl = document.getElementById('temperature_content_info');
            loaderTemperatureEl.appendChild(renderLoader('large'));
        } else {
            loaderTemperatureEl = document.getElementById('loader_temp_current');
            loaderTemperatureEl.appendChild(renderLoader('small'));
        }
    
        const measurements = await endpoints.getMeasurementValues();
    
        if (isFirstCall) {
            loaderTemperatureEl.classList.remove('content_info--hide-measures');
            const firstCallLoader = loaderTemperatureEl.getElementsByClassName('loader')[0];
            firstCallLoader.remove();
            isFirstCall = false;
        } else {
            loaderTemperatureEl.innerHTML = '';
        }
        isAJAXCallInProgress = false;
    
        // Temperature: Current value
        const temperatureCurrentEl = document.getElementById('measure_temp_current');
        temperatureCurrentEl.innerHTML = measurements.temperature.current.value;
        const temperatureCurrentTimestampEl = document.getElementById('measure_temp_current_timestamp');
        temperatureCurrentTimestampEl.innerHTML = measurements.temperature.current.timestamp;

        // Temperature: Latest means
        if (measurements.temperature.means) {
            measurements.temperature.means.forEach((mean, index) => {
                const temperatureMeanEl = document.getElementById(`measure_temp_mean${index}`);
                temperatureMeanEl.innerHTML = mean.value;
                const temperatureMeanTimestampEl = document.getElementById(`measure_temp_mean${index}_timestamp`);
                temperatureMeanTimestampEl.innerHTML = mean.timestamp;
            });
        }

        // Temperature: Record max and min
        if (measurements.temperature.recordMax) {
            const temperatureMaxEl = document.getElementById('measure_temp_max');
            temperatureMaxEl.innerHTML = measurements.temperature.recordMax.value;
            const temperatureMaxTimestampEl = document.getElementById('measure_temp_max_timestamp');
            temperatureMaxTimestampEl.innerHTML = measurements.temperature.recordMax.timestamp;
        }
        if (measurements.temperature.recordMin) {
            const temperatureMinEl = document.getElementById('measure_temp_min');
            temperatureMinEl.innerHTML = measurements.temperature.recordMin.value;
            const temperatureMinTimestampEl = document.getElementById('measure_temp_min_timestamp');
            temperatureMinTimestampEl.innerHTML = measurements.temperature.recordMin.timestamp;
        }
    }
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
