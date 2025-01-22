/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smoreron <7353718@gmail.com>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/20 18:28:49 by smoreron          #+#    #+#             */
/*   Updated: 2025/01/20 18:28:50 by smoreron         ###   ########.fr       */
/*                                                                            */
/******************************************************************************/



#ifndef SERVER_HPP
#define SERVER_HPP

#include <map>
#include <vector>
#include <string>
#include <netinet/in.h>
#include <poll.h>

#include "Client.hpp"
#include "Channel.hpp"

class Server {
public:
    // Тип функции-обработчика команды: метод Server, принимающий (Client &, std::vector<std::string> &)
    typedef void (Server::*commandFunc)(Client &, std::vector<std::string> &);

private:
    int                                     _port;
    std::string                             _password;
    int                                     _sockfd;
    std::string                             _serverName;

    // Храним клиентов по сокету
    std::map<int, Client>                   _clients;

    // Храним каналы по названию
    std::map<std::string, Channel>          _channels;

    // Мапа команд для быстрого вызова
    std::map<std::string, commandFunc>      _commands;

public:
    Server(int port, const std::string &password);
    ~Server();

    void            run();
    void            initCommands();

    // Обработка сетевых событий (accept, recv)
    void            acceptNewClient(std::vector<struct pollfd> &fds);
    void            handleClientData(std::vector<struct pollfd> &fds, size_t index);

    // Главная функция разбора поступившей строки (одна команда)
    void            processCommand(Client &client, const std::string &line);

    // Утилита для парсинга (split, и т.п.)
    std::vector<std::string> parseLine(const std::string &line);

    // Методы-обработчики команд (из cmd_*.cpp)
    void            cmdPass(Client &client, std::vector<std::string> &params);
    void            cmdNick(Client &client, std::vector<std::string> &params);
    void            cmdUser(Client &client, std::vector<std::string> &params);
    void            cmdJoin(Client &client, std::vector<std::string> &params);
    void            cmdPart(Client &client, std::vector<std::string> &params);
    void            cmdPrivmsg(Client &client, std::vector<std::string> &params);
    void            cmdQuit(Client &client, std::vector<std::string> &params);

    // Геттеры/сеттеры/утилиты
    const std::string &getPassword() const;
    const std::string &getServerName() const;
    std::map<int, Client> &getClients();
    std::map<std::string, Channel> &getChannels();

    void    removeClient(int sockfd, std::vector<struct pollfd> &fds, size_t index);
};

#endif
