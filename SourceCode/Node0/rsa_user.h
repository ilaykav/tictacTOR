#pragma once
#include "rsa.cpp"
#include <string>

class rsa_user
{
private:
	const int length = 4;
	unsigned long d;
	unsigned long p1;
	unsigned long p2;
	unsigned long e;
	unsigned long n;
	unsigned long fn;
	unsigned int k;

public:
	int chossing_k();
	unsigned long get_n();
	unsigned long get_e();
	unsigned long get_d();
	std::string start_connection(const char* host);
	std::string rsa_user::start_tunnel_connection(const char* host, std::string ip);
	std::string rsa_user::get_init_msg();
	std::string start_connection_send_ip(const char* host);
	int calculate_original_key(std::string s);
	void rsa_user::reset_params();
	rsa_user();
	~rsa_user();
};

