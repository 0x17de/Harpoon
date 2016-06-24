function Service(servicename, servicetype) {
	this.serviceview = new Element('#serviceconfig-'+servicetype);
	this.servicetype = servicetype;
	this.selector = new Element('option').text(servicename);
	this.selector.get().onclick = this.show;
	this.selector.get().value = servicetype;
	serviceSelector.add(this.selector);
}
Service.map = {};
Service.active = null;
Service.select = function(e) {
	Service.map[e.value].show();
};
Service.add = function(service) {
	Service.map[service.servicetype] = service;
};
Service.prototype.show = function() {
	if (Service.active)
		Service.active.hide();
	this.serviceview.show();
};
Service.prototype.hide = function() {
	this.serviceview.hide();
};

function ServiceIrc() {
	Service.call(this, 'IRC', 'irc');
}
ServiceIrc.prototype = Object.create(Service.prototype);
ServiceIrc.prototype.constructor = ServiceIrc;
ServiceIrc.prototype.save = function() {
	
};

