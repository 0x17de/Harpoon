class IrcUtils {
    static stripName(userName) {
        var exclamationPosition = userName.indexOf('!');
        return exclamationPosition >= 0 ? userName.substr(0, exclamationPosition) : userName;
    }
    static formatTime(time) {
        var date = new Date(time);
        return '[' + this.to2digits(date.getHours()) +':'+ this.to2digits(date.getMinutes()) +':'+ this.to2digits(date.getSeconds()) + ']';
    }
    static to2digits(number) {
        return number > 9 ? ''+number : '0'+number;
    }
}

class IrcUser extends UserBase {
    name() {
        if (!this.details.shortName)
            this.details.shortName = IrcUtils.stripName(this.details.name);
        return this.details.shortName;
    }
}

class IrcChannel extends ChannelBase {
    constructor(name, server) {
        super(name, server);
        this.backlogSplitHandle = q('<div>');
        q('#backlog-handles').add(this.backlogSplitHandle);
    }
    get(userName) {
        return this.users[IrcUtils.stripName(userName)];
    }
    addMessage(time, nick, msg) {
        var line = q('<div>');
        var time = q('<div>').text(time).addClass('backlog-time');
        var msg = q('<div>').text(msg).addClass('backlog-message');
        var nick = q('<div>').text(nick).addClass('backlog-nick');
        line.add(time).add(nick).add(msg);
        this.backlog.add(line);
    }
}

class IrcServer extends ServerBase {
    constructor(id, name, service) {
        super(name, service);
        this.id = id;
    }
    loadChannels(channels) {
        for (var channelName in channels) {
            var channelData = channels[channelName];
            var channel = new IrcChannel(channelName, this);

            var users = channelData.users;
            for (var userName in users)
                new IrcUser({name:userName}, channel);
        }
        return this;
    }
}

class IrcService extends ServiceBase {
    constructor(chat) {
        super('irc', chat);
        this.serverIdNameMap = {};
        this.activeNick = 'iirc';
    }
    loadSettings(settings) {
        this.settings = settings;
    }
    getById(id) {
        return this.servers[this.serverIdNameMap[id]];
    }
    sendMessage(msg, server, channel) {
        this.chat.send({
            cmd:'chat',
            server:server.id,
            channel:channel.name,
            msg:msg
        });
    }
    highlight(channel, message) {
        this.chat.highlight(channel, 'message');
        if (message.indexOf(this.activeNick) >= 0)
            this.chat.highlight(channel, 'highlight');
    }

    handleCommand(json) {
        switch(json.cmd) {
        case 'join':
            this.handleJoin(json); break;
        case 'part':
            this.handlePart(json); break;
        case 'chat':
            this.handleChat(json); break;
        case 'action':
            this.handleAction(json); break;
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
            var channel = this.getById(json.server).get(json.channel);
            if (!channel) return console.log('Channel not created: '+json.server+' '+json.channel);
            new IrcUser({name:json.nick}, channel);
            channel.addMessage(IrcUtils.formatTime(json.time), '-->', IrcUtils.stripName(json.nick) + ' has joined ' + channel.name);
        }
    }
    handlePart(json) {
        var channel = this.getById(json.server).get(json.channel);
        if (!channel) return console.log('Channel not created: '+json.server+' '+json.channel);
        if (json.nick.length === 0) {
            channel.remove();
        } else {
            var user = channel.get(json.nick);
            if (!user) return;
            user.remove();
            channel.addMessage(IrcUtils.formatTime(json.time), '<--', IrcUtils.stripName(json.nick) + ' has left ' + channel.name);
        }
    }
    handleAction(json) {
        var channel = this.getById(json.server).get(json.channel);
        if (!channel) return console.log('Channel not created: '+json.server+' '+json.channel);
        channel.addMessage(IrcUtils.formatTime(json.time), '*', IrcUtils.stripName(json.nick) + ' ' + json.msg);
        this.highlight(channel, json.msg);
    }
    handleChat(json) {
        var channel = this.getById(json.server).get(json.channel);
        if (!channel) return console.log('Channel not created: '+json.server+' '+json.channel);
        channel.addMessage(IrcUtils.formatTime(json.time), '<'+IrcUtils.stripName(json.nick)+'>', json.msg);
        this.highlight(channel, json.msg);
    }
    handleSettings(json) {
        this.loadSettings(json.data);
    }
    handleChatList(json) {
        var servers = json.servers;
        for (var i in servers) {
            this.serverIdNameMap[i] = servers[i].name;
            new IrcServer(i, servers[i].name, this)
                .loadChannels(servers[i].channels);
        }
    }
}
