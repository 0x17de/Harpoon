function Channel(serverId, parent, channelName, bServerChannel) {
	var self = this;
	this.serverId = serverId;
	this.channelName = channelName;
	this.bServerChannel = bServerChannel;

	this.root = bServerChannel ? parent : (new Element('li')).text(channelName);
	this.root.get().onclick = function() {
		self.link();
	};
	this.backlogRoot = new Element('div');
	this.userlistRoot = new Element('div');
	this.users = {};
	if (!bServerChannel) parent.add(this.root);
}
Channel.active = null;
Channel.prototype.remove = function() {
	this.root.remove();
}
Channel.prototype.unlink = function() {
	if (this.root)
		this.root.get().classList.remove('selected');
	if (this.bServerChannel) return;
	var parent = this.backlogRoot.get().parentNode;
	if (parent) parent.removeChild(this.backlogRoot.get());
}
Channel.prototype.link = function() {
	if (Channel.active)
		Channel.active.unlink();
	Channel.active = this;
	this.root.get().classList.add('selected');
	backlog.add(this.backlogRoot);
	input.get().focus();
}
Channel.prototype.addUser = function(userName, userData) {
	this.users[userName] = userData;
	var userRoot = userData.root = new Element('div');
	userRoot.text(userName);
	this.userlistRoot.add(userRoot);
}

function Server(parent, serverId, serverName) {
	var self = this;
	this.root = new Element('li');
	this.channels = {};
	this.serverId = serverId;
	this.serverName = (new Element('div')).text(serverName).attr('data-id', serverId);
	this.serverLog = new Channel(this.serverId, this.serverName, '['+serverName+']', true);
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
	return this.channels[channelName] = new Channel(this.serverId, this.channelRoot, channelName);
}
Server.prototype.get = function(channelName) {
	var channel = this.channels[channelName];
	if (!channel) return null;
	return channel;
}
Server.prototype.getServerLog = function() {
	return this.serverLog;
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
ServerList.prototype.get = function(type, serverId) {
	var typeData = this.byType[type];
	if (!typeData) return null;
	var server = typeData.servers[serverId];
	if (!server) return null;
	return server;
}
