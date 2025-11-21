/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmoundir <kmoundir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/12 15:51:10 by kmoundir          #+#    #+#             */
/*   Updated: 2025/08/11 13:03:34 by kmoundir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "headers.hpp"

std::string trim(const std::string &str)
{
    try
    {
        if (str.empty())
            return "";
        size_t start = 0;
        while (start < str.length() && std::isspace(str[start]))
            start++;

        size_t end = str.length();
        while (end > start && std::isspace(str[end - 1]))
            end--;
        return str.substr(start, end - start);
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        return "";
    }
}

bool validateInput(char **av)
{
    if (!av || !av[1] || !av[2])
        return false;
    try
    {
        std::string strPort = trim(av[1]);
        std::string password = trim(av[2]);
        if (strPort.empty() || password.empty())
            return false;
        int port = std::atoi(strPort.c_str());
        if (port <= 0 || port > 65535)
            return false;
        return true;
    }
    catch (const std::exception &e)
    {
        std::cerr << "ERROR:Invalid input" << std::endl;
        return false;
    }
}

std::vector<std::string> split(const std::string &str, char delimiter)
{
    try
    {
        std::vector<std::string> tokens;
        std::string token;
        std::istringstream tokenStream(str);
        while (std::getline(tokenStream, token, delimiter))
        {
            if (!token.empty())
                tokens.push_back(token);
        }
        return tokens;
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        return std::vector<std::string>();
    }
}

bool isNumber(const std::string &s)
{
    try
    {
        if (s.empty())
            return false;
        for (size_t i = 0; i < s.length(); ++i)
        {
            if (!std::isdigit(s[i]))
                return false;
        }
        return true;
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        return false;
    }
}

bool isInLimitInt(const std::string &s)
{
    try
    {
        long long limit = atoll(s.c_str());
        if (limit > INT_MAX || limit <= 0)
            return false;
        return true;
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        return false;
    }
}
