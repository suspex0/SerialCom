#include <iostream>
#include "logger.hpp"
#include "Serial.h"

#define DATA_LENGTH 255

int main()
{
	using namespace Common;
	auto logger_instance = std::make_unique<logger>();
	
	const char* test_msg = "150:10;\n";

	COM::SerialPort* SerialPort;

	SerialPort = new COM::SerialPort();
	if (SerialPort->is_connected()) 
	{
		LOG_INFO("Successfully connected to serial-port!");

		for (int i = 0;i < 10; i++)
			if (SerialPort->write(test_msg, DATA_LENGTH))
				LOG_INFO("Sended message.");
	}

	SerialPort->close();
	LOG_INFO("Exit");
	getchar();


	return 0;
}