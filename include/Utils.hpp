/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmoundir <kmoundir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/12 19:47:44 by kmoundir          #+#    #+#             */
/*   Updated: 2025/08/02 20:02:06 by kmoundir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HPP
# define UTILS_HPP

#include <sstream>
#include <string>
#include <vector>

template <typename T>
inline std::string to_string(const T& value)
{
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

std::string trim(const std::string& str);

bool validateInput(char **av);

std::vector<std::string> split(const std::string& str, char delimiter);

bool isNumber(const std::string& s);

bool isInLimitInt(const std::string& s);

# endif