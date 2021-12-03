function onRequest(endpoint, headers, callback) {
	function reqListener () {
		callback(JSON.parse(this.responseText));
		// removeEventListener after callback?
	}
	
	var oReq = new XMLHttpRequest();
	oReq.addEventListener("load", reqListener);
	oReq.open(headers.method, endpoint);
    if (headers.params) {
        oReq.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
    }
    oReq.send(headers.params || null);
}


function getMeasurementValues (tab) {
    const endpointUrl = `/api/${tab}`;

	const promise = new Promise((resolve, reject) => {
		onRequest(endpointUrl, { method: "GET" }, function (response) {
			resolve(response);
		});
	});

	return promise;
}

function changeRate (tab, params) {
    const endpointUrl = `/api/${tab}/rate`;

	const promise = new Promise((resolve, reject) => {
		onRequest(endpointUrl, { method: "POST", params }, function (response) {
			resolve(response);
		});
	});

	return promise;
}

/** Storage endpoints */

function getAllFilesFromStorage () {
	const endpointUrl = '/storage';

	const response = [
			'logs/alarms.log',
			'logs/measures.log',
			'logs/records.log',
			'logs/wifi.log',
	];

	return new Promise((resolve, reject) => {
			setTimeout(() => {
					resolve(response);
			}, 2000);
	});
}

function getFileFromStorage (filename) {
	// const endpointUrl = `/file?name=${filename}`;
	const endpointUrl = `/api/test`;

	const promise = new Promise((resolve, reject) => {
		onRequest(endpointUrl, { method: "GET" }, function (response) {
			resolve(response['test-1']);
		});
	});

	return promise;
}

async function deleteFlash () {
	const endpointUrl = `/delete-flash`;

	const response = {
			success: true,
	};

	return new Promise((resolve, reject) => {
			setTimeout(() => {
					resolve(response);
			}, 4000);
	});
}


const endpoints = {
	getMeasurementValues,
    changeRate,
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

function renderTabMeasurements(tabMeasurements, type) {
    
    // Measurement: Current value
    const measurementCurrentEl = document.getElementById(`measure_${type}_current`);
    measurementCurrentEl.innerHTML = tabMeasurements.current.value;
    const measurementCurrentTimestampEl = document.getElementById(`measure_${type}_current_timestamp`);
    measurementCurrentTimestampEl.innerHTML = tabMeasurements.current.timestamp;

    // Measurement: Latest means
    if (tabMeasurements.means) {
        tabMeasurements.means.forEach((mean, index) => {
            const measurementMeanEl = document.getElementById(`measure_${type}_mean${index}`);
            measurementMeanEl.innerHTML = mean.value;
        });
    }

    // Measurement: Record max and min
    if (tabMeasurements.recordMax) {
        const measurementMaxEl = document.getElementById(`measure_${type}_max`);
        measurementMaxEl.innerHTML = tabMeasurements.recordMax.value;
        const measurementMaxTimestampEl = document.getElementById(`measure_${type}_max_timestamp`);
        measurementMaxTimestampEl.innerHTML = tabMeasurements.recordMax.timestamp;
    }
    if (tabMeasurements.recordMin) {
        const measurementMinEl = document.getElementById(`measure_${type}_min`);
        measurementMinEl.innerHTML = tabMeasurements.recordMin.value;
        const measurementMinTimestampEl = document.getElementById(`measure_${type}_min_timestamp`);
        measurementMinTimestampEl.innerHTML = tabMeasurements.recordMin.timestamp;
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



async function onSubmitRate (tab) {
    const rateSelector = document.getElementById(`config_${tab}_rate`);
    const params = `value=${rateSelector.value}`;
    const response = await endpoints.changeRate('temp', params);

    if (response.success) {
        onUpdateCurrentInterval(tab, response.value)
    }
}

const tempRateSubmitBtn = document.getElementById('config_temp_rate_submit');
const humiRateSubmitBtn = document.getElementById('config_humi_rate_submit');
tempRateSubmitBtn.addEventListener('click', () => onSubmitRate('temp'))
humiRateSubmitBtn.addEventListener('click', () => onSubmitRate('humi'))







const fileIcon = `
    <svg height="18px" viewBox="0 0 24 24" width="18px" fill="#004">
        <path d="M0 0h24v24H0V0z" fill="none"/>
        <path d="M8 16h8v2H8zm0-4h8v2H8zm6-10H6c-1.1 0-2 .9-2 2v16c0 1.1.89 2 1.99 2H18c1.1 0 2-.9 2-2V8l-6-6zm4 18H6V4h7v5h5v11z"/>
    </svg>
`;

async function onFetchStorageFiles () {
    const listElement = document.getElementById('storage_nav_list');
    
    listElement.appendChild(renderLoader('medium'));

    const filenames = await endpoints.getAllFilesFromStorage();
    
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
        const fileContent = await endpoints.getFileFromStorage(filename);
        
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

        await endpoints.deleteFlash();

        listElement.innerHTML = '';
        
        // in the end
        await onFetchStorageFiles();
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
