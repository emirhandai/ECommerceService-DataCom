#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <process.h>  // _beginthread fonksiyonu için

#pragma comment(lib, "ws2_32.lib")  // Winsock library

#define PORT 8080
#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024

// Ürün yapısı
typedef struct {
    int id;
    char name[100];
    float price;
} Product;

// Ürün veritabanı (örnek ürünler)
Product products[] = {
    {1, "Laptop", 899.99},
    {2, "Smartphone", 499.99},
    {3, "Tablet", 299.99},
    {4, "Headphones", 89.99},
    {5, "Smartwatch", 149.99}
};

// Her istemciye özel iş parçacığı fonksiyonu
void handle_client(void* socket_desc) {
    SOCKET client_socket = *(SOCKET*)socket_desc;
    char buffer[BUFFER_SIZE];
    int recv_size;

    // Yeni bağlanan istemciye ürün listesi gönder
    send(client_socket, "Welcome to the E-Commerce Server!\n", 34, 0);
    for (int i = 0; i < sizeof(products) / sizeof(products[0]); i++) {
        snprintf(buffer, sizeof(buffer), "ID: %d, Name: %s, Price: %.2f\n",
            products[i].id, products[i].name, products[i].price);
        send(client_socket, buffer, strlen(buffer), 0);
    }

    // İstemciyle etkileşim
    while (1) {
        memset(buffer, 0, sizeof(buffer));
        recv_size = recv(client_socket, buffer, sizeof(buffer), 0);
        if (recv_size == SOCKET_ERROR || recv_size == 0) {
            break;  // Bağlantı kesildi
        }

        // Sipariş işleme
        printf("Received order: %s\n", buffer);
        send(client_socket, "Order received. Thank you for shopping with us!\n", 47, 0);
    }

    // Bağlantıyı kapat
    closesocket(client_socket);
    printf("Client disconnected\n");
}

int main() {
    WSADATA wsaData;
    SOCKET server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    int client_addr_len = sizeof(client_addr);
    int iResult;

    // Winsock'ı başlat
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed: %d\n", iResult);
        return 1;
    }

    // Sunucu soketi oluştur
    server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (server_socket == INVALID_SOCKET) {
        printf("Socket failed with error: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    // Sunucu adresini yapılandır
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // Sunucu adresine bağlan
    iResult = bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if (iResult == SOCKET_ERROR) {
        printf("Bind failed with error: %d\n", WSAGetLastError());
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    // Sunucuyu dinlemeye başla
    iResult = listen(server_socket, MAX_CLIENTS);
    if (iResult == SOCKET_ERROR) {
        printf("Listen failed with error: %d\n", WSAGetLastError());
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    printf("Server listening on port %d...\n", PORT);

    // Sonsuz döngüde gelen istemcileri kabul et
    while (1) {
        client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_addr_len);
        if (client_socket == INVALID_SOCKET) {
            printf("Accept failed with error: %d\n", WSAGetLastError());
            continue;
        }

        printf("Client connected\n");

        // Her istemci için yeni bir iş parçacığı başlat
        _beginthread(handle_client, 0, (void*)&client_socket);
    }

    // Sunucu soketini kapat
    closesocket(server_socket);
    WSACleanup();
    return 0;
}
