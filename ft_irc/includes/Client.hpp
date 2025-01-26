/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smoreron <smoreron@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/24 13:44:23 by smoreron          #+#    #+#             */
/*   Updated: 2025/01/26 12:50:37 by smoreron         ###   ########.fr       */
/*                                                                            */
/******************************************************************************/



#ifndef CLIENT_HPP
#define CLIENT_HPP
 
 
#include <string>

class Client {
private:
    int         _socket;
    bool        _isAuthenticated;   // Авторизован ли (PASS, NICK, USER - ок)
    bool        _hasPass;          // Принял ли правильный PASS
    bool        _hasNick;
    bool        _hasUser;

    std::string _nickname;
    std::string _username;
    std::string _realname;

    std::string _buffer;           // для накопления полученных данных (до \r\n)

public:
    Client();
    Client(int sock);
    ~Client();

    int         getSocket() const;
    void        setSocket(int sock);

    bool        isAuthenticated() const;
    void        setAuthenticated(bool val);

    bool        hasPass() const;
    void        setPass(bool val);

    bool        hasNick() const;
    void        setNickFlag(bool val);

    bool        hasUser() const;
    void        setUserFlag(bool val);

    const std::string &getNickname() const;
    void              setNickname(const std::string &nick);

    const std::string &getUsername() const;
    void              setUsername(const std::string &user);

    const std::string &getRealname() const;
    void              setRealname(const std::string &real);

    std::string       &getBuffer();
};

#endif
