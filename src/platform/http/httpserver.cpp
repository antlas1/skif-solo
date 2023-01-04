#include "httpserver.h"

std::string percentDecode(std::string s)
{
	std::map<std::string, std::string> reserved = {
		{"%21", "!"},
		{"%22", "\""},
		{"%23", "#"},
		{"%24", "$"},
		{"%25", "%"},
		{"%26", "&"},
		{"%27", "'"},
		{"%28", "("},
		{"%29", ")"},
		{"%2A", "*"},
		{"%2B", "+"},
		{"%2C", ","},
		{"%2F", "/"},
		{"%3A", ":"},
		{"%3B", ";"},
		{"%3D", "="},
		{"%3F", "?"},
		{"%40", "@"},
		{"%5B", "["},
		{"%5D", "]"},
		{"+", " "},
		{"%E7", "з"},
		{"%E9", "й"},
		{"%E8", "и"},
		{"%E0", "а"},
		{"%EA", "к"},
		{"%E2", "в"},
		{"%F9", "щ"},
		{"%FB", "ы"},
		{"%EB", "л"},
		{"%EF", "п"},
		{"%C7", "З"},
		{"%C9", "Й"},
		{"%C0", "А"},
		{"%C8", "И"},
		{"%CA", "К"},
		{"%CB", "Л"},
		{"%C2", "В"}
	};

	for (auto& f : reserved)
	{
		while (s.find(f.first) != std::string::npos)
		{
			s = replace(s, f.first, f.second);
		}
	}
	return s;
}

HttpServer::HttpServer(int port)
{
	if (listener.listen(port) != sf::Socket::Done)
	{
		Log("Error", "Couldn't listen on port " + std::to_string(port), (int)ConsoleColor::RED).print();
	}
	Log("Listening", "Port " + std::to_string(port), (int)ConsoleColor::GREEN).print();
}

void HttpServer::launch(int threadNumber)
{
	sf::Thread listener(&HttpServer::listen, this);
	//listener.launch();
	for (int i = 0; i < threadNumber; i++)
	{
		threads.push_back(std::make_unique<sf::Thread>(&HttpServer::answer, this));
		threads.push_back(std::make_unique<sf::Thread>(&HttpServer::listen, this));
	}
	for (int i = 0; i < threadNumber * 2; i++)
	{
		threads[i++]->launch();
		threads[i]->launch();
		sf::sleep(sf::milliseconds(100));
		threadIndex++;
	}
}

void HttpServer::listen()
{
	int index = threadIndex;
	std::unique_ptr<sf::TcpSocket> client = std::make_unique<sf::TcpSocket>();
	std::string requestLocation;
	client->setBlocking(true);
	while (listener.accept(*client) == sf::Socket::Status::Done)
	{
		char request[REQUEST_SIZE];
		size_t received;
		requestLocation.clear();
		std::string fullRequest;

		if (client->receive(request, REQUEST_SIZE, received) == sf::Socket::Done)
		{
			if (received < REQUEST_SIZE)
				request[received] = '\0';
			fullRequest += request;
			client->setBlocking(false);
			//sleep(milliseconds(100));
			while (client->receive(request, REQUEST_SIZE, received) == sf::Socket::Done)
			{
				if (received < REQUEST_SIZE)
					request[received] = '\0';
				fullRequest += request;
			}
			client->setBlocking(true);
			int i = -1;
			bool ok = false;
			while (fullRequest[i + 1] != '\n' && fullRequest[i + 1] != '\0')
			{
				i++;
				if (i >= 1 && fullRequest[i - 1] == ' ' || ok)
					ok = true;
				else
					continue;
				requestLocation += fullRequest[i];
				if (fullRequest[i + 1] == ' ')
					break;
			}

			if (logs)
			{
				mutex.lock();
				Log("Request (" + client->getRemoteAddress().toString() + ")", requestLocation + " (Thread " + std::to_string(index) + ")", (int)ConsoleColor::PINK).print();
				mutex.unlock();
			}

			if (fullRequest.find("POST") != std::string::npos)
			{
				requestLocation = requestLocation + "?" + parseContent(fullRequest);
			}

			mutex.lock();
			requestList.push(std::make_pair(requestLocation, std::make_unique<sf::TcpSocket>()));
			requestList.back().second.reset(client.release());
			client = std::make_unique<sf::TcpSocket>();
			mutex.unlock();
		}
	}
}

