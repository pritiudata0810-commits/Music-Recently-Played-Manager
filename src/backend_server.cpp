#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <csignal>

#include "song_manager.h"

// Link with Ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")

static SOCKET serverSocket = INVALID_SOCKET;
static bool running = true;

// Signal handler for clean server shutdown
void signalHandler(int signum) {
    std::cout << "\n[!] Shutting down C++ backend server cleanly...\n";
    running = false;
    if (serverSocket != INVALID_SOCKET) {
        closesocket(serverSocket);
        serverSocket = INVALID_SOCKET;
    }
    WSACleanup();
    exit(0);
}

// Utility to read file contents into a string
bool readFile(const std::string& path, std::string& content, bool binary = false) {
    std::ios_base::openmode mode = std::ios::in;
    if (binary) mode |= std::ios::binary;

    std::ifstream file(path.c_str(), mode);
    if (!file.is_open()) {
        return false;
    }
    std::ostringstream ss;
    ss << file.rdbuf();
    content = ss.str();
    return true;
}

// Extract query parameter value from URL (e.g. ?id=song_1)
std::string getQueryParam(const std::string& uri, const std::string& key) {
    size_t qPos = uri.find('?');
    if (qPos == std::string::npos) return "";

    std::string queryString = uri.substr(qPos + 1);
    std::string prefix = key + "=";
    size_t pos = queryString.find(prefix);
    if (pos == std::string::npos) return "";

    size_t start = pos + prefix.length();
    size_t end = queryString.find('&', start);
    if (end == std::string::npos) {
        return queryString.substr(start);
    }
    return queryString.substr(start, end - start);
}

// Determine content-type header from file path
std::string getContentType(const std::string& path) {
    if (path.find(".html") != std::string::npos) return "text/html; charset=utf-8";
    if (path.find(".css") != std::string::npos) return "text/css; charset=utf-8";
    if (path.find(".js") != std::string::npos) return "application/javascript; charset=utf-8";
    if (path.find(".svg") != std::string::npos) return "image/svg+xml";
    if (path.find(".json") != std::string::npos) return "application/json; charset=utf-8";
    if (path.find(".png") != std::string::npos) return "image/png";
    if (path.find(".jpg") != std::string::npos || path.find(".jpeg") != std::string::npos) return "image/jpeg";
    return "text/plain";
}

// Send standard HTTP response with CORS headers
void sendHttpResponse(SOCKET clientSocket, int statusCode, const std::string& contentType, const std::string& body) {
    std::string statusText = "OK";
    if (statusCode == 404) statusText = "Not Found";
    else if (statusCode == 400) statusText = "Bad Request";
    else if (statusCode == 500) statusText = "Internal Server Error";

    std::ostringstream response;
    response << "HTTP/1.1 " << statusCode << " " << statusText << "\r\n";
    response << "Content-Type: " << contentType << "\r\n";
    response << "Content-Length: " << body.length() << "\r\n";
    response << "Connection: close\r\n";
    response << "Access-Control-Allow-Origin: *\r\n";
    response << "Access-Control-Allow-Methods: GET, POST, OPTIONS\r\n";
    response << "Access-Control-Allow-Headers: Content-Type\r\n";
    response << "\r\n";
    response << body;

    std::string respStr = response.str();
    send(clientSocket, respStr.c_str(), (int)respStr.length(), 0);
    shutdown(clientSocket, SD_SEND);
}

