function onRequest(endpoint, headers) {

    const promise = new Promise((resolve, reject) => {
        function reqListener () {
            if (headers.isPlainResponse) {
                resolve(this.responseText);
            } else {
                resolve(JSON.parse(this.responseText.replace(/[\r\n]/g, '')));
            }
            // removeEventListener after callback?
        }
        
        var oReq = new XMLHttpRequest();
        oReq.addEventListener("load", reqListener);
        oReq.open(headers.method, endpoint);
        if (headers.params) {
            headers.params.forEach(param => {
                oReq.setRequestHeader(param.key, param.value);
            });
        }
        oReq.send(headers.params || null);
    })

	return promise;
}


function getMeasurementValues (tab) {
    const endpointUrl = `/api/${tab}`;
    return onRequest(endpointUrl, { method: "GET" });
}

function changeConfig (tab, configField, params) {
    const endpointUrl = `/api/${tab}/${configField}`;
    return onRequest(endpointUrl, { method: "POST", params });
}

/** Clock endpoints */

function getClock () {
    const endpointUrl = '/api/clock/get';
    return onRequest(endpointUrl, { method: "GET" });
}

function setClock (params) {
    const endpointUrl = '/api/clock/set';
    return onRequest(endpointUrl, { method: "POST", params });
}

function getBatteryLevel () {
    const endpointUrl = '/api/battery';
    return onRequest(endpointUrl, { method: "GET" });
}

function getPageInit () {
    const endpointUrl = '/api/init';
    return onRequest(endpointUrl, { method: "GET" });
}

/** Storage endpoints */

function getAllFilesFromStorage () {
    const endpointUrl = '/api/files/all';
    return onRequest(endpointUrl, { method: "GET" });
}

function getFileFromStorage (filename) {
	const endpointUrl = `/api/files/get`;
    const params = [{ key: 'filename', value: filename }]
    const isPlainResponse = true;

    return onRequest(endpointUrl, { method: "POST", params, isPlainResponse });
}

async function deleteFlash () {
	const endpointUrl = `/api/files/delete`;
    return onRequest(endpointUrl, { method: "GET" });
}


const endpoints = {
	getMeasurementValues,
    changeConfig,
    getClock,
    setClock,
    getBatteryLevel,
    getPageInit,
	getAllFilesFromStorage,
	getFileFromStorage,
	deleteFlash,
};




function renderLoader(size, tab) {
	const loaderEl = document.createElement('div');

	loaderEl.classList.add('loader');
	loaderEl.classList.add(`loader-${size}`);
    loaderEl.classList.add(`loader_${tab}`);

	return loaderEl;
}



let intervalRates = {
    temp: undefined,
    humi: undefined,
};
let poolingIntervalIDs = {
    temp: undefined,
    humi: undefined,
};
let isFirstCall = {
    temp: true,
    humi: true,
};
let isAJAXCallInProgress = {
    temp: false,
    humi: false,
};

async function renderMeasurementValues (tab) {
    if (!isAJAXCallInProgress[tab]) {
        isAJAXCallInProgress[tab] = true;

        let loaderElement;
        if (isFirstCall[tab]) {
            loaderElement = document.querySelector(`.content--loading#content_${tab}`);
            loaderElement.appendChild(renderLoader('large', tab));
        } else {
            loaderElement = document.getElementsByClassName(`loader_current_${tab}`)[0];
            if (loaderElement) {
                loaderElement.appendChild(renderLoader('small', tab));
            }
        }
    
        const measures = await endpoints.getMeasurementValues(tab);
    
        if (isFirstCall[tab]) {
            loaderElement.classList.remove('content--loading');
            const firstCallLoader = loaderElement.getElementsByClassName(`loader loader_${tab}`)[0];
            if (firstCallLoader) {
                firstCallLoader.remove();
            }
            isFirstCall[tab] = false;
        } else {
            loaderElement.innerHTML = '';
        }
        isAJAXCallInProgress[tab] = false;
    
        renderTabMeasurements(measures, tab);

        if (intervalRates[tab] !== measures.config.rate) {
            onSetCurrentInterval(tab, measures.config.rate);
        }
    }
}

