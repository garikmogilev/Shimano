#pragma comment(lib, "WS2_32.lib")
#include <string>
#include <iostream>
#include "Winsock2.h"
#include "Error.h"
#include <cstdlib>

#define COUNT 10000                     // quantity of packages for sending
#define LOCALHOST                       // use localhost
#define LOCAL_ADDRESS "192.168.0.100"   // local address PC
#define MAX_MESSAGE 50
#define MESSAGE "Hello from ClientU "
#define PORT 2000                       // port
#define EXIT                            // uncomment for send the message for close the server
#define PACKET_LOSS                     // if def, packages will be lost
#include <synchapi.h>

using namespace std;

int main()
{
    WSADATA wsaData;
    SOCKET cS;

    SOCKADDR_IN clnt;
    memset(&clnt,0,sizeof(clnt));   // mem zero

    clnt.sin_family = AF_INET;
    clnt.sin_port = htons(PORT);

#ifdef LOCALHOST
    clnt.sin_addr.s_addr = inet_addr("127.0.0.1");
#endif

#ifndef LOCALHOST
    clnt.sin_addr.s_addr = inet_addr(LOCAL_ADDRESS);
#endif

    int lc = sizeof(clnt);
    char ibuf[50];                              // buffer for input
    int  lb = 0;                                // input buffer length
    char obuf[50];                              // buffer for output
    int  lobuf = 0;                             // output buffer length
    char buffer[20];                            // array for func concat (itoa)

    try
    {
        if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0)
            throw SetErrorMsgText("Startup: ", WSAGetLastError());

        if ((cS = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET)
            throw SetErrorMsgText("socket: ", WSAGetLastError());

        cout << "Will be send " <<  COUNT <<  " packages" << endl;

        for (int i = 1; i <= COUNT; i++)
        {

            strcpy(obuf, MESSAGE);
            strcat(obuf, itoa(i, buffer,10));


            lobuf = sendto(cS, (char *)obuf, MAX_MESSAGE, 0, (sockaddr *) &clnt, lc);
            if (lobuf == SOCKET_ERROR)
                throw SetErrorMsgText("send:", WSAGetLastError());

#ifndef  PACKET_LOSS      //для потери пакетов не должен ожидать приема

            lb = recvfrom(cS, ibuf, MAX_MESSAGE, 0, (sockaddr *) &clnt, &lc);
            if (lb == SOCKET_ERROR)
                throw  SetErrorMsgText("recv:",WSAGetLastError());

            cout << ibuf << endl;
#endif
        }



#ifdef EXIT
        Sleep(5000);                    // waiting for the server

        strcpy(obuf, "");
        lobuf = sendto(cS, (char *)obuf, MAX_MESSAGE, 0, (sockaddr *) &clnt, lc);
        if (lobuf == SOCKET_ERROR)
            throw SetErrorMsgText("send:", WSAGetLastError());
#endif

        if (closesocket(cS) == SOCKET_ERROR)
            throw SetErrorMsgText("closesocket: ", WSAGetLastError());

        if (WSACleanup() == SOCKET_ERROR)
            throw SetErrorMsgText("Cleanup: ", WSAGetLastError());
    }
    catch (string errorMsgText)
    {
        cout << endl << errorMsgText << endl;
    }
    return 0;
}