int main(int argc, char* argv[]) {
    signal(SIGINT, signalHandler);

    int desiredPort = 8000;
    if (argc > 1) {
        desiredPort = std::stoi(argv[1]);
    }

    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        std::cerr << "WSAStartup failed with error: " << iResult << "\n";
        return 1;
    }

    serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "socket creation failed: " << WSAGetLastError() << "\n";
        WSACleanup();
        return 1;
    }

    // Set SO_REUSEADDR
    int optval = 1;
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, (const char*)&optval, sizeof(optval));

    // Auto-scan for available port starting from desiredPort
    int port = desiredPort;
    bool bound = false;
    sockaddr_in service;

    for (int p = desiredPort; p < desiredPort + 30; ++p) {
        service.sin_family = AF_INET;
        service.sin_addr.s_addr = inet_addr("127.0.0.1");
        service.sin_port = htons(p);

        if (bind(serverSocket, (SOCKADDR*)&service, sizeof(service)) == 0) {
            port = p;
            bound = true;
            break;
        }
    }

    if (!bound) {
        std::cerr << "Could not bind to any port in range " << desiredPort << " - " << (desiredPort + 30) << "\n";
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    // Save active port to file
    std::ofstream portFile("active_port.txt");
    if (portFile.is_open()) {
        portFile << port;
        portFile.close();
    }

    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "listen failed with error: " << WSAGetLastError() << "\n";
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    SongManager manager;

    std::cout << "\n======================================================================\n";
    std::cout << "        STACKSOUND: C++ HTTP SERVER & LIFO BACKEND RUNNING             \n";
    std::cout << "======================================================================\n";
    std::cout << " Server Address:  http://127.0.0.1:" << port << "\n";
    std::cout << " API Endpoints:   /api/songs, /api/state, /api/play, /api/previous,\n";
    std::cout << "                  /api/next, /api/peek, /api/clear\n";
    std::cout << " Static Files:    Serving from 'frontend/' and 'assets/'\n";
    std::cout << " Data Structure:  Array-based SongStack (Max Capacity: " << MAX_STACK_CAPACITY << ")\n";
    std::cout << " Status:          Ready! All user interactions log here in real-time.\n";
    std::cout << "======================================================================\n\n";

    while (running) {
        SOCKET clientSocket = accept(serverSocket, NULL, NULL);
        if (clientSocket == INVALID_SOCKET) {
            if (!running) break;
            continue;
        }

        char buffer[4096];
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        if (bytesReceived <= 0) {
            closesocket(clientSocket);
            continue;
        }
        buffer[bytesReceived] = '\0';
        std::string requestStr(buffer);

        std::istringstream reqStream(requestStr);
        std::string method, uri, httpVersion;
        reqStream >> method >> uri >> httpVersion;

        // Handle CORS Preflight
        if (method == "OPTIONS") {
            sendHttpResponse(clientSocket, 204, "text/plain", "");
            closesocket(clientSocket);
            continue;
        }

        // Strip query string for path matching
        std::string path = uri;
        size_t qPos = path.find('?');
        if (qPos != std::string::npos) {
            path = path.substr(0, qPos);
        }

        OperationResult result;

        // --- API ROUTES ---
        if (path == "/api/songs" && method == "GET") {
            sendHttpResponse(clientSocket, 200, "application/json; charset=utf-8", manager.getLibraryJson());
        }
        else if (path == "/api/state" && method == "GET") {
            sendHttpResponse(clientSocket, 200, "application/json; charset=utf-8", manager.getStateJson());
        }
        else if (path == "/api/play" && (method == "POST" || method == "GET")) {
            std::string songId = getQueryParam(uri, "id");
            if (songId.empty()) {
                // Try to find "id": "song_X" in request body if present
                size_t idPos = requestStr.find("\"id\"");
                if (idPos != std::string::npos) {
                    size_t quote1 = requestStr.find('"', idPos + 4);
                    if (quote1 != std::string::npos) {
                        size_t quote2 = requestStr.find('"', quote1 + 1);
                        if (quote2 != std::string::npos) {
                            songId = requestStr.substr(quote1 + 1, quote2 - quote1 - 1);
                        }
                    }
                }
            }

            manager.playSong(songId, result);
            manager.printTerminalLog(result);
            sendHttpResponse(clientSocket, 200, "application/json; charset=utf-8", manager.getStateJson());
        }
        else if (path == "/api/previous" && (method == "POST" || method == "GET")) {
            manager.previousSong(result);
            manager.printTerminalLog(result);
            sendHttpResponse(clientSocket, 200, "application/json; charset=utf-8", manager.getStateJson());
        }
        else if (path == "/api/next" && (method == "POST" || method == "GET")) {
            manager.nextSong(result);
            manager.printTerminalLog(result);
            sendHttpResponse(clientSocket, 200, "application/json; charset=utf-8", manager.getStateJson());
        }
        else if (path == "/api/peek" && (method == "POST" || method == "GET")) {
            Song topSong;
            manager.peekHistory(topSong, result);
            manager.printTerminalLog(result);
            sendHttpResponse(clientSocket, 200, "application/json; charset=utf-8", manager.getStateJson());
        }
        else if (path == "/api/clear" && (method == "POST" || method == "GET")) {
            manager.clearHistory(result);
            manager.printTerminalLog(result);
            sendHttpResponse(clientSocket, 200, "application/json; charset=utf-8", manager.getStateJson());
        }
        // --- STATIC FILE SERVING ---
        else {
            std::string filePath = "";
            if (path == "/" || path == "/index.html") {
                filePath = "frontend/index.html";
            } else if (path == "/style.css" || path == "/frontend/style.css") {
                filePath = "frontend/style.css";
            } else if (path == "/app.js" || path == "/frontend/app.js") {
                filePath = "frontend/app.js";
            } else if (path.find("/assets/") == 0) {
                filePath = path.substr(1); // remove leading slash
            } else {
                filePath = "frontend" + path;
            }

            std::string fileContent;
            // First try relative path
            bool found = readFile(filePath, fileContent, true);
            // Fallback to absolute project directory
            if (!found) {
                std::string altPath = "E:\\Priti\\B.Tech SEM 3\\FDS\\Music-Recently-Played-Manager\\" + filePath;
                found = readFile(altPath, fileContent, true);
            }

            if (found) {
                sendHttpResponse(clientSocket, 200, getContentType(filePath), fileContent);
            } else {
                std::string notFoundBody = "{\"error\":\"Not Found\",\"path\":\"" + path + "\"}";
                sendHttpResponse(clientSocket, 404, "application/json", notFoundBody);
            }
        }

        closesocket(clientSocket);
    }

    if (serverSocket != INVALID_SOCKET) {
        closesocket(serverSocket);
    }
    WSACleanup();
    return 0;
}
