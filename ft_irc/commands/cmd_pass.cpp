/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_pass.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smoreron <smoreron@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/26 13:17:00 by smoreron          #+#    #+#             */
/*   Updated: 2025/01/27 13:42:00 by smoreron         ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include "Server.hpp"
#include "Client.hpp"
#include <string>
#include <iostream>
#include <unistd.h>  
 
void Server::cmdPass(Client &client, std::vector<std::string> &params) {
    if (params.empty()) {
        // Недостаточно параметров
        std::string err = ":server 461 PASS :Not enough parameters\r\n";
        send(client.getSocket(), err.c_str(), err.size(), 0);
        return;
    }
    if (client.hasPass()) {
        // Пароль уже установлен
        std::string err = ":server 462 PASS :You may not reregister\r\n";
        send(client.getSocket(), err.c_str(), err.size(), 0);
        return;
    }
    // Сравниваем с серверным паролем
    if (params[0] == getPassword()) {
        client.setPass(true);
    } else {
        // Не совпадает — можно закрыть соединение
        std::string err = ":server 464 PASS :Password incorrect\r\n";
        send(client.getSocket(), err.c_str(), err.size(), 0);
        close(client.getSocket());
        // Внимание: нужно аккуратно убрать клиента из poll,
        // но для простоты - пусть клиент сам разорвёт коннект
    }
}
