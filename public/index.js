var layout
  , chat;

function init() {
    layout = new Layout();
    chat = new Chat();

    var service = new ServiceBase('irc', chat);
    var server1 = new ServerBase('TestServer', service);
    var server2 = new ServerBase('TestServer', service);
    var channel1 = new ChannelBase('#test1', server1);
    var channel2 = new ChannelBase('#test2', server1);
    var channel3 = new ChannelBase('#test3', server2);
    channel1.activate();
    channel1.test();
}
