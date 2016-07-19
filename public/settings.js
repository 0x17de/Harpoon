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
ServiceIrc.addServerPopup_Close = function(force) {
    if (force) disableClosePopup = false;
    closepopup((new Element('#popup-irc-add-server')).get());
};
ServiceIrc.addHostPopup_Close = function(force) {
    if (force) disableClosePopup = false;
    closepopup((new Element('#popup-irc-add-host')).get());
};
ServiceIrc.addServerPopup_Open = function() {
    showpopup('irc-add-server');
    (new Element('#serviceconfig-irc-newserver-btnsave')).removeAttr('disabled');
    (new Element('#serviceconfig-irc-newserver-btnclose')).removeAttr('disabled');
    (new Element('#serviceconfig-irc-newserver-name')).val('');
};
ServiceIrc.addHostPopup_Open = function() {
    if (ServiceIrc.selectedServerId == null) return;
    showpopup('irc-add-host');
    (new Element('#serviceconfig-irc-newhost-btnsave')).removeAttr('disabled');
    (new Element('#serviceconfig-irc-newhost-btnclose')).removeAttr('disabled');
    (new Element('#serviceconfig-irc-newhost-host')).val('');
    (new Element('#serviceconfig-irc-newhost-port')).val('');
    (new Element('#serviceconfig-irc-newhost-password')).val('');
    (new Element('#serviceconfig-irc-newhost-option-ipv6')).val(false);
    (new Element('#serviceconfig-irc-newhost-option-ssl')).val(true);
};
ServiceIrc.addServerPopup_Save = function() {
    disableClosePopup = true;
    (new Element('#serviceconfig-irc-newserver-btnsave')).attr('disabled', 'disabled');
    (new Element('#serviceconfig-irc-newserver-btnclose')).attr('disabled', 'disabled');
    send({cmd: 'addserver',
          type: 'irc',
          name: (new Element('#serviceconfig-irc-newserver-name')).val()
         });
};
ServiceIrc.addHostPopup_Save = function() {
    disableClosePopup = true;
    (new Element('#serviceconfig-irc-newserver-btnsave')).attr('disabled', 'disabled');
    (new Element('#serviceconfig-irc-newserver-btnclose')).attr('disabled', 'disabled');
    send({cmd: 'addhost',
          type: 'irc',
          serverId: ServiceIrc.selectedServerId,
          host: (new Element('#serviceconfig-irc-newhost-host')).val(),
          port: parseInt((new Element('#serviceconfig-irc-newhost-port')).val()),
          password: (new Element('#serviceconfig-irc-newhost-password')).val(),
          ipv6: (new Element('#serviceconfig-irc-newhost-option-ipv6')).val(),
          ssl: (new Element('#serviceconfig-irc-newhost-option-ssl')).val()
         });
};
ServiceIrc.prototype.load = function(json) {
    this.clear();
    if (json) this.data = json;

    var noneSelected = true;
    if (!json.servers) json.servers = {};
    var servers = json.servers;
    for (var serverId in servers) {
        if (!servers[serverId].channels) servers[serverId.channels] = [];
        if (!servers[serverId].hosts) servers[serverId.hosts] = {};

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

    var splitKey = hostKey.split(":", 2);
    var hostVal = splitKey[0];
    var portVal = parseInt(splitKey[1]);

    host.attr('id', 'irc-host-'+hostVal+"-"+portVal);
    host.get().onclick = function() {
        if (ServiceIrc.selectedHost)
            ServiceIrc.selectedHost.removeClass('selected');
        ServiceIrc.selectedHostKey = hostKey;
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
    var serverInput = new Element('input');
    server.add(serverInput);

    if (!serverData.nicks) serverData.nicks = [];
    if (!serverData.hosts) serverData.hosts = {};

    server.attr('id', 'irc-server-'+serverId);
    server.get().onclick = function() {
        if (ServiceIrc.selectedServer)
            ServiceIrc.selectedServer.removeClass('selected');
        self.setServerData(serverData);
        server.class('selected');
        ServiceIrc.selectedServerId = serverId;
        ServiceIrc.selectedServer = server;
    };
    serverInput.val(serverData.name);
    this.networklist.add(server);

    var nicks = serverData.nicks;
    for (var i = 0; i < nicks.length; ++i)
        this.addNick(nicks[i]);

    return server;
};
ServiceIrc.deleteServer = function(serverId) {
    send({type:'irc', cmd:'deleteserver', id:serverId});
    var serverItem = Element('#irc-server-'+serverId);
    var next = serverItem.get().nextElementSibling || serverItem.get().previousElementSibling;
    if (next) next.click();
    serverItem.remove();
    serverList.get('irc', serverId).remove();
};
ServiceIrc.deleteHost = function(serverId, hostKey) {
    var splitKey = hostKey.split(":", 2);
    var host = splitKey[0];
    var port = parseInt(splitKey[1]);

    send({type:'irc', cmd:'deletehost', serverId:serverId, host:host, port:port});
    var hostItem = Element('#irc-host-'+host+"-"+port);
    var next = hostItem.get().nextElementSibling || hostItem.get().previousElementSibling;
    if (next) next.click();
    hostItem.remove();
};
ServiceIrc.prototype.addNick = function(nick) {
    var nickPad = new Element('div');
    var nickEntry = new Element('input');
    nickPad.add(nickEntry);

    nickPad.get().onclick = function() {
        if (ServiceIrc.selectedNick)
            ServiceIrc.selectedNick.removeClass('selected');
        nickPad.class('selected');
        ServiceIrc.selectedNick = nickPad;
    };
    nickEntry.val(nick);
    this.nicklist.add(nickPad);
};
ServiceIrc.prototype.save = function() {
    // TODO save
};
