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