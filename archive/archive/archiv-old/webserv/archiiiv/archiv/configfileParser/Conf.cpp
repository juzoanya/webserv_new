/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Conf.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/19 12:30:42 by mberline          #+#    #+#             */
/*   Updated: 2023/12/20 11:21:11 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <fstream>
#include <sstream>
#include "Conf.hpp"

Config::Config( void )
{ }

Config::~Config( void )
{ }

int checkDirectives(std::stringstream & dirstream)
{
    std::string dirval;
    dirstream >> dirval;
    if (dirval.size() > 0 && dirval[0] == '#')
        return (0);
    std::cout << "KEY:" << dirval << std::endl;
    while (dirstream >> dirval) {
        std::cout << "VALUE:" << dirval << std::endl;
    }
    return (1);
}

// int recursiveContext(std::stringstream & ss)
// {
//     std::cout << "new recursion" << std::endl;
//     if (ss.eof()) {
//         std::cout << "returnn"<< std::endl;
//         return (0);
//     }
//     std::string context;
//     std::string contextval;
//     std::getline(ss, context, '{');
//     std::cout << "context: " << context << std::endl;
//     recursiveContext(ss);
//     // std::cout << "------------------------------" << std::endl;
//     // std::cout << "after recursion: ss is: " << ss.str() << std::endl;
//     std::cout << "------------------------------" << std::endl;
//     std::cout << "after recursion: context is: " << context << std::endl;
//     std::cout << "------------------------------" << std::endl;
//     std::stringstream newss(context);
//     std::getline(newss, contextval, '}');
//     std::cout << "after recursion: contextval is: " << contextval << std::endl;
//     std::cout << "------------------------------" << std::endl;
//     // std::cout << "context: " << context << std::endl;
//     // std::cout << "context val: \n" << contextval << std::endl;
//     return (1);
// }


int recursiveContext2(std::stringstream & ss)
{
    std::cout << "new recursion" << std::endl;
    if (ss.eof()) {
        std::cout << "returnn"<< std::endl;
        return (0);
    }
    std::string context;
    std::string contextval;
    std::getline(ss, context, '{');
    std::cout << "context: " << context << std::endl;
    recursiveContext2(ss);
    // std::cout << "------------------------------" << std::endl;
    // std::cout << "after recursion: ss is: " << ss.str() << std::endl;
    std::cout << "------------------------------" << std::endl;
    std::cout << "after recursion: context is: " << context << std::endl;
    std::cout << "------------------------------" << std::endl;
    std::stringstream newss(context);
    std::getline(newss, contextval, '}');
    std::cout << "after recursion: contextval is: " << contextval << std::endl;
    std::cout << "------------------------------" << std::endl;
    // std::cout << "context: " << context << std::endl;
    // std::cout << "context val: \n" << contextval << std::endl;
    return (1);
}

// int recursiveContext(std::string contx, std::stringstream & ss)
// {
//     std::cout << "\n\nnew recursion, contx: " << contx << std::endl;
//     if (ss.eof()) {
//         std::cout << "returnn"<< std::endl;
//         return (0);
//     }
//     std::string context;
//     std::string contextval;
//     std::getline(ss, context, '{');
//     std::stringstream helper(context);
//     std::string dirval;
//     while (std::getline(helper, dirval, ';')) {
//         std::stringstream dirstream(dirval);
//         std::string valval;
//         dirstream >> valval;
//         if (valval.size() > 0 && valval[0] == '#') {
//             std::cout << "comment" << std::endl;
//         } else {
//             std::cout << "Key: " << valval << " | Values: ";
//             while (dirstream >> valval) {
//                 std::cout << valval << ", ";
//             }
//             std::cout << std::endl;
//         }
        
//     }
//     // helper >> contx;
//     contx = dirval;
//     std::cout << "context: " << context << std::endl;
//     recursiveContext(contx, ss);
//     std::cout << "\n\n ------------- contx is: " << contx << "-----------------" << std::endl;
//     std::cout << "after recursion: context is: " << context << std::endl;
//     std::cout << "------------------------------" << std::endl;
//     std::stringstream newss(context);
//     std::getline(newss, contextval, '}');
//     std::cout << "\nafter recursion: contextval is: " << contextval << std::endl;
//     std::cout << "------------------------------" << std::endl;
//     return (1);
// }

// int recursiveContext(std::string contx, std::stringstream & ss)
// {
//     std::string braceopen;
//     std::string braceclose;
//     std::getline(ss, braceopen, '{');
//     std::getline(ss, braceclose, '}');
//     std::stringstream   checkprev(braceclose);
//     std::string braceopennested;
//     std::getline(checkprev, braceopennested, '{');

//     std::cout << "contx: " << contx << std::endl;
//     std::cout << "braceopen: " << braceopen << std::endl;
//     std::cout << "braceclose: " << braceclose << std::endl;
//     std::cout << "braceopennested: " << braceopennested << std::endl;
    
    
//     // if (braceopen.size() < braceclose.size()) {
        
//     // }
//     // std::cout << "context: " << context << std::endl;
//     // std::cout << "\n\n ------------- contx is: " << contx << "-----------------" << std::endl;
//     // std::cout << "after recursion: context is: " << context << std::endl;
//     // std::cout << "------------------------------" << std::endl;
//     // std::cout << "\nafter recursion: contextval is: " << contextval << std::endl;
//     // std::cout << "------------------------------" << std::endl;
//     return (1);
// }



typedef std::vector< std::pair< std::string, std::vector<std::string> > > keyvalue_t;
typedef std::vector< std::pair< std::vector<std::string>, std::vector<std::string> > > keysvalues_t;

struct ConfigInstance {
    std::vector<std::string>    currInstance;
    std::string                 instanceName;
    std::string                 instanceValue;
    keysvalues_t                directives;
    std::vector<ConfigInstance> nestedInstances;
};


int parseDirectives(std::string & directives)
{
    std::stringstream   helper(directives);
    std::string         directive;
    while (std::getline(helper, directive, ';')) {
        std::stringstream dirstream(directive);
        std::string key;
        std::string value;
        std::vector<std::string> values;
        dirstream >> key;
        if (key.size() > 0 && key[0] == '#') {
            std::cout << "comment" << std::endl;
        } else {
            std::cout << "Key: " << key << " | Values: ";
            while (dirstream >> value) {
                std::cout << value << ", ";
                values.push_back(value);
            }
            std::cout << std::endl;
        }
        
    }
    return (0);
}


int recursiveContext(std::string context, std::string values, std::vector<ConfigInstance>& instances)
{
    std::stringstream ss(values);
    std::string subvalues;
    std::getline(ss, subvalues, '{');
    recursiveContext()
    
    
    // std::cout << "------------------------------" << std::endl;
    return (1);
}


int Config::parseConfig( const char *filepath )
{
	std::ifstream	ifs;
	ifs.open(filepath);
	if (ifs.fail())
		return (-1);
    std::stringstream   ss;
    ss << ifs.rdbuf();
    std::string buff = ss.str();
    std::string directive;
    ifs.close();

    std::string ht = "http";
    std::vector<ConfigInstance> instances;
    // std::vector<keyvalue_t
    // std::vector< std::pair< std::string, std::vector<std::string> > >
    recursiveContext(ht, ss);

    // while (std::getline(ss, directive, ';')) {
    //     std::cout << "current directve:" << directive << std::endl;
    // }

    // std::string context;
    // std::string contextval;
    // while (std::getline(ss, context, '{') && std::getline(ss, contextval, '}')) {
    //     std::cout << "context: " << context << std::endl;
    //     std::cout << "context val: \n" << contextval << std::endl;
    // }
    //     while (std::getline(ss, directive, ';')) {
            
    //         std::cout << "current directve:" << directive << std::endl;
    //         std::stringstream dirstream(directive);
    //         std::string dirval;
    //         dirstream >> dirval;
    //         std::cout << "KEY:" << dirval << std::endl;
    //         while (dirstream >> dirval) {
    //             std::cout << "VALUE:" << dirval << std::endl;
    //         }
    //     }

    return (1);
}

int	main(int argc, char** argv)
{
	if (argc != 2) {
		std::cerr << "invalid number of arguments\n";
	}
	Config	conf;
	try
	{
		conf.parseConfig(argv[1]);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	
	return (0);
}
