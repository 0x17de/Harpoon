function Service(servicename, servicetype) {
    this.serviceview = new Element('#serviceconfig-'+servicetype);
    this.servicetype = servicetype;
    this.selector = new Element('option').text(servicename);
    this.selector.get().onclick = this.show;
    this.selector.get().value = servicetype;
    serviceSelector.add(this.selector);
}
Service.map = {};
Service.active = null;
Service.select = function(e) {
    Service.map[e.value].show();
};
Service.add = function(service) {
    Service.map[service.servicetype] = service;
};
Service.prototype.show = function() {
    if (Service.active)
        Service.active.hide();
    this.serviceview.show();
};
Service.prototype.hide = function() {
    this.serviceview.hide();
};

function ServiceIrc() {
    Service.call(this, 'IRC', 'irc');
    this.networklist = new Element('#serviceconfig-irc-networklist');
    this.hostlist = new Element('#serviceconfig-irc-hostlist');
    this.nicklist = new Element('#serviceconfig-irc-nicklist');
    this.clear();
}
ServiceIrc.prototype = Object.create(Service.prototype);
ServiceIrc.prototype.constructor = ServiceIrc;
ServiceIrc.prototype.clear = function() {
    this.networklist.clear();
    this.hostlist.clear();
    this.nicklist.clear();
};
ServiceIrc.selectedServer = null;
ServiceIrc.prototype.load = function(json) {
    this.clear();
    this.data = json;

    var noneSelected = true;
    var servers = json.servers;
    for (var serverId in servers) {
        var serverData = servers[serverId];
        var server = this.addServer(serverId, serverData);
        if (noneSelected) {
            noneSelected = false;
            server.get().click();
        }
    }
};
ServiceIrc.prototype.setServerData = function(serverData) {
    ServiceIrc.selectedServerData = serverData;
    ServiceIrc.selectedHost = null;
    this.hostlist.clear();

    var noneSelected = true;
    var hosts = serverData.hosts;
    for (var hostKey in hosts) {
        var host = this.addHost(hostKey, hosts[hostKey]);
        if (noneSelected) {
            noneSelected = false;
            host.get().click();
        }
    }
};
ServiceIrc.prototype.addHost = function(hostKey, hostData) {
    var self = this;
    var host = new Element('div');

    host.get().onclick = function() {
        if (ServiceIrc.selectedHost)
            ServiceIrc.selectedHost.removeClass('selected');
        ServiceIrc.selectedHostData = hostData;
        host.class('selected');
        ServiceIrc.selectedHost = host;
    };
    host.text(hostKey);
    this.hostlist.add(host);

    return host;
};
ServiceIrc.prototype.addServer = function(serverId, serverData) {
    var self = this;
    var server = new Element('div');

    server.attr('data-id', serverId);
    server.get().onclick = function() {
        if (ServiceIrc.selectedServer)
            ServiceIrc.selectedServer.removeClass('selected');
        self.setServerData(serverData);
        server.class('selected');
        ServiceIrc.selectedServer = server;
    };
    server.text(serverData.name);
    this.networklist.add(server);

    var nicks = serverData.nicks;
    for (var i = 0; i < nicks.length; ++i)
        this.addNick(nicks[i]);

    return server;
};
ServiceIrc.prototype.addNick = function(nick) {
    var nickPad = new Element('div');
    var nickEntry = new Element('input');
    nickPad.add(nickEntry);
    nickEntry.val(nick);
    this.nicklist.add(nickPad);
};
ServiceIrc.prototype.save = function() {
    // TODO save
};