void HttpServer::answer()
{
	int index = threadIndex;
	while (true)
	{
		mutex.lock();
		if (!requestList.empty())
		{
			std::string requestLocation = requestList.front().first;
			std::unique_ptr<sf::TcpSocket> client(requestList.front().second.release());
			requestList.pop();
			mutex.unlock();

			std::string filename;
			std::string head;
			std::map<std::string, std::string> parsedUrl = parseUrlLocation(requestLocation);

			std::string mimeType;
			bool found = false;
			for (auto& f : urlChars)
			{
				if (parsedUrl["$$Location"] == f.url)
				{
					found = true;

					f.var = &parsedUrl;
					if (f.callback)
						f.callback(&f);

					mimeType = f.mimeType;
					head = "HTTP/1.1 " + std::to_string(f.code) + "\nContent-Type: " + f.mimeType + "\n";
					if (f.code >= 300 && f.code < 400)
					{
						head += "Location: " + f.redirectUrl + "\n";
					}
					filename = f.filename;
					if (logs)
					{
						mutex.lock();
						Log("Response (" + client->getRemoteAddress().toString() + ")", requestLocation + " FOUND (Thread " + std::to_string(index) + ")", (int)ConsoleColor::GREEN).print();
						mutex.unlock();
					}
					break;
				}
			}
			if (!found)
			{
				if (logs)
				{
					mutex.lock();
					Log("Response (" + client->getRemoteAddress().toString() + ")", requestLocation + " NOT FOUND", (int)ConsoleColor::RED).print();
					mutex.unlock();
				}

				for (auto& f : urlChars)
				{
					if (f.code == 404 || f.url == formatUrl("default"))
					{
						found = true;

						f.var = &parsedUrl;
						if (f.callback)
							f.callback(&f);

						head = "HTTP/1.1 " + std::to_string(f.code) + "\nContent-Type: " + f.mimeType + "\n";
						if (f.redirectUrl.size())
							head += "Location: " + f.redirectUrl + "\n";
						filename = f.filename;
					}
				}
			}

			if (found)
			{
				std::string page;
				if(saving)
					page = savedFiles[filename];
				else
				{
					std::ifstream file(filename.c_str(), std::ifstream::binary);
					bool started = false;
					while (file.good())
					{
						if (!started)
						{
							started = !started;
						}
						else page += '\n';
						std::string buffer;
						std::getline(file, buffer);
						page += buffer;
					}
				}
				if (mimeType.find("text") != std::string::npos || mimeType.find("application") != std::string::npos)
				{
					for (auto& var : parsedUrl)
					{
						std::regex r("\\$");
						r = std::regex(std::regex_replace(var.first, r, "\\$"));
						page = std::regex_replace(page, r, var.second);
					}
					std::regex r("\\$[a-zA-Z0-9]+");
					page = std::regex_replace(page, r, "null");
				}
				head += "Content-Length: " + std::to_string(page.size()) + "\n\n" + page;
				mutex.lock();
				client->send(head.c_str(), head.size() + 1);
				mutex.unlock();
			}
			else
			{
				std::string s = "HTTP/1.1 404 NOT FOUND\nContent-Type: text/html\nContent-Length: 155\n\n<!DOCTYPE HTML/>\n<html>\n<head>\n<title>Page Not Found</title>\n<meta charset=\"iso-8859-1\"/>\n</head><body>\n<h1>Error 404 : Page Not Found</h1>\n</body>\n</html>";
				mutex.lock();
				client->send(s.c_str(), s.size() + 1);
				mutex.unlock();
			}
		}
		else
		{
			mutex.unlock();
			sf::sleep(sf::milliseconds(100));
			continue;
		}
	}
}

HttpServer& HttpServer::req(std::string url, unsigned short code, std::string mimeType, std::string filename)
{
	UrlChar buffer;
	buffer.url = formatUrl(url);
	buffer.code = code;
	buffer.mimeType = mimeType;
	buffer.filename = filename;
	if (saving)
		downloadFile(filename);
	urlChars.push_back(buffer);

	return *this;
}

