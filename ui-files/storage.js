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

/**
 * Pending functionality:
 * 1. Refresh content button (both for list and file-content)
 * 2. Remove content from DOM while loading/refreshing (And remove event listener)
 * 3. Create reusable loader
 */


