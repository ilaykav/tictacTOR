#include "rsa_user.h"

int rsa_user::chossing_k()
{
	for (int i = 2; i < 100; i++)
	{
		if ((i*this->fn + 1) % this->e == 0)
		{
			return i;
		}
	}
	return -1;
}


rsa_user::rsa_user()
{
	this->p1 = generatePrime(length);
	this->p2 = generatePrime(length, p1);
	std::cout << "p1- " << p1 << "p2- " << p2 << std::endl;
	this->n = p1*p2;
	this->fn = (p1 - 1)*(p2 - 1);
	this->e = calc_e(fn);
	this->k = this->chossing_k();
	this->d = (int)(k*fn + 1) / e;
	std::cout << "d: " << d << std::endl;
	std::cout << "k: " << k << std::endl;
}

void rsa_user::reset_params()
{
	this->p1 = generatePrime(length);
	this->p2 = generatePrime(length, p1);
	this->n = p1*p2;
	this->fn = (p1 - 1)*(p2 - 1);
	this->e = calc_e(fn);
	this->k = this->chossing_k();
	this->d = (int)(k*fn + 1) / e;
	std::cout << "reseted parameters of the starter node" << std::endl;
}

std::string rsa_user::get_init_msg()
{
	std::string params = std::to_string(this->get_e());
	params.append("@");
	params.append(std::to_string(this->get_n()));
	std::cout << "sending : e = " << this->e << ", n= " << this->n << std::endl;
	char *cparams = new char[strlen(params.c_str()) + 1];
	//strncpy_s(cparams, params.c_str(), _TRUNCATE);
	strcpy_s(cparams, strlen(cparams), params.c_str());
	return cparams;
}

rsa_user::~rsa_user()
{
}

unsigned long rsa_user::get_e() { return e; }

unsigned long rsa_user::get_n() { return n; }

unsigned long rsa_user::get_d() { return d; }


int rsa_user::calculate_original_key(std::string s)
{
	int ret = get_aes_key(std::stoi(s), this->get_d(), this->get_n());
	return ret;
}