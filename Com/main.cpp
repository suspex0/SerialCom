#include <iostream>
#include "logger.hpp"
#include "Serial.h"

// max data to send

int main()
{
	using namespace Common;

	auto logger_instance = std::make_unique<logger>();

	std::string portName;
	COM::SerialPort* SerialPort;

	if (!COM::select_port(portName))
	{
		LOG_ERROR("INVALID SERIAL PORT SELECTED!");
		return 0;
	}

	SerialPort = new COM::SerialPort(portName.c_str());
	if (SerialPort->is_connected()) 
	{
		LOG_INFO("Successfully connected to serial-port!");

		for (int i = 0;i < 20; i++)
		{
			SerialPort->move_mouse(150, 0);
			Sleep(250);
		}
	}

	SerialPort->close();
	LOG_INFO("Exit");
	getchar();


	return 0;
}