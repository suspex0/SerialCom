#include "Serial.h"
#include "logger.hpp"

COM::SerialPort::SerialPort()
{
    // Get user input for device selection
    get_port();

    this->connected = false;
    this->handle = CreateFileA(static_cast<LPCSTR>(portName), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL, NULL);

    if (this->handle == INVALID_HANDLE_VALUE)
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

// returns read bytes count, or if error occurs, returns 0
int COM::SerialPort::read(const char* buffer, unsigned int buf_size)
{
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

bool COM::SerialPort::is_connected()
{
    if (!ClearCommError(this->handle, &this->errors, &this->status))
    {
        this->connected = false;
    }

    return this->connected;
}

bool COM::SerialPort::get_port() //added function to find the present serial 
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

        if (test != 0) //QueryDosDevice returns zero if it didn't find an object
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
        
        LOG_RAW(Common::log_color::green | Common::log_color::intensify, "ID", " - ", "COM-X", " - ", "DEVICE-NAME", "\n");
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

        std::string helper = "\\\\.\\COM" + std::to_string(devices.at(input).id);
        portName = helper.c_str();
    }

    return gotPort;
}

void COM::SerialPort::close()
{
    this->~SerialPort();
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