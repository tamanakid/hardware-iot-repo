function getMeasurementValues () {
	const endpointUrl = '/measurements';

	const response = {
			temperature: {
					current: {
							value: (10 + Math.random()*20).toFixed(1),
							timestamp: '09/06/2021 - 17:51:33'
					},
					means: [
							{ value: (10 + Math.random()*20).toFixed(1), timestamp: '09/06/2021 - 17:51:08' },
							{ value: (10 + Math.random()*20).toFixed(1), timestamp: '09/06/2021 - 17:50:08' },
							{ value: (10 + Math.random()*20).toFixed(1), timestamp: '09/06/2021 - 17:49:08' },
					],
					recordMax: {
							value: (30 + Math.random()*5).toFixed(1),
							timestamp: '09/06/2021 - 17:46:38'
					},
					recordMin: {
							value: (5 + Math.random()*5).toFixed(1),
							timestamp: '08/06/2021 - 17:46:38'
					},
			},
			humidity: {
					current: {
							value: (30 + Math.random()*30).toFixed(1),
							timestamp: '09/06/2021 - 17:51:33'
					},
					means: [
							{ value: (30 + Math.random()*30).toFixed(1), timestamp: '09/06/2021 - 17:51:08' },
							{ value: (30 + Math.random()*30).toFixed(1), timestamp: '09/06/2021 - 17:50:08' },
							{ value: (30 + Math.random()*30).toFixed(1), timestamp: '09/06/2021 - 17:49:08' },
					],
					recordMax: {
							value: (60 + Math.random()*10).toFixed(1),
							timestamp: '09/06/2021 - 17:46:38'
					},
					recordMin: {
							value: (15 + Math.random()*15).toFixed(1),
							timestamp: '08/06/2021 - 17:46:38'
					},
			},
	};

	return new Promise((resolve, reject) => {
			setTimeout(() => {
					resolve(response);
			}, 1500);
	});
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
	const endpointUrl = `/file?name=${filename}`;

	// TODO: if file is empty?

	const response = `
			17/11/2021 09:36:20 > temperature alarm (value: 31.2 - threshold: 30.0)\n
			17/11/2021 09:36:25 > temperature alarm (value: 30.6 - threshold: 30.0)\n
			17/11/2021 09:36:30 > temperature alarm (value: 30.9 - threshold: 30.0)\n
			17/11/2021 09:41:05 > temperature alarm (value: 30.2 - threshold: 30.0)\n
	`;

	return new Promise((resolve, reject) => {
			setTimeout(() => {
					resolve(response);
			}, 2000);
	});
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
	getAllFilesFromStorage,
	getFileFromStorage,
	deleteFlash,
};




function renderLoader(size) {
	const loaderEl = document.createElement('div');

	loaderEl.classList.add('loader');
	loaderEl.classList.add(`loader-${size}`);

	return loaderEl;
}




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
    temperature: document.querySelector('[data-tab-id=temperature]'),
    humidity: document.querySelector('[data-tab-id=humidity]'),
    storage: document.querySelector('[data-tab-id=storage]'),
}

const tabContents = {
    temperature: document.querySelector('#content_temperature'),
    humidity: document.querySelector('#content_humidity'),
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

tabButtons.temperature.onclick = function () {
    setTab('temperature');
}

tabButtons.humidity.onclick = function () {
    setTab('humidity');
}

tabButtons.storage.onclick = function () {
    setTab('storage');
}

function onInit() {
    let currentTab = localStorage.getItem(LOCAL_STORAGE_KEY_TAB);
    if (currentTab === null) {
        currentTab = 'temperature';
    }
    setTab(currentTab);
}
onInit();
