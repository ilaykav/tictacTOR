#pragma once

#include <random>
#include <math.h>
#include <iostream>
#include <string>
#include <ctime>


static int generate_key()
{
	return rand() % 14 + 2;
}

static std::string r_decrypt(std::string message, int key)
{
	std::cout << "decrypting message " << message << " with key " << key;
	for (char &c : message)
	{
		c = char(int(c) - key);
	}
	std::cout << "decrypted = " << message << std::endl;
	return message;
}

static int r_decrypt(int message, int key)
{
	std::cout << "decrypting message " << message << " with key " << key;
	std::cout << "decrypted = " << int(message) - key << std::endl;
	return int(message) - key;
}

static void r_encrypt(std::string &message, int key)
{
	std::cout << "encrypting message " << message << " with key " << key;
	for (int i = 0; i < message.length(); i++)
	{
		message.at(i) = char(int(message.at(i)) + key);
	}
	std::cout << " message encrypted = " << message << std::endl;
}


static bool isPrime(int n) // TODO: Make this rabin
{
	for (int i = 2; i <= sqrt(n); ++i)
	{
		if (n%i == 0)
		{
			return false;
		}
	}
	return true;
}

static int gcd(int a, int b) {
	return b == 0 ? a : gcd(b, a % b);
}

static unsigned long calc_e(unsigned long fn)
{
	unsigned long e = 3;
	while (gcd(e, fn) != 1)
	{
		e += 2;
	}
	return e;
}

static int generatePrime(int length)
{
	int m = (int)std::pow(10, length);
	int r = (int)std::rand() % m + 1;
	if (r % 2 == 0) { r += 1; }
	while (!isPrime(r))
	{
		r += 2;
	}
	return r;
}

static int generatePrime(int length, int p1)
{
	int m = (int)std::pow(10, length);
	int r = (int)std::rand() % m;
	if (r % 2 == 0) { r += 1; }
	while (!isPrime(r) || r == p1)
	{
		r += 2;
	}
	return r;
}

static std::string calculate_reponse(std::string s, int key)
{
	std::size_t mark = s.find("@");
	std::string se = s.substr(0, static_cast<int>(mark));
	std::string sn = s.substr(static_cast<int>(mark) + 1, std::strlen(s.c_str()) - (static_cast<int>(mark) + 1));

	int n = std::stoi(sn);
	int e = std::stoi(se);

	unsigned long long ret = std::pow(key, e);
	ret = ret % n;
	std::cout << "sending key ^ e mod n = " << key << " ^ " << e << " % " << n << " = " << ret << std::endl;
	std::string sret = std::to_string(ret);

	return sret;
}

static int get_aes_key(int a, int b, int n) {
	std::cout << "c^d mod n = m, c=" << a << " b= " << b << "n= " << n << std::endl;
	long long x = 1, y = a;
	while (b > 0) {
		if (b % 2 == 1) {
			x = (x*y) % n;
		}
		y = (y*y) % n; // squaring the base
		b /= 2;
	}
	return x%n;

}