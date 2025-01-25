#include "Client.hpp"

Client::Client()
: _socket(-1), _isAuthenticated(false), _hasPass(false),
  _hasNick(false), _hasUser(false)
{}

Client::Client(int sock)
: _socket(sock), _isAuthenticated(false), _hasPass(false),
  _hasNick(false), _hasUser(false)
{}

Client::~Client() {}

int Client::getSocket() const { return _socket; }
void Client::setSocket(int sock) { _socket = sock; }

bool Client::isAuthenticated() const { return _isAuthenticated; }
void Client::setAuthenticated(bool val) { _isAuthenticated = val; }

bool Client::hasPass() const { return _hasPass; }
void Client::setPass(bool val) { _hasPass = val; }

bool Client::hasNick() const { return _hasNick; }
void Client::setNickFlag(bool val) { _hasNick = val; }

bool Client::hasUser() const { return _hasUser; }
void Client::setUserFlag(bool val) { _hasUser = val; }

const std::string &Client::getNickname() const { return _nickname; }
void Client::setNickname(const std::string &nick) { _nickname = nick; }

const std::string &Client::getUsername() const { return _username; }
void Client::setUsername(const std::string &user) { _username = user; }

const std::string &Client::getRealname() const { return _realname; }
void Client::setRealname(const std::string &real) { _realname = real; }

std::string &Client::getBuffer() { return _buffer; }
