/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smoreron <smoreron@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/25 11:08:18 by smoreron          #+#    #+#             */
/*   Updated: 2025/01/26 13:03:31 by smoreron         ###   ########.fr       */
/*                                                                            */
/******************************************************************************/


#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <map>
#include <set>

class Client; 

class Channel {
private:
    std::string             _name;
    std::string             _topic; // Участники: можно хранить сокеты или ники
    std::set<int>           _members; // упрощённо: идентификатор - это сокет
    int                     _op;      // оператор канала (сокет того, кто создал)

public:
    Channel();
    Channel(const std::string &name, int creatorSock);
    ~Channel();

    const std::string       &getName() const;
    void                    setName(const std::string &name);

    const std::string       &getTopic() const;
    void                    setTopic(const std::string &topic);

    const std::set<int>     &getMembers() const;

    void    addMember(int sockfd);
    void    removeMember(int sockfd);
    bool    isMember(int sockfd) const;

    int     getOperator() const;
    void    setOperator(int sockfd);
};

#endif
