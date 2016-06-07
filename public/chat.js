var username = "testuser";
var password = "testpassword";
var ws;

var activeServer = 1;
var activeChannel = '#test';

var input, log;

function sendInput() {
	var input = document.getElementById('input');
	sendMessage(input.value);
	input.value = '';
}
function sendMessage(msg) {
	send({cmd:"chat", server:activeServer, channel:activeChannel, msg:msg});
}
function send(json) {
	ws.send(JSON.stringify(json));
}
function startChat() {
	var proto = document.location.protocol.replace(/^http/, "ws");
	ws = new WebSocket(proto+"//"+document.domain+":8080/ws");
	ws.onopen = function() {
		putLog(timestamp(), "--", "Connected", 'event');
		ws.send("LOGIN "+username+" "+password+"\n");
	};
	ws.onclose = function() {
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
	var pureNick = json.nick && json.nick.substr(0, json.nick.indexOf('!'));
	var nick = pureNick && '<'+pureNick+'>';
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
	}
}
function putLog(time, nick, msg, style) {
	var row;
	log.add(
	  row = (new Element("div")).class('row')
	    .add(etime=(new Element("div").text(time).class('time')))
	    .add(enick=(new Element("div").text(nick).class('nick')))
	    .add(emsg=(new Element("div").text(msg).class('msg')))
	);
	if (style) row.class(style);
}
function putLine(msg) {
	log.add(
	  (new Element("div")).text(msg)
	);
}
function init() {
	input = new Element('#input');
	log = new Element('#log');
	input.get().onkeydown = function(e) {
		if (e.keyCode == 13 && !e.shiftKey) {
			e.preventDefault();
			var msg = input.val('');
			sendMessage(msg);
			putLog(timestamp(), '<iirc>', msg);
		}
	};
	input.get().focus();
	startChat();
}
