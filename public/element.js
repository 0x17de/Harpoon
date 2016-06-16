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
Element.prototype.remove = function() {
	this.e.parentNode.removeChild(this.e);
}
Element.prototype.purge = function() {
	this.remove();
	this.e = null;
}
Element.prototype.get = function() {
	return this.e;
}
Element.prototype.text = function(text) {
	if (this.textElement) {
		this.textElement.data = text;
	} else {
		this.textElement = document.createTextNode(text);
		this.add(this.textElement);
	}
	return this;
};
Element.prototype.attr = function(attr, value) {
	if (value === void 0)
		return this.e.getAttribute(attr);
	this.e.setAttribute(attr, value);
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
