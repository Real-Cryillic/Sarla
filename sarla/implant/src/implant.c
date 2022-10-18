#include <windows.h>
#include <stdio.h>

// Socket globals
char host[] = "10.10.10.10";
int port = 53;

struct AgentInformation {
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
} Agent;


void Register() {
    DWORD HostnameLength = 260;
    if (GetComputerNameA(Agent.Hostname, &HostnameLength)) {
        printf("Hostname: %s \n", Agent.Hostname);
    }

    DWORD UsernameBuffer = 260;
    if (GetUserNameA(Agent.Username, &UsernameBuffer)) {
        printf("Username: %s \n", Agent.Username);
    }

    if (Agent.ProcId = GetCurrentProcessId()) {
        printf("Process ID: %lu \n", Agent.ProcId);
    }

    if (Agent.Version = GetVersion()) {
        printf("Host Version: %lu \n", Agent.Version);
    }

    
}


int main() {
    Register();
}