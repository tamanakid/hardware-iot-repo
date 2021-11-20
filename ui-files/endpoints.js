// export default endpoints = {
//     a: 6
// };




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




const endpoints = {
    getMeasurementValues,
    getAllFilesFromStorage,
    getFileFromStorage,
};
