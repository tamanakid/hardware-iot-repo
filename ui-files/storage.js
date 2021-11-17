
async function onFetchStorageFiles () {

    const listElement = document.getElementById('storage_nav_list');
    
    listElement.innerHTML = 'Loading';
    const files = await endpoints.getAllFilesFromStorage();
    listElement.innerHTML = '';


    files.forEach(file => {
        const fileElement = document.createElement('li');

        fileElement.classList.add('storage_nav_list_el');
        fileElement.setAttribute('name', file);
        fileElement.innerHTML = file;

        fileElement.addEventListener('click', onFetchFileContent);  
        
        listElement.appendChild(fileElement);
    });



}


async function onFetchFileContent (event) {
    const displayElement = document.getElementById('storage_display');

    displayElement.innerHTML = 'Loading';
    const filename = event.target.getAttribute('name');
    const fileContent = await endpoints.getFileFromStorage(filename);
    displayElement.innerHTML = '';

    displayElement.classList.remove('storage_display--no-file');
    displayElement.innerHTML = fileContent;

}


/**
 * Pending functionality:
 * 1. Refresh content button (both for list and file-content)
 * 2. Remove content from DOM while loading/refreshing (And remove event listener)
 * 3. Create reusable loader
 */