function renderTabMeasurements(tabMeasurements, tab) {
    
    // Measurement: Current value
    const measurementCurrentEl = document.getElementById(`measure_${tab}_current`);
    measurementCurrentEl.innerHTML = (tab === 'temp') ? onConvertTemperature(tabMeasurements.current.value) : tabMeasurements.current.value;
    const measurementCurrentTimestampEl = document.getElementById(`measure_${tab}_current_timestamp`);
    measurementCurrentTimestampEl.innerHTML = tabMeasurements.current.timestamp;

    // Measurement: Latest means
    if (tabMeasurements.means) {
        tabMeasurements.means.forEach((mean, index) => {
            const measurementMeanEl = document.getElementById(`measure_${tab}_mean${index}`);
            if (mean.value) {
                const measurementMeanValueEl = document.getElementById(`measure_${tab}_mean${index}_value`);
                measurementMeanValueEl.innerHTML = (tab === 'temp') ? onConvertTemperature(mean.value) : mean.value;
                measurementMeanEl.style.display = 'list-item';
            } else {
                measurementMeanEl.style.display = 'none';
            }
        });
    }

    // Measurement: Record max and min
    if (tabMeasurements.recordMax) {
        const measurementMaxEl = document.getElementById(`measure_${tab}_max`);
        measurementMaxEl.innerHTML = (tab === 'temp') ? onConvertTemperature(tabMeasurements.recordMax.value) : tabMeasurements.recordMax.value;
        const measurementMaxTimestampEl = document.getElementById(`measure_${tab}_max_timestamp`);
        measurementMaxTimestampEl.innerHTML = tabMeasurements.recordMax.timestamp;
    }
    if (tabMeasurements.recordMin) {
        const measurementMinEl = document.getElementById(`measure_${tab}_min`);
        measurementMinEl.innerHTML = (tab === 'temp') ? onConvertTemperature(tabMeasurements.recordMin.value) : tabMeasurements.recordMin.value;
        const measurementMinTimestampEl = document.getElementById(`measure_${tab}_min_timestamp`);
        measurementMinTimestampEl.innerHTML = tabMeasurements.recordMin.timestamp;
    }

    // Config

    // Alarm
    const alarmEl = document.getElementById(`alarm_led_${tab}`);
    if (tabMeasurements.alarm) {
        alarmEl.classList.add('alarm-on');
    } else {
        alarmEl.classList.remove('alarm-on');
    }
}

function onSetCurrentInterval(tab, value) {
    intervalRates[tab] = value;
    poolingIntervalIDs[tab] = setInterval(() => renderMeasurementValues(tab), intervalRates[tab] * 1000);
}

function onUpdateCurrentInterval(tab, value) {
    intervalRates[tab] = value;
    
    clearInterval(poolingIntervalIDs[tab]);
    poolingIntervalIDs[tab] = setInterval(() => renderMeasurementValues(tab), intervalRates[tab] * 1000);
}


renderMeasurementValues('temp');
renderMeasurementValues('humi');



async function onSubmitConfig (tab, configField) {
    const rateSelector = document.getElementById(`config_${tab}_${configField}`);
    // const params = `value=${rateSelector.value}`;

    // Validate Threshold
    let value = Number(rateSelector.value);
    if (configField === 'thres') {
        value = (tab === 'temp' && configField === 'thres') ? onDeconvertTemperature(value, currentTempUnits) : value;
        if (!value || value < 0 || value > 100) {
            alert('Value must be between 0 and 100 ºC (32 and 212 in ºF -- 273.2 and 373.2 in K)');
            return;
        }
    }

    const params = [{ key: 'value', value }];
    const response = await endpoints.changeConfig(tab, configField, params);

    if (response.success) {
        if (configField == 'rate') {
            onUpdateCurrentInterval(tab, response.value);
        }
        const valueEl = document.getElementById(`config_${tab}_${configField}_value`);
        if (valueEl) {
            const newValue = (tab === 'temp' && configField === 'thres') ? onConvertTemperature(response.value) : response.value;
            valueEl.innerHTML = newValue;
        }
    }
}

const tempRateSubmitBtn = document.getElementById('config_temp_rate_submit');
const humiRateSubmitBtn = document.getElementById('config_humi_rate_submit');
tempRateSubmitBtn.addEventListener('click', () => onSubmitConfig('temp', 'rate'));
humiRateSubmitBtn.addEventListener('click', () => onSubmitConfig('humi', 'rate'));

