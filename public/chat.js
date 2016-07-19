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
    var input = document.getElementById('input');
    sendMessage(input.value);
    input.value = '';
}
function sendMessage(msg) {
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
    var json = JSON.parse(msg);
    if (!json) {
        console.log("!<< "+msg);
        return;
    }
    console.log("<< "+JSON.stringify(json));
    if (json.type === void 0 || json.type === 'irc')
        onIrcMessage(json);
}
function twoDigit(e) {
    return e < 10 ? '0'+e : ''+e;
}
function timestamp() {
    var d = new Date();
    return twoDigit(d.getHours()) + ":" + twoDigit(d.getMinutes()) + ":" + twoDigit(d.getSeconds());
}
function onIrcMessage(json) {
    var target;
    if (json.server && json.channel)
        target = serverList.get('irc', json.server).get(json.channel);

    var pureNick, nick;
    if (json.nick !== void 0) {
        pureNick = (json.nick === '')
            ? activeNick
            : json.nick.substr(0, json.nick.indexOf('!'));
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
        putLog(json.type, json.server, json.channel, timestamp(), '-->', pureNick+' joined the channel', 'event');
        target.addUser(pureNick);
        break;
    case 'part':
        putLog(json.type, json.server, json.channel, timestamp(), '<--', pureNick+' left the channel', 'event');
        target.removeUser(pureNick);
        break;
    case 'quit':
        var channels = serverList.get('irc', json.server).channels;
        for (var channelName in channels) {
            if (channels[channelName].removeUser(pureNick))
                putLog(json.type, json.server, channelName, timestamp(), '<--', pureNick+' has quit'+(json.msg?' ('+json.msg+')':''), 'event');
        }
        break;
    case 'chat':
        putLog(json.type, json.server, json.channel, timestamp(), nick, json.msg);
        break;
    case 'action':
        putLog(json.type, json.server, json.channel, timestamp(), '*', pureNick+' '+json.msg, 'action');
        break;
    case 'kick':
        putLog(json.type, json.server, json.channel, timestamp(), '<--', pureNick+' was kicked', 'event');
        target.removeUser(pureNick);
        break;
    case 'nickchange':
        var channels = serverList.get('irc', json.server).channels;
        for (var channelName in channels) {
            if (channels[channelName].renameUser(pureNick, json.newNick))
                putLog(json.type, json.server, channelName, timestamp(), '<->', pureNick+' is now known as '+json.newNick, 'event');
        }
        break;
    case 'notice':
        putLog(json.type, json.server, json.channel, timestamp(), '*'+pureNick+'*', json.msg, 'notice');
        break;
    default:
        console.warning("Unknown command: "+json.cmd);
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
