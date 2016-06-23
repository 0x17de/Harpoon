var currentPage;
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
function closepage_(args) {
	var e = args[0] || window.event;
	if (e.stopPropagation) e.stopPropagation();
	e.cancelBubble = true;
	if (currentPage.get() == e.target)
		closepage();
}
