class UserBase {
    constructor(details, channel) {
        this.details = details;
        this.channel = channel;
        this.channel.users[this.name()] = this;
        this.userEntry = this.createEntry();
        this.channel.userlist.add(this.userEntry);
    }
    createEntry() {
        return q('<div>').text(this.name());
    }
    name() {
        return this.details.name;
    }
    remove() {
        this.userEntry.remove();
        delete this.channel.users[this.name];
    }
}

class ChannelBase {
    constructor(name, server) {
        this.name = name;
        this.server = server;
        this.server.channels[name] = this;
        this.channelEntry = this.createEntry();
        this.server.serverEntry.add(this.channelEntry);
        this.backlog = q('<div>').addClass('backlog');
        this.userlist = q('<div>').addClass('userlist');
        this.users = {};
        if (!this.server.service.chat.activeChannel)
            this.activate();
    }
    createEntry() {
        return q('<div>').text(this.name).addClass('channel');
    }
    get(userName) {
        return this.users[userName];
    }
    activate() {
        this.server.service.chat.activate(this);
    }
    remove() {
        this.channelEntry.remove();
        delete this.server.channels[this.name];
    }
}

class ServerBase {
    constructor(name, service) {
        this.name = name;
        this.service = service;
        this.service.servers[name] = this;
        this.serverEntry = this.createEntry();
        this.service.serviceEntry.add(this.serverEntry);
        this.channels = {};
    }
    createEntry() {
        return q('<div>')
            .addClass('serverPadding')
            .add(q('<div>').text(this.name).addClass('server'));
    }
    remove() {
        for (var i in this.channels)
            this.channels[i].remove();
        this.serverEntry.remove();
        delete this.service.servers[this.name];
    }
    get(channelName) {
        return this.channels[channelName];
    }
}

class ServiceBase {
    constructor(name, chat) {
        this.name = name;
        this.chat = chat;
        this.chat.services[name] = this;
        this.serviceEntry = this.createEntry();
        this.chat.channellist.add(this.serviceEntry);
        this.servers = {};
    }
    createEntry() {
        return q('<div>');
    }
    get(serverName) {
        return this.servers[serverName];
    }
    handleCommand() {
        throw "ServiceBase must be derived";
    }
}

class Chat {
    constructor() {
        this.services = {};
        this.channellist = q('#chat-channellist');
        this.backlog = q('#chat-backlog');
        this.userlist = q('#chat-userlist');

        this.activeChannel = null;

        this.ws = null;
        this.pingInterval = null;
    }
    deactivate() {
        this.backlog.children().remove();
        this.userlist.children().remove();
        this.activeChannel = null;
    }
    activate(channel) {
        this.deactivate();
        this.backlog.add(channel.backlog);
        this.userlist.add(channel.userlist);
        this.activeChannel = channel;
    }
    get(serviceName) {
        return this.services[serviceName];
    }

    login(username, password) {
        var proto = document.location.protocol.replace(/^http/, "ws");
        this.ws = new WebSocket(proto+"//"+document.domain+":8080/ws");
        this.ws.onopen = (cxn)=>this.onWsOpen(cxn, username, password);
        this.ws.onclose = ()=>this.onWsClose(username, password);
        this.ws.onmessage = (msg)=>this.onMessage(msg);
    }
    onWsOpen(cxn, username, password) {
        var username = "user"; // TODO: till login form exists
        var password = "password";

        this.send("LOGIN "+username+" "+password+"\n");
    }
    onWsClose(username, password) {
        if (this.pingInterval !== null)
            clearInterval(this.pingInterval);
        setTimeout(()=>login(username, password), 1000);
    }
    onMessage(msg) {
        if (msg.data instanceof Blob || msg.data instanceof ArrayBuffer) {
            // unpack message
            var reader = new FileReader();
            reader.onload = (e)=>this.handleCommand(e.target.result);
            reader.readAsText(msg.data);
        } else {
            handleCommand(msg.data);
        }
    }
    send(val) {
        if ('string' === typeof val) {
            console.log("OUT: "+val);
            this.ws.send(val);
        } else {
            console.log("OUT: "+JSON.stringify(val));
            this.ws.send(JSON.stringify(val));
        }
    }
    handleCommand(msg) {
        console.log("MSG: "+msg);
        if (msg.length === 0) return; // skip empty messages

        var json = null;
        try {
            json = JSON.parse(msg);
        } catch(e) {
            console.log("Failed to parse >>"+msg+"<<. Reason: "+e);
            return;
        }

        if (json.type) {
            // service specific message
            var service = this.get(json.type);
            if (service)
                service.handleCommand(json);
            else
                console.log("Service missing: "+json.type);
            return;
        }

        // handle standard messages
        switch (json.cmd) {
        case 'login':
            this.pingInterval = setInterval(()=>this.send({cmd:'ping'}), 60000);
            this.send({cmd:'querysettings'});
            break;
        }
    }
}
