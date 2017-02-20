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
        this.largestId = -1;
        this.smallestId = Number.MAX_VALUE;
    }
    get(userName) {
        return this.users[IrcUtils.stripName(userName)];
    }
    addMessage(id, time, nick, msg) {
        var nid = Number(id);
        var line = q('<div>').attr('id', 'message-'+id);
        var time = q('<div>').text(time).addClass('backlog-time');
        var msg = q('<div>').text(msg).addClass('backlog-message');
        var nick = q('<div>').text(nick).addClass('backlog-nick');
        line.add(time).add(nick).add(msg);

        if (nid > this.largestId) {
            this.backlog.add(line);
            this.largestId = nid;
        } else if (nid < this.smallestId) {
            this.backlog.insertBefore(line, this.backlog.firstChild());
            this.smallestId = nid;
        }
        if (this.scrollPosition === 'bottom') {
            this.server.service.chat.doScroll();
        }
        if (nid < this.smallestId)
            this.smallestId = nid;
    }
    requestBacklog() {
        var request = {
            protocol:'irc',
            cmd:'requestbacklog',
            server:this.server.id,
            channel:this.name,
        }
        this.server.service.chat.send(request);
    }
}

class IrcServer extends ServerBase {
    constructor(id, activeNick, name, service) {
        super(name, service);
        this.activeNick = activeNick;
        this.id = id;
    }
    loadChannels(channels) {
        for (var channelName in channels) {
            var channelData = channels[channelName];
            var channel = new IrcChannel(channelName, this);
            if (channelData.disabled)
                channel.setDisabled(channelData.disabled);

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
    }
    loadSettings(settings) {
        this.settings = settings;
    }
    getById(id) {
        return this.servers[this.serverIdNameMap[id]];
    }
    sendMessage(msg, server, channel) {
        if (msg.indexOf('/') === 0) {
            var split = msg.substr(1).split(' ');
            var cmd = split[0];

            switch(cmd) {
            case 'join':
                if (split.length >= 2) {
                    if (!split[2]) split[2] = '';
                    this.chat.send({
                        protocol:'irc',
                        cmd:'join',
                        server:server.id,
                        channel:split[1],
                        password:split[2]
                    });
                }
                break;
            case 'part':
                var partChannel = split[1] || this.chat.activeChannel.name;
                this.chat.send({
                    protocol:'irc',
                    cmd:'part',
                    server:server.id,
                    channel:partChannel
                });
                break;
            case 'nick':
                if (split[1] != '') {
                    this.chat.send({
                        protocol:'irc',
                        cmd:'nick',
                        server:server.id,
                        channel:channel.name,
                        nick:split[1]
                    });
                }
                break;
            case 'me':
                var message = msg.substr(msg.indexOf(' ')+1);
                if (message != '') {
                    this.chat.send({
                        protocol:'irc',
                        cmd:'action',
                        server:server.id,
                        channel:channel.name,
                        msg:message
                    });
                }
                break;
            }
        } else {
            this.chat.send({
                cmd:'chat',
                protocol:'irc',
                server:server.id,
                channel:channel.name,
                msg:msg
            });
        }
    }
    highlight(channel, message) {
        this.chat.highlight(channel, 'message');
        if (message.indexOf(channel.server.activeNick) >= 0)
            this.chat.highlight(channel, 'highlight');
    }

    handleCommand(json) {
        switch(json.cmd) {
        case 'join':
            this.handleJoin(json); break;
        case 'part':
            this.handlePart(json); break;
        case 'quit':
            this.handleQuit(json); break;
        case 'chat':
            this.handleChat(json); break;
        case 'topic':
            this.handleTopic(json); break;
        case 'action':
            this.handleAction(json); break;
        case 'settings':
            this.handleSettings(json); break;
        case 'chatlist':
            this.handleChatList(json); break;
        case 'nickchange':
            this.handleNickChange(json); break;
        case 'backlogresponse':
            this.handleBacklogResponse(json); break;
        }
    }

    handleBacklogResponse(json) {
        var channel = this.getById(json.server).get(json.channel);
        if (!channel) return console.log('Channel not created: '+json.server+' '+json.channel);

        for (var i = 0; i < json.lines.length; ++i) {
            var line = json.lines[i];
            var nick, msg;

            switch(line.type) {
            case "msg":
            case "notice":
            case "action":
                nick = '<'+IrcUtils.stripName(line.sender)+'>';
                msg = line.msg;
                break;
            case "topic":
                nick = '*';
                msg = IrcUtils.stripName(line.sender) + ' changed the topic: ' + json.topic;
                break;
            case "join":
                nick = '-->';
                msg = IrcUtils.stripName(line.sender) + ' has joined ' + channel.name;
                break;
            case "part":
                nick = '<--';
                msg = IrcUtils.stripName(line.sender) + ' has left ' + channel.name;
                break;
            case "kick":
                nick = '*';
                msg = IrcUtils.stripName(line.sender) + ' was kicked ' + line.msg;
                break;
            default:
                nick = '*';
                msg = line.msg;
                break;
            }
            channel.addMessage(line.id, IrcUtils.formatTime(line.time), nick, msg);
        }
    }
    handleTopic(json) {
        var channel = this.getById(json.server).get(json.channel);
        if (!channel) return console.log('Channel not created: '+json.server+' '+json.channel);
        channel.addMessage(json.id, IrcUtils.formatTime(json.time), '*', IrcUtils.stripName(json.nick) + ' changed the topic: ' + json.topic);
    }
    handleNickChange(json) {
        var server = this.getById(json.server);
        if (IrcUtils.stripName(json.nick) === server.activeNick) {
            server.activeNick = json.newNick;
        }
        for (var channelName in server.channels) {
            var channel = server.get(channelName);
            var user = channel.get(json.nick);
            if (!user) continue;
            user.rename(json.newNick);
            channel.addMessage(json.id, IrcUtils.formatTime(json.time), '<->', IrcUtils.stripName(json.nick) + ' is now known as ' + json.newNick);
        }
    }
    handleJoin(json) {
        if (json.nick.length === 0) {
            new IrcChannel(json.channel, this.getById(json.server)).activate();
        } else {
            var channel = this.getById(json.server).get(json.channel);
            if (!channel) return console.log('Channel not created: '+json.server+' '+json.channel);
            channel.setDisabled(false);
            new IrcUser({name:json.nick}, channel);
            channel.addMessage(json.id, IrcUtils.formatTime(json.time), '-->', IrcUtils.stripName(json.nick) + ' has joined ' + channel.name);
        }
    }
    handlePart(json) {
        var channel = this.getById(json.server).get(json.channel);
        if (!channel) return console.log('Channel not created: '+json.server+' '+json.channel);
        if (IrcUtils.stripName(json.nick) === channel.server.activeNick) {
            channel.setDisabled(true);
        } else {
            var user = channel.get(json.nick);
            if (!user) return;
            user.remove();
            channel.addMessage(json.id, IrcUtils.formatTime(json.time), '<--', IrcUtils.stripName(json.nick) + ' has left ' + channel.name);
        }
    }
    handleQuit(json) {
        var server = this.getById(json.server);
        if (IrcUtils.stripName(json.nick) === server.activeNick) {
            // TODO: handle quit
        } else {
            for (var channelName in server.channels) {
                var channel = server.get(channelName);
                var user = channel.get(json.nick);
                if (!user) continue;
                user.remove();
                channel.addMessage(json.id, IrcUtils.formatTime(json.time), '<--', IrcUtils.stripName(json.nick) + ' has quit ' + channel.name);
            }
        }
    }
    handleAction(json) {
        var channel = this.getById(json.server).get(json.channel);
        if (!channel) return console.log('Channel not created: '+json.server+' '+json.channel);
        channel.addMessage(json.id, IrcUtils.formatTime(json.time), '*', IrcUtils.stripName(json.nick) + ' ' + json.msg);
        this.highlight(channel, json.msg);
    }
    handleChat(json) {
        var channel = this.getById(json.server).get(json.channel);
        if (!channel) return console.log('Channel not created: '+json.server+' '+json.channel);
        channel.addMessage(json.id, IrcUtils.formatTime(json.time), '<'+IrcUtils.stripName(json.nick)+'>', json.msg);
        this.highlight(channel, json.msg);
    }
    handleSettings(json) {
        this.loadSettings(json.data);
    }
    handleChatList(json) {
        var servers = json.servers;
        for (var i in servers) {
            var server = servers[i];
            this.serverIdNameMap[i] = server.name;
            new IrcServer(i, server.nick, server.name, this)
                .loadChannels(servers[i].channels);
        }
    }
}
