/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_nick.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smoreron <smoreron@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/26 12:49:57 by smoreron          #+#    #+#             */
/*   Updated: 2025/01/27 12:39:10 by smoreron         ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include "Server.hpp" 
#include "Client.hpp"
#include <string>
#include <unistd.h>

void Server::cmdNick(Client &client, std::vector<std::string> &params) {
    if (params.empty()) {
        std::string err = ":server 431 :No nickname given\r\n";
        send(client.getSocket(), err.c_str(), err.size(), 0);
        return;
    }
    std::string newNick = params[0];

    for (std::map<int, Client>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
        if (it->second.getNickname() == newNick && it->first != client.getSocket()) {
            std::string err = ":server 433 " + newNick + " :Nickname is already in use\r\n";
            send(client.getSocket(), err.c_str(), err.size(), 0);
            return;
        }
    }

    client.setNickname(newNick);
    client.setNickFlag(true);

    if (client.hasNick() && client.hasUser() && client.hasPass()) {
        client.setAuthenticated(true);
        std::string welcome = ":server 001 " + client.getNickname() + " :Welcome to ft_irc_server\r\n";
        send(client.getSocket(), welcome.c_str(), welcome.size(), 0);
    }
}
