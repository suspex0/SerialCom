#pragma once
#define COM_WAIT_TIME 2000
#define MAX_DATA_LENGTH 255

#include <windows.h>
#include <iostream>



namespace COM
{

    BOOL CloseHandle_s(HANDLE handle);

    class SerialPort
    {
        private:
            bool        connected;
            HANDLE      handle = NULL;
            COMSTAT     status;
            DWORD       errors = 0;
            const char* portName;

        public:
            explicit    SerialPort();
                        ~SerialPort();
            

            DWORD       get_error() { return errors; }
            bool        get_port();
            int         read(const char* buffer, unsigned int buf_size);
            bool        write(const char* buffer, unsigned int buf_size);
            bool        is_connected();
            void        close();
    };

}

