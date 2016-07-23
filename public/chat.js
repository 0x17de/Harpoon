var username = "user";
var password = "password";
var ws, ping;

var activeNick = 'iirc';

var input,
    backlog,
    logscroll,
    connected,
    serverList,
    userlist,
    channeltitle,
    tryingToConnect = false,
    pageTransparent,
    pageTransparentPopup,
    pageChat,
    serviceSelector;

function sendInput() {
    if (!connected) return;
    var input = document.getElementById('input');
    sendMessage(input.value);
    input.value = '';
}
function parseMessageCommand(msg) {
    var splitMsg = msg.split(' ', 2);
    var cmd = splitMsg[0];
    if (Channel.active && Channel.active.type === 'irc') {
        if (cmd === 'join') {
            var loginData = splitMsg[1].split(' ');
            send({type:'irc', cmd:'join', serverId:Channel.active.serverId, channel:loginData[0], password:loginData[1]});
        } else  if (cmd === 'part') {
            var channel = splitMsg[1] || Channel.active.channelName;
            send({type:'irc', cmd:'part', serverId:Channel.active.serverId, channel:channel});
            var channelPage = serverList.get('irc', Channel.active.serverId).get(channel);
            channelPage.unlink();
            channelPage.remove();
        } else if (cmd === 'nick') {
            send({type:'irc', cmd:'nick', serverId:Channel.active.serverId, nick:splitMsg[1]});
        } else if (cmd === 'me') {
            send({type:'irc', cmd:'me', serverId:Channel.active.serverId, channel:Channel.active.channelName, nick:splitMsg[1]});
        }
    }
}
function sendMessage(msg) {
    if (msg.length === 0) return;
    if (msg[0] === '/') {
        if (msg[1] === '/') {
            msg = msg.substr(1);
        } else {
            parseMessageCommand(msg.substr(1));
            return;
        }
    }
    if (!Channel.active) return;
    if (Channel.active.bServerChannel) return;
    send({cmd:"chat", server:Channel.active.serverId, channel:Channel.active.channelName, msg:msg});
}
function sendPing() {
    send({cmd:'ping'});
}
function send(json) {
    console.log(">> "+JSON.stringify(json));
    ws.send(JSON.stringify(json));
}
function startChat() {
    if (!tryingToConnect)
        putLog('', null, null, timestamp(), "--", "Trying to connect ...", 'event');
    tryingToConnect = true;

    var proto = document.location.protocol.replace(/^http/, "ws");
    ws = new WebSocket(proto+"//"+document.domain+":8080/ws");
    if (ping) clearInterval(ping);
    ping = setInterval(sendPing, 60000);
    ws.onopen = function() {
        putLog('', null, null, timestamp(), "--", "Connection established", 'event');
        connected = true;
        ws.send("LOGIN "+username+" "+password+"\n");
    };
    ws.onclose = function() {
        clearInterval(ping);
        ping = void 0;
        if (connected)
            putLog('', null, null, timestamp(), "--", "Connection lost", 'event');
        tryingToConnect = false;
        connected = false;
        setTimeout(startChat, 1000);
    };
    ws.onmessage = function(msg) {
        if (msg.data instanceof Blob
         || msg.data instanceof ArrayBuffer) {
            var reader = new FileReader();
            reader.onload = function(e) {
                onMessage(e.target.result);
            };
            reader.readAsText(msg.data);
        } else {
            onMessage(msg.data);
        }
    };
}
function getScroll(evt, cb) {
    if (!evt) evt = event;
    var direction = (evt.defaul < 0 || evt.whellDelta > 0) ? 1 : -1;
    return direction;
}
function onMessage(msg) {
    if (msg === '') return;
    try {
        var json = JSON.parse(msg);
        if (json.type === void 0 || json.type === 'irc')
            onIrcMessage(json);
    } catch(e) {
        console.log("Failed to parse >>" + msg + "<<. Reason: "+e);
    }
}
function twoDigit(e) {
    return e < 10 ? '0'+e : ''+e;
}
function timestamp(opt_ts) {
    var d = opt_ts ? new Date(opt_ts) : new Date();
    return twoDigit(d.getHours()) + ":" + twoDigit(d.getMinutes()) + ":" + twoDigit(d.getSeconds());
}
function onIrcMessage(json) {
    var target;
    if ((json.server || json.serverId) && json.channel) {
        var server = serverList.get('irc', json.server || json.serverId);
        if (server)
            target = server.get(json.channel);
    }

    var pureNick, nick;
    if (json.nick !== void 0) {
        pureNick = json.nick.indexOf('!') == -1 ? json.nick : json.nick.substr(0, json.nick.indexOf('!'));
        var nick = pureNick && '<'+pureNick+'>';
    }
    switch (json.cmd) {
    case 'serveradded':
        var ircService = Service.map['irc'];
        ircService.data.servers[json.serverId] = {name: json.name};
        ircService.load();
        ServiceIrc.addServerPopup_Close(true);
        break;
    case 'hostadded':
        var hostKey = json.host + ":" + json.port;
        var ircService = Service.map['irc'];
        ircService.data.servers[json.serverId].hosts[hostKey] = {
            hasPassword: json.hasPassword,
            ipv6: json.ipv6,
            ssl: json.ssl
        };
        ircService.load();
        ServiceIrc.addHostPopup_Close(true);
        break;
    case 'login':
        console.log('Login successful');
        send({cmd:'querysettings'});
        break;
    case 'settings':
        var service =  Service.map[json.type];
        if (!service) break;
        service.load(json.data);
        break;
    case 'userlist':
        target.clearUsers();
        var users = json.users;
        for (var i = 0; i < users.length; ++i)
            target.addUser(users[i]);
        break;
    case 'chatlist':
        var chooseFirstChannel = true;
        serverList.clear('irc');
        var servers = json.servers;
        for(var serverId in servers) {
            var serverData = servers[serverId];
            var server = serverList.add('irc', serverId, serverData.name);
            var channels = serverData.channels;
            for (var channelName in channels) {
                var channelData = channels[channelName];
                var channel = server.add(channelName);
                if (chooseFirstChannel) {
                    channel.link(); // set new channel to active
                    chooseFirstChannel = false;
                }
                var users = channelData.users;
                for (userName in users) {
                    var userData = users[userName];
                    channel.addUser(userName, userData);
                }
            }
        }
        break;
    case 'join':
        if (json.nick === '') {
            var server = serverList.get('irc', json.server);
            server.add(json.channel);
        } else {
            putLog(json.type, json.server, json.channel, timestamp(json.time), '-->', pureNick+' joined the channel', 'event');
            target.addUser(pureNick);
        }
        break;
    case 'part':
        if (json.nick === '') {
            var server = serverList.get('irc', json.server);
            // TODO: remove channel
        } else {
            putLog(json.type, json.server, json.channel, timestamp(json.time), '<--', pureNick+' left the channel', 'event');
            target.removeUser(pureNick);
        }
        break;
    case 'quit':
        var channels = serverList.get('irc', json.server).channels;
        for (var channelName in channels) {
            if (channels[channelName].removeUser(pureNick))
                putLog(json.type, json.server, channelName, timestamp(json.time), '<--', pureNick+' has quit'+(json.msg?' ('+json.msg+')':''), 'event');
        }
        break;
    case 'chat':
        putLog(json.type, json.server, json.channel, timestamp(json.time), nick, json.msg);
        break;
    case 'action':
        putLog(json.type, json.server, json.channel, timestamp(json.time), '*', pureNick+' '+json.msg, 'action');
        break;
    case 'kick':
        putLog(json.type, json.server, json.channel, timestamp(json.time), '<--', pureNick+' was kicked', 'event');
        target.removeUser(pureNick);
        break;
    case 'nickchange':
        var channels = serverList.get('irc', json.server).channels;
        for (var channelName in channels) {
            if (channels[channelName].renameUser(pureNick, json.newNick))
                putLog(json.type, json.server, channelName, timestamp(json.time), '<->', pureNick+' is now known as '+json.newNick, 'event');
        }
        break;
    case 'notice':
        putLog(json.type, json.server, json.channel, timestamp(json.time), '*'+pureNick+'*', json.msg, 'notice');
        break;
    default:
        console.log("WARNING: Unknown command: "+json.cmd);
    }
}
function putLog(type, serverId, channelName, time, nick, msg, style) {
    var channel;

    if (type === '') {
        channel = serverList.log;
    } else {
        if (channelName === null) {
            channel = serverList.get(type, serverId).getServerLog();
        } else {
            channel = serverList.get(type, serverId).get(channelName);
        }
    }
    if (!channel) return;

    var doScroll = logscroll.scrollHeight - logscroll.clientHeight <= logscroll.scrollTop + 10 /*tolerance*/;
    var row;
    channel.backlogRoot.add(
      row = (new Element("div")).class('row')
        .add(etime=(new Element("div").text(time).class('time')))
        .add(enick=(new Element("div").text(nick).class('nick')))
        .add(emsg=(new Element("div").text(msg).class('msg')))
    );

    if (style) row.class(style);
    if (doScroll) logscroll.scrollTop = logscroll.scrollHeight - logscroll.clientHeight;
}
function putLine(msg) {
    backlog.add(
      (new Element("div")).text(msg)
    );
}
function init() {
    input = new Element('#input');
    bar = new Element('#channellist');
    backlog = new Element('#backlog');
    userlist = new Element('#userlist');
    channeltitle = new Element('#channeltitle');
    pageChat = new Element('#page-chat');
    pageTransparent = new Element('#page-transparent');
    pageTransparentPopup = new Element('#page-transparent-popup');
    serviceSelector = new Element('#service-selector');

    // TODO: Server tells active services
    Service.add(new ServiceIrc());
    // select first service
    Service.select(serviceSelector.get());

    serverList = new ServerList(bar);

    logscroll = backlog.get().parentNode.parentNode;
    input.get().onkeydown = function(e) {
        if (e.keyCode == 13 && !e.shiftKey) {
            e.preventDefault();
            var msg = input.val('');
            sendMessage(msg);
        }
    };
    input.get().focus();
    startChat();
}
