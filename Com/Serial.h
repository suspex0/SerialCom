#pragma once
#define COM_WAIT_TIME 2000
#define MAX_DATA_LENGTH 255

#include <windows.h>
#include <iostream>

#define COM_MAX_LENGTH 255

namespace COM
{

    BOOL            CloseHandle_s(HANDLE handle);
    bool            select_port(std::string& portName);

    class SerialPort
    {
        private:
            bool        connected;
            HANDLE      handle = NULL;
            COMSTAT     status;
            DWORD       errors = 0;

            const char* m_format = "%d:%d\n";

        public:
            explicit    SerialPort(const char* portName);
                        ~SerialPort();

            DWORD       get_error() { return errors; }

            void        move_mouse(int x, int y);

            bool        is_connected();
            int         read(const char* buffer, unsigned int buf_size);
            bool        write(const char* buffer, unsigned int buf_size);
            void        close();
    };

}

