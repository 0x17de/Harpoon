var layout
  , chat;

function init() {
    layout = new Layout();
    chat = new Chat();

    var service = new ServiceBase('irc', chat);
    var server = new ServerBase('TestServer', service);
    var channel = gc = new IrcChannel('#test', server);
    channel.activate();
    channel.test();
}
