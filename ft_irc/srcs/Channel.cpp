/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smoreron <smoreron@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/25 11:08:26 by smoreron          #+#    #+#             */
/*   Updated: 2025/01/26 12:50:55 by smoreron         ###   ########.fr       */
/*                                                                            */
/******************************************************************************/


#include "Channel.hpp"

Channel::Channel() : _op(-1) {}

Channel::Channel(const std::string &name, int creatorSock)
: _name(name), _op(creatorSock) {}

Channel::~Channel() {}

const std::string &Channel::getName() const {
    return _name;
}
void Channel::setName(const std::string &name) {
    _name = name;
}

const std::string &Channel::getTopic() const {
    return _topic;
}
void Channel::setTopic(const std::string &topic) {
    _topic = topic;
}

const std::set<int> &Channel::getMembers() const {
    return _members;
}

void Channel::addMember(int sockfd) {
    _members.insert(sockfd);
}

void Channel::removeMember(int sockfd) {
    _members.erase(sockfd);
}

bool Channel::isMember(int sockfd) const {
    return (_members.find(sockfd) != _members.end());
}

int Channel::getOperator() const {
    return _op;
}
void Channel::setOperator(int sockfd) {
    _op = sockfd;
}
