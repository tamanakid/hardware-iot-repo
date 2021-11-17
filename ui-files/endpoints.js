// export default endpoints = {
//     a: 6
// };



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
    getAllFilesFromStorage,
    getFileFromStorage,
};
