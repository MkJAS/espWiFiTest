#include <iostream>
#include <string>
#include <winsock2.h>  // For Windows Sockets
#include <ws2tcpip.h>  // For additional socket functions (inet_pton, etc.)
#pragma comment(lib, "Ws2_32.lib")  // Link against Winsock library

#define DEFAULT_PORT "8080" // Change the port number as needed
#define DEFAULT_BUFFER_LENGTH 512

int main() {
    WSADATA wsaData;
    SOCKET ConnectSocket = INVALID_SOCKET;
    struct addrinfo *result = NULL, *ptr = NULL, hints;
    std::string serverAddress;
    std::string message;
    char recvbuf[DEFAULT_BUFFER_LENGTH];
    int recvbuflen = DEFAULT_BUFFER_LENGTH;

    // Step 1: Initialize Winsock
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        std::cerr << "WSAStartup failed: " << iResult << std::endl;
        return 1;
    }

    // Step 2: Setup address info hints
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Step 3: Ask user for server IP address
    std::cout << "Enter server IP address: ";
    std::getline(std::cin, serverAddress);

    // Step 4: Resolve the server address and port
    iResult = getaddrinfo(serverAddress.c_str(), DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        std::cerr << "getaddrinfo failed: " << iResult << std::endl;
        WSACleanup();
        return 1;
    }

    // Step 5: Attempt to connect to an address
    for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {
        // Create socket
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
        if (ConnectSocket == INVALID_SOCKET) {
            std::cerr << "Error at socket(): " << WSAGetLastError() << std::endl;
            WSACleanup();
            return 1;
        }

        // Connect to server
        iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
            continue;
        }
        break;
    }

    freeaddrinfo(result);

    if (ConnectSocket == INVALID_SOCKET) {
        std::cerr << "Unable to connect to server!" << std::endl;
        WSACleanup();
        return 1;
    }

    std::cout << "Connected to server. Type your messages to send (type 'exit' to quit):" << std::endl;

    // Step 6: Send and receive data
    while (true) {
        // Get user input
        std::cout << "> ";
        std::getline(std::cin, message);

        // Exit condition
        if (message == "exit") {
            break;
        }

        // Send the message to the server
        iResult = send(ConnectSocket, message.c_str(), (int)message.length(), 0);
        if (iResult == SOCKET_ERROR) {
            std::cerr << "send failed: " << WSAGetLastError() << std::endl;
            closesocket(ConnectSocket);
            WSACleanup();
            return 1;
        }

        std::cout << "Bytes Sent: " << iResult << std::endl;

        // Receive response from server
        // iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
        // if (iResult > 0) {
        //     std::cout << "Bytes received: " << iResult << std::endl;
        //     std::cout << "Server response: " << std::string(recvbuf, iResult) << std::endl;
        // } else if (iResult == 0) {
        //     std::cout << "Connection closed by server." << std::endl;
        //     break;
        // } else {
        //     std::cerr << "recv failed: " << WSAGetLastError() << std::endl;
        //     break;
        // }
    }

    // Step 7: Shutdown the connection
    iResult = shutdown(ConnectSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        std::cerr << "shutdown failed: " << WSAGetLastError() << std::endl;
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }

    // Cleanup
    closesocket(ConnectSocket);
    WSACleanup();

    return 0;
}
