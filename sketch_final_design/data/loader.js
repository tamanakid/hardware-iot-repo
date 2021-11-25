function renderLoader(size) {
    const loaderEl = document.createElement('div');

    loaderEl.classList.add('loader');
    loaderEl.classList.add(`loader-${size}`);

    return loaderEl;
}
