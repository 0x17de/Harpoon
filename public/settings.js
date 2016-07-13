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
ServiceIrc.addServerPopup_Open = function() {
    showpopup('irc-add-server');
    (new Element('#serviceconfig-irc-newserver-btnsave')).attr('disabled', '');
    (new Element('#serviceconfig-irc-newserver-btnclose')).attr('disabled', '');
    (new Element('#serviceconfig-irc-newserver-name')).val('');
    (new Element('#serviceconfig-irc-newserver-port')).val('');
    (new Element('#serviceconfig-irc-newserver-password')).val('');
    (new Element('#serviceconfig-irc-newserver-option-ipv6')).val(false);
    (new Element('#serviceconfig-irc-newserver-option-ssl')).val(true);
};
ServiceIrc.addServerPopup_Save = function() {
    disableClosePopup = true;
    (new Element('#serviceconfig-irc-newserver-btnsave')).attr('disabled', 'disabled');
    (new Element('#serviceconfig-irc-newserver-btnclose')).attr('disabled', 'disabled');
    send({cmd: 'addserver',
          type: 'irc',
          name: (new Element('#serviceconfig-irc-newserver-name')).val(),
          port: (new Element('#serviceconfig-irc-newserver-port')).val(),
          password: (new Element('#serviceconfig-irc-newserver-password')).val(),
          ipv6: (new Element('#serviceconfig-irc-newserver-option-ipv6')).val(),
          ssl: (new Element('#serviceconfig-irc-newserver-option-ssl')).val()
         });
};
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
    var serverInput = new Element('input');
    server.add(serverInput);

    server.attr('data-id', serverId);
    server.get().onclick = function() {
        if (ServiceIrc.selectedServer)
            ServiceIrc.selectedServer.removeClass('selected');
        self.setServerData(serverData);
        server.class('selected');
        ServiceIrc.selectedServer = server;
    };
    serverInput.val(serverData.name);
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

