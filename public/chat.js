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
		putLine("Connected");
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
	//var json = JSON.parse(msg);
	putLine(msg);
}
function putLine(msg) {
	var text = document.createTextNode(msg);
	var p = document.createElement("div");
	p.appendChild(text);
	log.appendChild(p);
}
function init() {
	input = document.getElementById('input');
	log = document.getElementById('log');
	input.onkeydown = function(e) {
		if (e.keyCode == 13 && !e.shiftKey) {
			e.preventDefault();
			var msg = input.value;
			input.value = '';
			sendMessage(msg);
			putLine(msg);
		}
	};
	input.focus();
	startChat();
}
