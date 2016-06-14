function Channel(parent, channelName) {
	this.root = (new Element('li')).text(channelName);
	parent.add(this.root);
}
Channel.prototype.remove = function() {
	this.root.remove();
}
Channel.prototype.select = function(bSelect) {
	if (bSelect) {
		this.root.get().classList.add('selected');
	} else {
		this.root.get().classList.remove('selected');
	}
}

function Server(parent, serverId, serverName) {
	this.root = new Element('li');
	this.channels = {};
	this.serverId = serverId;
	this.serverName = (new Element('div')).text(serverName).attr('data-id', serverId);
	this.channelRoot = new Element('ul');
	parent.add(
		this.root
			.add(this.serverName)
			.add(this.channelRoot)
	);
}
Server.prototype.remove = function() {
	this.root.remove();
}
Server.prototype.add = function(channelName) {
	return this.channels[channelName] = new Channel(this.channelRoot, channelName);
}

function ServerList(root) {
	this.root = root;
	this.byType = {};
}
ServerList.prototype.clear = function(type) {
	var serverData = this.byType[type];
	if (!serverData) return;
	var servers = serverData.servers;
	for (var i in servers)
		servers[i].remove();
	delete this.byType[type];
}
ServerList.prototype.add = function(type, serverId, serverName) {
	var typeData = this.byType[type];
	if (!typeData) {
		typeData = this.byType[type] = {
			root: new Element('ul'),
			servers: {}
		};
		this.root.add(typeData.root);
	}
	return typeData.servers[serverId] = new Server(typeData.root, serverId, serverName);
}
