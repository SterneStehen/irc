/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smoreron <smoreron@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/24 14:48:26 by smoreron          #+#    #+#             */
/*   Updated: 2025/01/27 13:23:43 by smoreron         ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include "Server.hpp"
#include "Client.hpp" 
#include "Channel.hpp"
#include <iostream>
#include <stdexcept>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <poll.h>

Server::Server(int port, const std::string &password)
: _port(port), _password(password), _sockfd(-1), _serverName("server_irc")
{
    // Создание сокета
    _sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (_sockfd < 0)
        throw std::runtime_error("socket() failed");

    // Опция для повторного использования порта
    int opt = 1;
    if (setsockopt(_sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
        throw std::runtime_error("setsockopt() failed");

    // Привязка socket + port
    sockaddr_in serv_addr;
    std::memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(_port);

    if (bind(_sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        throw std::runtime_error("bind() failed");

    // Слушаем
    if (listen(_sockfd, SOMAXCONN) < 0)
        throw std::runtime_error("listen() failed");

    // Инициализация мапы команд
    initCommands();

    std::cout << "Server started on port " << _port 
              << " with password '" << _password 
			  <<  "\ncontinue connect: telnet localhost " << _port << std::endl;
}

Server::~Server() {
    if (_sockfd != -1)
        close(_sockfd);
}

void Server::run() {
    std::vector<struct pollfd> fds;

    // Добавляем главный сокет
    struct pollfd serverPfd;
    serverPfd.fd = _sockfd;
    serverPfd.events = POLLIN;
    fds.push_back(serverPfd);

    // Основной цикл
    while (true) {
        int ret = poll(&fds[0], fds.size(), -1);
        if (ret < 0) {
            std::cerr << "poll() error\n";
            break;
        }

        for (size_t i = 0; i < fds.size(); i++) {
            if (fds[i].revents & POLLIN) {
                // Проверяем, главный ли это сокет
                if (fds[i].fd == _sockfd) {
                    // Новое соединение
                    acceptNewClient(fds);
                } else {
                    // Пришли данные от клиента
                    handleClientData(fds, i);
                }
            }
            // Если клиент закрылся (POLLHUP / POLLERR)
            if (fds[i].revents & (POLLHUP | POLLERR)) {
                removeClient(fds[i].fd, fds, i);
                break;
            }
        }
    }
}

void Server::initCommands() {
    // Связываем название команды с методом класса Server
    _commands["PASS"]    = &Server::cmdPass;
    _commands["NICK"]    = &Server::cmdNick;
    _commands["USER"]    = &Server::cmdUser;
    _commands["JOIN"]    = &Server::cmdJoin;
    _commands["PART"]    = &Server::cmdPart;
    _commands["PRIVMSG"] = &Server::cmdPrivmsg;
    _commands["QUIT"]    = &Server::cmdQuit;
	_commands["NOTICE"] = &Server::cmdNotice;
}

void Server::acceptNewClient(std::vector<struct pollfd> &fds) {
    sockaddr_in cli_addr;
	char hellomsg[256] = "Hello, it is server IRC 42\n I wait: \n PASS \n NICK \n USER nick 0 * : realname\n";
    socklen_t cli_len = sizeof(cli_addr);
    int clientSock = accept(_sockfd, (struct sockaddr *)&cli_addr, &cli_len);
    if (clientSock < 0) {
        std::cerr << "accept() failed\n";
        return;
    }
	
	send(clientSock, hellomsg, sizeof(hellomsg), 0);

    // Создаём Client
    Client newClient(clientSock);
    _clients[clientSock] = newClient;

    // Добавляем в poll
    struct pollfd pfd;
    pfd.fd = clientSock;
    pfd.events = POLLIN;
    fds.push_back(pfd);

    std::cout << "New client connected: " << clientSock << std::endl;
}

void Server::handleClientData(std::vector<struct pollfd> &fds, size_t index) {
    int clientSock = fds[index].fd;
    char buffer[512];
    std::memset(buffer, 0, sizeof(buffer));

    int n = recv(clientSock, buffer, sizeof(buffer) - 1, 0);
    if (n <= 0) {
        // Клиент отключился или ошибка
        removeClient(clientSock, fds, index);
        return;
    }

    // Добавляем в буфер клиента
    Client &client = _clients[clientSock];
    client.getBuffer() += std::string(buffer);

    // Разбираем по \r\n
    std::string &buf = client.getBuffer();
    size_t pos;
    while ((pos = buf.find("\r\n")) != std::string::npos) {
        std::string line = buf.substr(0, pos);
        buf.erase(0, pos + 2);

        // Обработка одной команды
        processCommand(client, line);
    }
}

void Server::processCommand(Client &client, const std::string &line) {
    if (line.empty())
        return;

    std::vector<std::string> tokens = parseLine(line);
    if (tokens.empty())
        return;

    std::string command = tokens[0];
    // Удаляем команду из tokens[0], чтобы в tokens остались только аргументы
    tokens.erase(tokens.begin());

    // Ищем команду в _commands
    if (_commands.find(command) != _commands.end()) {
        commandFunc func = _commands[command];
		std::string msg_run = "Server 42 IRC run  "  + command + "\n";
        send(client.getSocket(), msg_run.c_str(), msg_run.size(), 0);
        // Вызываем соответствующую функцию
        (this->*func)(client, tokens);
    } else {
        // Можно отправить клиенту сообщение об ошибке/неизвестной команде
        std::string msg = ":server 42  " + client.getNickname() + " " + command + " :Unknown command\r\n";
        send(client.getSocket(), msg.c_str(), msg.size(), 0);
    }
}

std::vector<std::string> Server::parseLine(const std::string &line) {
    // Очень упрощённый парсер:
    // 1. Разделяем строку по пробелам
    // 2. Если есть двоеточие (:) — остаток строки считаем одним аргументом
    std::vector<std::string> tokens;
    size_t i = 0;
    while (i < line.size()) {
        // Пропускаем пробелы
        while (i < line.size() && isspace(line[i]))
            i++;
        if (i >= line.size()) break;

        // Если нашли ':', забираем всё до конца как один токен
        if (line[i] == ':') {
            tokens.push_back(line.substr(i + 1));
            break;
        }
        // Иначе копим символы до следующего пробела
        size_t start = i;
        while (i < line.size() && !isspace(line[i]))
            i++;
        tokens.push_back(line.substr(start, i - start));
    }
    return tokens;
}

void Server::removeClient(int sockfd, std::vector<struct pollfd> &fds, size_t index) {
    std::cout << "Client disconnected: " << sockfd << std::endl;

    // Удаляем клиента из всех каналов
    for (std::map<std::string, Channel>::iterator it = _channels.begin(); it != _channels.end(); ++it) {
        Channel &chan = it->second;
        if (chan.isMember(sockfd)) {
            chan.removeMember(sockfd);
        }
    }

    close(sockfd);
    _clients.erase(sockfd);
    fds.erase(fds.begin() + index);
}

// Геттеры/сеттеры
const std::string &Server::getPassword() const {
    return _password;
}

const std::string &Server::getServerName() const {
    return _serverName;
}

std::map<int, Client> &Server::getClients() {
    return _clients;
}

std::map<std::string, Channel> &Server::getChannels() {
    return _channels;
}
