var currentPage;
var currentPopup;
function closepage(e) {
    if (e) {
        while (!e.classList.contains('page'))
            e = e.parentNode;
        (new Element(e)).hide();
    } else {
        if (currentPage)
            currentPage.hide();
    }
    pageChat.get().classList.remove('blur');
    pageTransparent.hide();
}
function showpage(pagename) {
    pageChat.get().classList.add('blur');
    pageTransparent.show();
    (currentPage = new Element('#page-'+pagename)).show();
}
function closepopup(e) {
    if (e) {
        while (!e.classList.contains('popup'))
            e = e.parentNode;
        (new Element(e)).hide();
    } else {
        if (currentPopup)
            currentPopup.hide();
    }
    pageTransparentPopup.hide();
}
function showpopup(popupname) {
    pageTransparentPopup.show();
    (currentPopup = new Element('#popup-'+popupname)).show();
}
function closepage_(args) {
    var e = args[0] || window.event;
    if (e.stopPropagation) e.stopPropagation();
    e.cancelBubble = true;
    if (currentPage && currentPage.get() == e.target)
        closepage();
}
function closepopup_(args) {
    var e = args[0] || window.event;
    if (e.stopPropagation) e.stopPropagation();
    e.cancelBubble = true;
    if (currentPopup && currentPopup.get() == e.target)
        closepopup();
}
