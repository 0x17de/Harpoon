class ChannelBase {
    constructor(name, server) {
        this.name = name;
        this.server = server;
        this.server.channels[name] = this;
        this.channelEntry = q('<div>').text(this.name).addClass('channel');
        this.server.serverEntry.add(this.channelEntry);
        this.backlog = q('<div>').addClass('backlog');
        this.userlist = q('<div>').addClass('userlist');
    }
    activate() {
        this.server.service.chat.activate(this);
    }
    remove() {
        this.channelEntry.remove();
        delete this.server.channels[this.name];
    }
    test() {
        this.userlist
            .add(q('<div>').text('User1'))
            .add(q('<div>').text('User2'))
            .add(q('<div>').text('User3'));
        this.backlog
            .add(q('<div>').text('TEXT'));
    }
}

class ServerBase {
    constructor(name, service) {
        this.name = name;
        this.service = service;
        this.service.servers[name] = this;
        this.serverEntry = q('<div>')
            .addClass('serverPadding')
            .add(q('<div>').text(this.name).addClass('server'));
        this.service.serviceEntry.add(this.serverEntry);
        this.channels = {};
    }
    remove() {
        for (var i in this.channels)
            this.channels[i].remove();
        this.serverEntry.remove();
        delete this.service.servers[this.name];
    }
    get(channelName) {
        return this.channels[channel];
    }
}

class ServiceBase {
    constructor(name, chat) {
        this.name = name;
        this.chat = chat;
        this.chat.services[name] = this;
        this.serviceEntry = q('<div>');
        this.chat.channellist.add(this.serviceEntry);
        this.servers = {};
    }
    get(serverName) {
        return this.servers[serverName];
    }
}

class Chat {
    constructor() {
        this.services = {};
        this.channellist = q('#chat-channellist');
        this.backlog = q('#chat-backlog');
        this.userlist = q('#chat-userlist');
    }
    activate(channel) {
        this.backlog.children().remove();
        this.backlog.add(channel.backlog);
        this.userlist.children().remove();
        this.userlist.add(channel.userlist);
    }
    get(serviceName) {
        return this.services[serviceName];
    }
}
