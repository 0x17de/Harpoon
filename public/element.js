Element = function(tagOrElement) {
	if ((typeof tagOrElement) === 'string') {
		if (tagOrElement[0] === '#') {
			this.e = document.getElementById(tagOrElement.substr(1));
		} else {
			this.e = document.createElement(tagOrElement);
		}
	} else {
		this.e = tagOrElement;
	}
};
Element.prototype.get = function() {
	return this.e;
}
Element.prototype.text = function(text) {
	this.add(document.createTextNode(text));
	return this;
};
Element.prototype.add = function(e) {
	this.e.appendChild((e instanceof Element) ? e.get() : e);
	return this;
};
Element.prototype.class = function(c) {
	this.e.classList.add(c);
	return this;
};
Element.prototype.val = function(t) {
	var val = this.e.value;
	if (t !== void 0) this.e.value = t;
	return val;
};
