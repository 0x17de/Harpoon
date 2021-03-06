class UserBase {
    constructor(details, channel) {
        this.details = details;
        this.channel = channel;
        this.channel.users[this.name()] = this;
        this.userEntry = this.createEntry();
        this.channel.userlist.add(this.userEntry);
    }
    clear() {
        this.remove();
    }
    createEntry() {
        return q('<div>').text(this.name());
    }
    rename(newName) {
        this.channel._rename(this.details.name, newName);
        this.details.name = newName;
        this.userEntry.text(newName);
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
        this.backlogRequested = false;
        this.channelEntry = this.createEntry();
        this.server.serverEntry.add(this.channelEntry);
        this.backlog = q('<div>').addClass('backlog');
        this.userlist = q('<div>').addClass('userlist');
        this.users = {};
        this.scrollPosition = 'bottom';
        this.disabled = false;
        if (!this.server.service.chat.activeChannel)
            this.activate();
    }
    onBacklogScrollTop() {
        // handle backlog requests
        if (!this.backlogRequested) {
            this.backlogRequested = true;
            this.requestBacklog();
        }
    }
    requestBacklog() {
        console.log("Channel class needs to override 'requestBacklog()'");
        /* ... */
    }
    clear() {
        for (var i in this.users)
            this.users[i].clear();
        this.remove();
    }
    setDisabled(disabled) {
        if (disabled) {
            this.channelEntry.addClass('disabled');
        } else {
            this.channelEntry.removeClass('disabled');
            if (this.backlog.parent().scroll()[0].top == 0) {
                if (!this.backlogRequested) {
                    this.backlogRequested = true;
                    this.requestBacklog();
                }
            }
        }
        this.disabled = disabled;
    }
    getDisabled() {
        return this.disabled;
    }
    createEntry() {
        return q('<div>')
            .text(this.name)
            .addClass('channel')
            .on('click', ()=>this.activate());
    }
    _rename(userName, newUserName) {
        var user = this.users[userName];
        this.users[newUserName] = user;
        delete this.users[userName];
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
    clear() {
        for (var i in this.channels)
            this.channels[i].clear();
        this.remove();
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
    clear() {
        for (var i in this.servers)
            this.servers[i].clear();
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
        this.message = q('#message');
        this.message.on('keydown', (event)=>this.messageKey(event));

        this.activeChannel = null;
        this.backlog.on('scroll', (e)=>{
            if (!this.activeChannel) return;
            if (this.backlog.scroll()[0].top == 0)
                this.activeChannel.onBacklogScrollTop(); // checks for backlog request
            if (this.checkScroll()) { // auto scroll down check
                this.activeChannel.scrollPosition = 'bottom';
            } else {
                this.activeChannel.scrollPosition = this.backlog.scroll()[0].top;
            }
        });

        this.ws = null;
        this.pingInterval = null;
    }
    clear() {
        this.deactivate();
        for (var i in this.services)
            this.services[i].clear();
    }
    deactivate() {
        this.backlog.children().remove();
        this.userlist.children().remove();
        if (this.activeChannel) {
            this.activeChannel.channelEntry.removeClass('active');
            this.activeChannel = null;
        }
    }
    activate(channel) {
        this.deactivate();
        this.backlog.add(channel.backlog);
        this.userlist.add(channel.userlist);
        this.activeChannel = channel;
        var scrollPosition = channel.scrollPosition;
        channel.channelEntry.addClass('active');
        channel.channelEntry.removeClass('message', 'highlight');
        this.backlog.scroll(scrollPosition);
        if (this.backlog.scroll()[0].top == 0)
            this.activeChannel.onBacklogScrollTop(); // checks for backlog request
    }
    checkScroll() {
        var scroll = this.backlog.scroll()[0];
        var bounds = this.backlog.bounds()[0];
        return scroll.height - bounds.height <= scroll.top + 10;
    }
    doScroll() {
        this.backlog.scroll(this.activeChannel.scrollPosition);
    }

    highlight(channel, type) {
        if (this.activeChannel === channel) return;
        channel.channelEntry.addClass(type);
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
        this.clear();
        this.send("LOGIN "+username+" "+password+"\n");
    }
    onWsClose(username, password) {
        if (this.pingInterval !== null)
            clearInterval(this.pingInterval);
        if (this.loginSuccess) {
            setTimeout(()=>this.login(username, password), 1000);
        } else {
            q('#login,#login-error').css({'display':'block'});
        }
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
    messageKey(event) {
        if (event.keyCode == 13 && !event.shiftKey) {
            event.preventDefault();

            var channel = this.activeChannel;
            if (!channel) return;
            var server = channel.server;
            var service = server.service;

            var msg = this.message.val()[0];
            this.message.val('');
            service.sendMessage(msg, server, channel);
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

        if (!json.protocol) {
            if (json.cmd == "login" && json.success)
                this.loginSuccess = true;
        } else {
            // service specific message
            var service = this.get(json.protocol);
            if (service)
                service.handleCommand(json);
            else
                console.log("Service missing: "+json.protocol);
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
