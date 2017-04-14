#include <iostream>
#include <fstream>
#include <string>

#include "Interpreter.h"




int main(int argc, char** argv)
{
	std::ifstream fr;

#ifdef _DEBUG
	fr.open("../Examples/hello.txt");
#else
	if (argc >= 2)
		fr.open(argv[1]);
	else
		fr.open("code.txt");
#endif

	if (fr.is_open() == false)
	{
		std::cerr << "Can\'t open file." << std::endl;
		return 0;
	}


	std::string code;

	while (!fr.eof())
	{
		char buf[512];
		fr.read(buf, 512);

		code += buf;
	}


	Interpreter interpreter;

	interpreter.ready(code);
	interpreter.execute();


	return 0;
}

