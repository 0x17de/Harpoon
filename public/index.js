var layout
  , chat;

function init() {
    layout = new Layout();
    chat = new Chat();
    new IrcService(chat);

    chat.login('user', 'password'); // TODO: use login dialog
}
