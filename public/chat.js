var username = "testuser";
var password = "testpassword";
var ws, ping;

var activeNick = 'iirc';
var activeServer = 1;
var activeChannel = '#test';

var input, log, logscroll;

function sendInput() {
	var input = document.getElementById('input');
	sendMessage(input.value);
	input.value = '';
}
function sendMessage(msg) {
	send({cmd:"chat", server:activeServer, channel:activeChannel, msg:msg});
}
function sendPing() {
	send({cmd:'ping'});
}
function send(json) {
	ws.send(JSON.stringify(json));
}
function startChat() {
	var proto = document.location.protocol.replace(/^http/, "ws");
	ws = new WebSocket(proto+"//"+document.domain+":8080/ws");
	if (ping) clearInterval(ping);
	ping = setInterval(sendPing, 60000);
	ws.onopen = function() {
		putLog(timestamp(), "--", "Connected", 'event');
		ws.send("LOGIN "+username+" "+password+"\n");
	};
	ws.onclose = function() {
		clearInterval(ping);
		ping = void 0;
		putLine("Disconnected");
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
function onMessage(msg) {
	var json = JSON.parse(msg);
	if (!json) {
		console.log(msg);
		return;
	}
	if (json.type == 'irc')
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
	console.log(json.nick);
	var pureNick, nick;
	if (json.nick !== void 0) {
		pureNick = (json.nick === '')
			? activeNick
			: json.nick.substr(0, json.nick.indexOf('!'));
		var nick = pureNick && '<'+pureNick+'>';
	}
	switch (json.cmd) {
	case 'join':
		putLog(timestamp(), '-->', pureNick, 'event');
		break;
	case 'part':
		putLog(timestamp(), '<--', pureNick, 'event');
		break;
	case 'quit':
		putLog(timestamp(), '<--', pureNick, 'event');
		break;
	case 'chat':
		putLog(timestamp(), nick, json.msg);
		break;
	case 'nickchange':
		putLog(timestamp(), '<->', pureNick+' is now known as '+json.newNick, 'event');
		break;
	default:
		console.warning("Unknown command: "+json.cmd);
	}
}
function putLog(time, nick, msg, style) {
	var doScroll = logscroll.scrollHeight - logscroll.clientHeight <= logscroll.scrollTop + 5 /*tolerance*/;
	var row;
	log.add(
	  row = (new Element("div")).class('row')
	    .add(etime=(new Element("div").text(time).class('time')))
	    .add(enick=(new Element("div").text(nick).class('nick')))
	    .add(emsg=(new Element("div").text(msg).class('msg')))
	);
	if (style) row.class(style);
	if (doScroll) log.get().scrollIntoView(false);
}
function putLine(msg) {
	log.add(
	  (new Element("div")).text(msg)
	);
}
function init() {
	input = new Element('#input');
	log = new Element('#log');
	logscroll = log.get().parentNode.parentNode;
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
