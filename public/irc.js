class IrcUser extends UserBase {
}

class IrcChannel extends ChannelBase {
    constructor(name, server) {
        super(name, server);
        this.backlogSplitHandle = q('<div>');
        q('#backlog-handles').add(this.backlogSplitHandle);
    }
    addMessage(time, nick, msg) {
        var line = q('<div>');
        var time = q('<div>').text(time).addClass('backlog-time');
        var msg = q('<div>').text(msg).addClass('backlog-message');
        var nick = q('<div>').text(nick).addClass('backlog-nick');
        line.add(time).add(nick).add(msg);
        this.backlog.add(line);
    }
    test() {
        this.userlist.add(q('<div>').text('User1'));
        this.addMessage('[22:03:45]', 'nick1', 'message1');
        this.addMessage('[22:03:45]', 'nick2', 'message2');
        this.addMessage('[22:03:45]', 'nick3', 'message3isverylongmessage3isverylongmessage3isverylongmessage3isverylongmessage3isverylongmessage3isverylongmessage3isverylongmessage3isverylongmessage3isverylongmessage3isverylong');
        this.addMessage('[22:03:45]', 'nick4', 'message4 is very long message4 is very long message4 is very long message4 is very long message4 is very long message4 is very long');
        this.addMessage('[22:03:45]', 'nick5', 'message5');
        this.addMessage('[22:03:45]', 'nick', 'message');
        this.addMessage('[22:03:45]', 'nick', 'message');
        this.addMessage('[22:03:45]', 'nick', 'message');
        this.addMessage('[22:03:45]', 'nick', 'message');
        this.addMessage('[22:03:45]', 'nick', 'message');
        this.addMessage('[22:03:45]', 'nick', 'message');
        this.addMessage('[22:03:45]', 'nick', 'message');
        this.addMessage('[22:03:45]', 'nick', 'message');
        this.addMessage('[22:03:45]', 'nick', 'message');
        this.addMessage('[22:03:45]', 'nick', 'message');
        this.addMessage('[22:03:45]', 'nick', 'message');
        this.addMessage('[22:03:45]', 'nick', 'message');
        this.addMessage('[22:03:45]', 'nick', 'message');
        this.addMessage('[22:03:45]', 'nick', 'message');
        this.addMessage('[22:03:45]', 'nick', 'message');
        this.addMessage('[22:03:45]', 'nick', 'message');
        this.addMessage('[22:03:45]', 'nick', 'message');
        this.addMessage('[22:03:45]', 'nick', 'message');
        this.addMessage('[22:03:45]', 'nick', 'message');
    }
}

class IrcServer extends ServerBase {
    loadChannels(channels) {
        for (var channelName in channels) {
            var channelData = channels[channelName];
            new IrcChannel(channelName, this);
        }
        return this;
    }
}

class IrcService extends ServiceBase {
    constructor(chat) {
        super('irc', chat);
        this.serverIdNameMap = {};
    }
    loadSettings(settings) {
        this.settings = settings;
    }
    getById(id) {
        return this.servers[this.serverIdNameMap[id]];
    }

    handleCommand(json) {
        switch(json.cmd) {
        case 'join':
            this.handleJoin(json); break;
        case 'part':
            this.handlePart(json); break;
        case 'message':
            this.handleChat(json); break;
        case 'settings':
            this.handleSettings(json); break;
        case 'chatlist':
            this.handleChatList(json); break;
        }
    }

    handleJoin(json) {
        if (json.nick.length === 0) {
            new IrcChannel(json.channel, this.getById(json.server));
        } else {
            new IrcUser({name:json.nick}, this.getById(json.server).get(json.channel));
        }
    }
    handlePart(json) {
    }
    handleChat(json) {
    }
    handleSettings(json) {
        this.loadSettings(json.data);
    }
    handleChatList(json) {
        var servers = json.servers;
        for (var i in servers) {
            this.serverIdNameMap[i] = servers[i].name;
            new IrcServer(servers[i].name, this)
                .loadChannels(servers[i].channels);
        }
    }
}
