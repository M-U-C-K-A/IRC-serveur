/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hdelacou <hdelacou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/16 06:12:15 by hdelacou          #+#    #+#             */
/*   Updated: 2025/12/16 06:18:41 by hdelacou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
** ============================================================================
**                           STRING PARSING UTILITIES
** ============================================================================
**
**  getParam(): Extracts first parameter from IRC command line
**  getChannelName(): Parses channel name (starting with #) from message
**
** ============================================================================
*/

#include "../includes/Utils.hpp"

/*
* Get the first parameter from a line
* @param cmdLength the length of the command
* @param line the line to parse
* @return the first parameter
*/
const std::string getParam(int cmdLength, const std::string &line)
{
	std::string args = line.substr(cmdLength);
	std::string firstParam = args.substr(0, args.find(' '));
	return (firstParam);
}

/*
* Get the channel name from a line
* @param line the line to parse
* @return the channel name
*/
const std::string getChannelName(const std::string &line)
{
	const size_t channelPos = line.find('#');
	if (channelPos == std::string::npos)
	{
		return (EMPTY_STRING);
	}

	const std::string tmp = line.substr(channelPos, line.length());
	const std::string channelName = tmp.substr(0, tmp.find(' '));
	return (channelName);
}