HttpServer& HttpServer::req(std::string url, unsigned short code, std::string mimeType, std::string filename, std::function<void(UrlChar*)> callback)
{
	UrlChar buffer;
	buffer.url = formatUrl(url);
	buffer.code = code;
	buffer.mimeType = mimeType;
	buffer.filename = filename;
	buffer.callback = callback;
	if (saving)
		downloadFile(filename);

	urlChars.push_back(buffer);

	return *this;
}

HttpServer& HttpServer::req(std::string url, unsigned short code, std::string redirectUrl)
{
	UrlChar buffer;
	buffer.url = formatUrl(url);
	buffer.code = code;
	buffer.redirectUrl = redirectUrl;
	buffer.mimeType = "text/plain";

	urlChars.push_back(buffer);

	return *this;
}

HttpServer& HttpServer::req(std::string url, unsigned short code, std::string redirectUrl, std::function<void(UrlChar*)> callback)
{
	UrlChar buffer;
	buffer.url = formatUrl(url);
	buffer.code = code;
	buffer.redirectUrl = redirectUrl;
	buffer.mimeType = "text/plain";
	buffer.callback = callback;

	urlChars.push_back(buffer);

	return *this;
}

HttpServer& HttpServer::req(std::string url, unsigned short code, std::function<void(UrlChar*)> callback, std::string page_contents)
{
	UrlChar buffer;
	buffer.url = formatUrl(url);
	buffer.code = code;
	buffer.mimeType = "text/html";
	buffer.callback = callback;
	buffer.filename = buffer.url;

	savedFiles[buffer.url] = page_contents;
	urlChars.push_back(buffer);

	return *this;
}

void HttpServer::replacePageContent(std::string page_name, std::string page_contents)
{
	if (savedFiles.count(page_name))
	{
		savedFiles[page_name] = page_contents;
	}
}

std::string HttpServer::parseContent(std::string request)
{
	std::string cl = "Content-Length: ";
	int valueSize = std::stoi(request.substr(request.find(cl) + cl.size(), request.find("\r\n", request.find(cl)) - request.find(cl) - cl.size()));

	std::string res = request.substr(request.find("\r\n\r\n") + 4, valueSize);
	return res;
}

std::map<std::string, std::string> HttpServer::parseUrlLocation(std::string url) const
{
	std::map<std::string, std::string> res;
	res.emplace("$$Location", url.substr(0, url.find("?")));
	if (url.find("?") != std::string::npos)
	{
		url = url.substr(url.find("?") + 1, url.size());
		std::vector<std::string> pairs = sToVect(url, "&");

		for (auto& p : pairs)
		{
			res.emplace("$" + p.substr(0, p.find("=")), percentDecode(p.substr(p.find("=") + 1, p.size())));
		}
	}

	return res;
}

std::string HttpServer::formatUrl(std::string url) const
{
	if (!url.empty() && url[0] != '/')
	{
		url = "/" + url;
	}
	else if (!url.empty() && (url.size() == 1) && url[0] == '/')
	{
		//ничего не делаем
	}
	else if (!url.empty() && url[url.size() - 1] == '/')
	{
		url.resize(url.size() - 1);
	}
	return url;
}

void HttpServer::downloadFile(std::string filename)
{
	std::string fileStr;
	if (savedFiles.find(filename) == savedFiles.end())
	{
		std::ifstream file(filename.c_str(), std::ifstream::binary);
		bool started = false;
		while (file.good())
		{
			if (!started)
			{
				started = !started;
			}
			else fileStr += '\n';
			std::string buffer;
			std::getline(file, buffer);
			fileStr += buffer;
		}
		savedFiles.emplace(filename, fileStr);
	}
}

std::vector<std::string> sToVect(std::string s, std::string delimiter)
{
	size_t pos = 0;
	std::string token;
	std::vector<std::string> res;
	while ((pos = s.find(delimiter)) != std::string::npos)
	{
		token = s.substr(0, pos);
		res.push_back(token);
		s.erase(0, pos + delimiter.length());
	}
	if (!s.empty())
		res.push_back(s);
	return res;
}

std::string replace(std::string& s, std::string a, std::string b)
{
	return(s.replace(s.find(a), a.length(), b));
}