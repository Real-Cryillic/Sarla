#include <windows.h>

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