#pragma once
#include <SFML/Network.hpp>
#include <string>
#include <iostream>
#include <map>
#include <fstream>
#include <functional>
#include <queue>
#include "serverlog.h"
#include <regex>

#define REQUEST_SIZE 5000


std::string percentDecode(std::string s);

/** 
 * Contains all the characteristics of the action related to the url.
 */
class UrlChar
{
public:
	/** 
	 * \public
	 * The url part after the hostname.
	 */
	std::string url;

	/** 
	 * \public
	 * The url to redirect to if response code is 3XX.
	 */
	std::string redirectUrl;

	/** 
	 * \public
	 * The response code to the request.
	 */
	unsigned short code;

	/**
	 * \public 
	 * The MIME type of the data to send.
	 */
	std::string mimeType;

	/** 
	 * \public
	 * The name of the file to send.
	 */
	std::string filename;

	/** 
	 * \public
	 * The callback function.
	 * \note The callback function is called \b before anything is done with other members.
	 * \param UrlChar* Allow to use this parameter as if it was \c this.
	 */
	std::function<void(UrlChar*)> callback;

	/** 
	 * \public
	 * The list of all variables (prefixed by a \c $) coming from GET and POST methods.
	 * \par Example: var["$name"] to access/modify the name variable.
	 * \note \c $$Location contains the url from which the request is coming.
	 */
	std::map<std::string, std::string>* var;
};

/** 
 * This class allows to run a http server.
 */
class HttpServer
{
public:
	/**
	 * \public
	 * \param port The port where the server will listen.
	 * \note By default, \c port is \c 0 which means that the server will run on any port available.
	 */
	HttpServer(int port = sf::Socket::AnyPort);

	/**
	 * \public
	 * Launches the server.
	 * \param threadNumber The number of threads to use for responses.
	 */
	void launch(int threadNumber = 1);

	/**
	 * \public
	 * Manages the response for a given url.
	 * \param url The url part after the hostname.
	 * \param code The response code to the request.
	 * \param mimeType The MIME type of the data to send.
	 * \param filename The name of the file to send.
	 * \return A reference on the current HttpServer to allow chaining req().
	 * \see UrlChar
	 */
	HttpServer& req(std::string url, unsigned short code, std::string mimeType, std::string filename);
	
	/**
	 * \public
	 * Manages the response for a given url.
	 * \param url The url part after the hostname.
	 * \param code The response code to the request.
	 * \param mimeType The MIME type of the data to send.
	 * \param filename The name of the file to send.
	 * \param callback The callback function.
	 * \note The callback function is called \b before anything is done with other members.
	 * \return A reference on the current HttpServer to allow chaining req().
	 * \see UrlChar, UrlChar.callback
	 */
	HttpServer& req(std::string url, unsigned short code, std::string mimeType, std::string filename, std::function<void(UrlChar*)> callback);
	
	/**
	 * \public
	 * \param url The url part after the hostname.
	 * \param code The response code to the request.
	 * \param redirectUrl The url to redirect to.
	 * \note For redirection, the code must be 3XX.
	 * \return A reference on the current HttpServer to allow chaining req().
	 * \see UrlChar
	 */
	HttpServer& req(std::string url, unsigned short code, std::string redirectUrl);

	/**
	 * \public
	 * \param url The url part after the hostname.
	 * \param code The response code to the request.
	 * \param redirectUrl The url to redirect to.
	 * \param callback The callback function.
	 * \note For redirection, the code must be 3XX.
	 * \note The callback function is called \b before anything is done with other members.
	 * \return A reference on the current HttpServer to allow chaining req().
	 * \see UrlChar, UrlChar.callback
	 */
	HttpServer& req(std::string url, unsigned short code, std::string redirectUrl, std::function<void(UrlChar*)> callback);

	HttpServer& req(std::string url, unsigned short code, std::function<void(UrlChar*)> callback, std::string page_contents);
	void replacePageContent(std::string page_name, std::string page_contents);

	bool logs = true;
	bool saving = true;
private:

	/**
	 * \private
	 * Parse the request to isolate variables.
	 * \param request The full Http Request as a string.
	 * \return A percent encoded string containing the variables.
	 */
	std::string parseContent(std::string request);

	/** 
	 * \private
	 * Sends the response to the request.
	 */
	//void sendHTML();

	/** 
	 * \private
	 * Contains all the characteristics of all the actions related to the urls.
	 */
	std::vector<UrlChar> urlChars;

	/**
	 * \private
	 * Isolate the variables from the url of the Http request.
	 * \param url The url with the variables (as if it was a GET).
	 * \return All the variables
	 */
	std::map<std::string, std::string> parseUrlLocation(std::string url) const;

	/**
	 * \private
	 * Format the url to always match the request.
	 * \param s The url.
	 * \return The formatted url;
	 */
	std::string formatUrl(std::string url) const;

	/** 
	 * \private
	 * The listener of the server.
	 */
	sf::TcpListener listener;
	
	/** 
	 * \private
	 * The queue of all requests received.
	 */
	std::queue<std::pair<std::string, std::unique_ptr<sf::TcpSocket>>> requestList;

	sf::Mutex mutex;

	/** 
	 * \private
	 * Loaded files to avoid opening files all the time.
	 */
	std::map<std::string, std::string> savedFiles;

	/**
	 * \private
	 * \param filename The name of the file.
	 */
	void downloadFile(std::string filename);

	/** 
	 * \private
	 * The list of response threads. 
	 */
	std::vector<std::unique_ptr<sf::Thread>> threads;

	/**
	 * \public
	 * Makes the server listen.
	 */
	void listen();

	/** 
	 * \public
	 * Makes the server answer.
	 */
	void answer();

	/** 
	 * Used to index each thread.
	 */
	int threadIndex = 0;
};

/** 
 * Converts a string into a vector<string> of tokens splitted around a delimiter.
 * \param s The original string.
 * \param delimiter The delimiter around which to split.
 * \return A vector of string tokens.
 */
std::vector<std::string> sToVect(std::string s, std::string delimiter);

/** 
 * Replace a string by another in a string.
 * \param s The original string.
 * \param a The string to replace.
 * \param b The replacing string.
 * \return The modified string.
 * \note \c s is modified \b and returned by this function.
 */
std::string replace(std::string& s, std::string a, std::string b);