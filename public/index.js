var layout
  , chat;

function init() {
    layout = new Layout();
    chat = new Chat();
    new IrcService(chat);

    q('#login-user').focus();
    //login('user', 'password');
}

function login() {
    var user = q('#login-user').val();
    var password = q('#login-password').val();
    q('#login-user').val("");
    q('#login-password').val("");
    q('#login').css({'display':'none'});
    chat.login(user, password);
}
