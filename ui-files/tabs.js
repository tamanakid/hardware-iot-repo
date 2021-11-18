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

// function onUnmountTab (tab) {
//     switch (tab) {
//         case 'storage':
//             onUnmountStorage();
//             break;
//     }
// }



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


/* Page Initialization */

function onInit() {
    let currentTab = localStorage.getItem(LOCAL_STORAGE_KEY_TAB);
    if (currentTab === null) {
        currentTab = 'temperature';
    }
    setTab(currentTab);
}
onInit();
