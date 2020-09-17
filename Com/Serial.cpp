#include "Serial.h"
#include "logger.hpp"

COM::SerialPort::SerialPort(const char* portName)
{
    this->connected = false;
    this->handle = CreateFileA(static_cast<LPCSTR>(portName), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL, NULL);

    if (this->handle == INVALID_HANDLE_VALUE || this->handle == NULL)
    {
        if (GetLastError() == ERROR_FILE_NOT_FOUND)
        {
            LOG_ERROR("Handle is invalid! ", portName, " is not available");
        }
        else
        {
            LOG_ERROR("ERROR UNKNOWN");
        }

    }
    else
    {
        DCB dcbSerialParameters = { 0 };

        if (!GetCommState(this->handle, &dcbSerialParameters))
        {
            LOG_ERROR("Failed to get serial-port parameters.");
        }
        else
        {
            dcbSerialParameters.BaudRate = CBR_9600;
            dcbSerialParameters.ByteSize = 8;
            dcbSerialParameters.StopBits = ONESTOPBIT;
            dcbSerialParameters.Parity = NOPARITY;
            dcbSerialParameters.fDtrControl = DTR_CONTROL_ENABLE;

            if (!SetCommState(handle, &dcbSerialParameters))
            {
                LOG_ERROR("Could not set serial port parameters!");
            }
            else
            {
                this->connected = true;
                PurgeComm(this->handle, PURGE_RXCLEAR | PURGE_TXCLEAR);
                Sleep(COM_WAIT_TIME);
            }
        }
    }
}

COM::SerialPort::~SerialPort()
{
    if (this->connected)
    {
        this->connected = false;
        CloseHandle_s(this->handle);
    }
}

int COM::SerialPort::read(const char* buffer, unsigned int buf_size)
{
    // returns read bytes count, or if error occurs, returns 0
    DWORD bytesRead{};
    unsigned int toRead = 0;

    ClearCommError(this->handle, &this->errors, &this->status);

    if (this->status.cbInQue > 0)
    {
        if (this->status.cbInQue > buf_size)
        {
            toRead = buf_size;
        }
        else
        {
            toRead = this->status.cbInQue;
        }
    }

    memset((void*)buffer, 0, buf_size);

    if (ReadFile(this->handle, (void*)buffer, toRead, &bytesRead, NULL))
    {
        return bytesRead;
    }

    return 0;
}

bool COM::SerialPort::write(const char* buffer, unsigned int buf_size)
{
    DWORD bytesSend;

    if (!WriteFile(this->handle, (void*)buffer, buf_size, &bytesSend, 0))
    {
        ClearCommError(this->handle, &this->errors, &this->status);
        return false;
    }

    return true;
}

void COM::SerialPort::move_mouse(int x, int y)
{
    char buffer[COM_MAX_LENGTH];
    sprintf(buffer, m_format, x, y);

    if (!this->write(buffer, COM_MAX_LENGTH))
        LOG_ERROR("Error while writing to serial-port.");
}

bool COM::SerialPort::is_connected()
{
    if (!ClearCommError(this->handle, &this->errors, &this->status))
    {
        this->connected = false;
    }

    return this->connected;
}

void COM::SerialPort::close()
{
    this->~SerialPort();
}

// Namespace only

bool COM::select_port(std::string& portName)  
{
    struct DosDevice
    {
        const char* name;
        const char* path;
        int id;
    };

    char lpTargetPath[5000];
    bool gotPort = false;
    std::vector <DosDevice> devices = { };

    for (int i = 0; i < 255; i++) 
    {
        std::string str = "COM" + std::to_string(i); 
        DWORD test = QueryDosDeviceA(str.c_str(), lpTargetPath, 5000);

        if (test != 0) // QueryDosDevice returns zero if not valid
        {
            DosDevice dd;
            dd.name = str.c_str();
            dd.path = lpTargetPath;
            dd.id = i;
            devices.push_back(dd);
            gotPort = true;
        }

        if (::GetLastError() == ERROR_INSUFFICIENT_BUFFER)
        { }
    }

    if (!gotPort)
    {
        LOG_ERROR("NO VALID DEVICE IS CONNECTED!");
        portName = "\0";
    }
    else
    {
        LOG_RAW(Common::log_color::green | Common::log_color::intensify, "Found com-ports:\n");
        int i = 0;

        for (auto item : devices)
        {
            LOG_RAW(Common::log_color::green | Common::log_color::intensify, i, " - ", "COM", item.id, " - ", item.path, "\n");
            ++i;
        }
        
        LOG_RAW(Common::log_color::green | Common::log_color::intensify, "Select com-port: ");
        int input = -1;

        do
        {
            std::cin >> input;
            if (std::cin.fail())
            {
                input = -1;
            }
        } 
        while (input == -1);
        portName = std::string("\\\\.\\COM" + std::to_string(devices.at(input).id));
    }

    return gotPort;
}

BOOL COM::CloseHandle_s(HANDLE handle)
{
    __try {
        if (handle)
        {
            CloseHandle(handle);
            return TRUE;
        }

    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        return FALSE;
    }
    return FALSE;
}
