#pragma once

class Server
{
public:
	virtual ~Server() {}

	virtual bool Setup(const std::string& addr) = 0;
	virtual bool PrepareTCPSocket() = 0;
};

std::unique_ptr<Server> CreateServer();

class Client
{
public:
	virtual ~Client() {}

	virtual bool Setup(const std::string& addr) = 0;
	virtual bool Connect() = 0;
};