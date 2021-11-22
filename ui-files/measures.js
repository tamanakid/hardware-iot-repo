
let currentInterval = undefined;

let isFirstCall = true;
let isAJAXCallInProgress = false;

async function renderMeasurementValues () {
    if (!isAJAXCallInProgress) {
        isAJAXCallInProgress = true;

        let loaderElements;
        if (isFirstCall) {
            loaderElements = document.getElementsByClassName('content_info');
            loaderElements[0].appendChild(renderLoader('large'));
            loaderElements[1].appendChild(renderLoader('large'));
        } else {
            loaderElements = document.getElementsByClassName('loader_current');
            loaderElements[0].appendChild(renderLoader('small'));
            loaderElements[1].appendChild(renderLoader('small'));
        }
    
        const measurements = await endpoints.getMeasurementValues();
    
        if (isFirstCall) {
            for (let i = 0; i < loaderElements.length; i++) {
                loaderElements[i].classList.remove('content_info--hide-measures');
                const firstCallLoader = loaderElements[i].getElementsByClassName('loader')[0];
                firstCallLoader.remove();
            }
            isFirstCall = false;
        } else {
            loaderElements[0].innerHTML = '';
            loaderElements[1].innerHTML = '';
        }
        isAJAXCallInProgress = false;
    
        renderTabMeasurements(measurements.temperature, 'temp');
        renderTabMeasurements(measurements.humidity, 'humidity');
    }
}

function renderTabMeasurements(tabMeasurements, type) {
    
    // Temperature: Current value
    const temperatureCurrentEl = document.getElementById(`measure_${type}_current`);
    temperatureCurrentEl.innerHTML = tabMeasurements.current.value;
    const temperatureCurrentTimestampEl = document.getElementById(`measure_${type}_current_timestamp`);
    temperatureCurrentTimestampEl.innerHTML = tabMeasurements.current.timestamp;

    // Temperature: Latest means
    if (tabMeasurements.means) {
        tabMeasurements.means.forEach((mean, index) => {
            const temperatureMeanEl = document.getElementById(`measure_${type}_mean${index}`);
            temperatureMeanEl.innerHTML = mean.value;
            const temperatureMeanTimestampEl = document.getElementById(`measure_${type}_mean${index}_timestamp`);
            temperatureMeanTimestampEl.innerHTML = mean.timestamp;
        });
    }

    // Temperature: Record max and min
    if (tabMeasurements.recordMax) {
        const temperatureMaxEl = document.getElementById(`measure_${type}_max`);
        temperatureMaxEl.innerHTML = tabMeasurements.recordMax.value;
        const temperatureMaxTimestampEl = document.getElementById(`measure_${type}_max_timestamp`);
        temperatureMaxTimestampEl.innerHTML = tabMeasurements.recordMax.timestamp;
    }
    if (tabMeasurements.recordMin) {
        const temperatureMinEl = document.getElementById(`measure_${type}_min`);
        temperatureMinEl.innerHTML = tabMeasurements.recordMin.value;
        const temperatureMinTimestampEl = document.getElementById(`measure_${type}_min_timestamp`);
        temperatureMinTimestampEl.innerHTML = tabMeasurements.recordMin.timestamp;
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
