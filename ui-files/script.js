const tabButtons = {
    temperature: document.querySelector('[data-tab-id=temperature]'),
    humidity: document.querySelector('[data-tab-id=humidity]'),
    storage: document.querySelector('[data-tab-id=storage]'),
}


const tabContents = {
    temperature: document.querySelector('#content_temperature'),
    humidity: document.querySelector('#content_humidity'),
    // storage: document.querySelector('#content_storage'),
}


tabButtons.temperature.onclick = function () {
    tabContents.temperature.classList.remove('content--hidden');
    tabContents.humidity.classList.add('content--hidden');
    // tabContents.storage.classList.add('content--hidden');
}

tabButtons.humidity.onclick = function () {
    tabContents.humidity.classList.remove('content--hidden');
    tabContents.temperature.classList.add('content--hidden');
    // tabContents.storage.classList.add('content--hidden');
}