const tempThresSubmitBtn = document.getElementById('config_temp_thres_submit');
const humiThresSubmitBtn = document.getElementById('config_humi_thres_submit');
tempThresSubmitBtn.addEventListener('click', () => onSubmitConfig('temp', 'thres'));
humiThresSubmitBtn.addEventListener('click', () => onSubmitConfig('humi', 'thres'));



const clockValueEl = document.getElementById('clock_value');
const clockHourInput = document.getElementById('clock_set_hour');
const clockMinuteInput = document.getElementById('clock_set_min');

async function onGetClock () {
    const value = await endpoints.getClock();
    clockValueEl.innerHTML = `${value.hour}:${value.minute}`;
}

onGetClock();
setInterval(onGetClock, 30000);

async function onSubmitClock () {
    // const params = `hour=${clockHourInput.value}&minute=${clockMinuteInput.value}`;
    const params = [
        { key: 'hour', value: clockHourInput.value },
        { key: 'minute', value: clockMinuteInput.value },
    ];
    const { value } = await endpoints.setClock(params);
    clockValueEl.innerHTML = `${value.hour}:${value.minute}`;
    clockHourInput.value = `${value.hour}`;
    clockMinuteInput.value = `${value.minute}`;
}

const clockSubmitEl = document.getElementById('clock_submit');
clockSubmitEl.addEventListener('click', onSubmitClock);



let currentTempUnits = 'c';

function onChangeTemperatureUnits (event) {
    const previousTempUnits = currentTempUnits;
    currentTempUnits = event.target.value;

    let currentTempLabel = '';
    switch (currentTempUnits) {
        case 'c':
            currentTempLabel = 'ºC';
            break;
        case 'f':
            currentTempLabel = 'ºF';
            break;
        case 'k':
            currentTempLabel = 'K';
            break;
    }

    const tempUnitElements = document.getElementById('content_temp').querySelectorAll('.measure_value_units');
    
    for (let i = 0; i < tempUnitElements.length; i++) {
        tempUnitElements[i].innerHTML = currentTempLabel;
        
        const currentValueEl = tempUnitElements[i].previousElementSibling;
        const value = currentValueEl.innerHTML;
        const numberValue = Number(value);

        if (value && !isNaN(numberValue)) {
            const deconvertedValue = onDeconvertTemperature(numberValue, previousTempUnits);
            currentValueEl.innerHTML = onConvertTemperature(deconvertedValue);
        }
    }
}

function onConvertTemperature (value) {
    if (value === null)
        return null;
    
    let convertedValue = '';
    switch (currentTempUnits) {
        case 'c':
            convertedValue = Number(value);
            break;
        case 'f':
            convertedValue = (value*1.8) + 32;
            break;
        case 'k':
            convertedValue = value + 273.2;
            break;
    }
    return convertedValue.toFixed(1);
}

function onDeconvertTemperature (value, previousUnits) {
    let deconvertedValue = null;
    switch (previousUnits) {
        case 'c':
            deconvertedValue = Number(value);
            break;
        case 'f':
            deconvertedValue = (value - 32)/1.8; 
            break;
        case 'k':
            deconvertedValue = value - 273.2;
            break;
    }
    return deconvertedValue;
}


const tempUnitsSelect = document.getElementById('config_temp_units');
tempUnitsSelect.addEventListener('change', onChangeTemperatureUnits);



const batteryValueEl = document.getElementById('battery_value');

async function onGetBatteryLevel () {
    const { value } = await endpoints.getBatteryLevel();
    batteryValueEl.innerHTML = value;
}

onGetBatteryLevel();
setInterval(onGetBatteryLevel, 20000);



async function onGetPageInit () {
    await endpoints.getPageInit();
}
onGetPageInit();





const fileIcon = `
    <svg height="18px" viewBox="0 0 24 24" width="18px" fill="#004">
        <path d="M0 0h24v24H0V0z" fill="none"/>
        <path d="M8 16h8v2H8zm0-4h8v2H8zm6-10H6c-1.1 0-2 .9-2 2v16c0 1.1.89 2 1.99 2H18c1.1 0 2-.9 2-2V8l-6-6zm4 18H6V4h7v5h5v11z"/>
    </svg>
`;

function onRenderStorageFiles (filenames, listElement) {
    listElement.innerHTML = '';
    
    filenames.forEach(filename => {
        const fileElement = document.createElement('li');

        fileElement.classList.add('storage_nav_list_el');
        fileElement.setAttribute('name', filename);
        fileElement.innerHTML = fileIcon + filename;

        fileElement.addEventListener('click', onFetchFileContent);  
        
        listElement.appendChild(fileElement);
    });
}

async function onFetchStorageFiles () {
    const listElement = document.getElementById('storage_nav_list');
    
    listElement.appendChild(renderLoader('medium'));

    const filenames = await endpoints.getAllFilesFromStorage();

    onRenderStorageFiles(filenames, listElement);
}

async function onFetchFileContent (event) {
    const target = event.currentTarget;

    if (!target.classList.contains('storage_nav_list_el--selected')) {
        const listElement = document.getElementById('storage_nav_list');
        for (let i = 0; i < listElement.children.length; i++) {
            listElement.children[i].classList.remove('storage_nav_list_el--selected');
        }
        target.classList.add('storage_nav_list_el--selected');
        
        const displayElement = document.getElementById('storage_display');
        
        displayElement.innerHTML = '';
        displayElement.appendChild(renderLoader('medium'));
        displayElement.classList.add('storage_display--no-file');
        
        const filename = target.getAttribute('name');
        let fileContent = await endpoints.getFileFromStorage(filename);
        fileContent = fileContent.replaceAll('���', '\n');
        fileContent = fileContent.replaceAll('��', '\n');
        fileContent = fileContent.replaceAll('\n\r\n', '\n');
        
        displayElement.innerHTML = '';
        displayElement.classList.remove('storage_display--no-file');
        
        displayElement.innerHTML = fileContent;
    }
}

function onMountStorage () {
    const displayElement = document.getElementById('storage_display');

    displayElement.innerHTML = 'Select a file to view its contents';
    displayElement.classList.add('storage_display--no-file');

    const listElement = document.getElementById('storage_nav_list');

    listElement.innerHTML = '';

    onFetchStorageFiles();
}

async function onStorageDelete () {
    const isAcceptDelete = confirm('Are you sure you want to delete the entire flash?');

    if (isAcceptDelete) {
        const listElement = document.getElementById('storage_nav_list');
        listElement.appendChild(renderLoader('medium'));

        const displayElement = document.getElementById('storage_display');
        displayElement.innerHTML = 'Select a file to view its contents';
        displayElement.classList.add('storage_display--no-file');

        const undeletedFiles = await endpoints.deleteFlash();

        onRenderStorageFiles(undeletedFiles, listElement);
    }

}

const storageDeleteEl = document.getElementById('storage_delete');
storageDeleteEl.addEventListener('click', onStorageDelete);



const localStorage = window.localStorage;
const LOCAL_STORAGE_KEY_TAB = 'WEATHER_STATION_TAB';

/* DOM references */
const tabButtons = {
    temp: document.querySelector('[data-tab-id=temp]'),
    humi: document.querySelector('[data-tab-id=humi]'),
    storage: document.querySelector('[data-tab-id=storage]'),
}

const tabContents = {
    temp: document.querySelector('#content_temp'),
    humi: document.querySelector('#content_humi'),
    storage: document.querySelector('#content_storage'),
}

function onMountTab (newTab) {
    switch (newTab) {
        case 'storage':
            onMountStorage();
            break;
    }
}

/* Event handlers */

function setTab (newTab) {
    const tabs = Object.keys(tabContents);
    tabs.forEach((tab) => {
        if (tab === newTab) {
            tabContents[tab].classList.remove('content--hidden');
            tabButtons[tab].classList.add('container_tabs_el--selected');
        } else {
            tabContents[tab].classList.add('content--hidden');
            tabButtons[tab].classList.remove('container_tabs_el--selected');

            // onUnmountTab(tab)
        }
    });

    localStorage.setItem(LOCAL_STORAGE_KEY_TAB, newTab);

    onMountTab(newTab);
}

tabButtons.temp.onclick = function () {
    setTab('temp');
}

tabButtons.humi.onclick = function () {
    setTab('humi');
}

tabButtons.storage.onclick = function () {
    setTab('storage');
}

function onInit() {
    let currentTab = localStorage.getItem(LOCAL_STORAGE_KEY_TAB);
    if (currentTab === null) {
        currentTab = 'temp';
    }
    setTab(currentTab);
}
onInit();
