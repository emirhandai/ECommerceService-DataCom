#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")  // Winsock library

#define PORT 8080
#define BUFFER_SIZE 1024
#define MAX_CART_ITEMS 10

// Ürün yapısı
typedef struct {
    int id;
    char name[100];
    float price;
} Product;

// Sepet yapısı
typedef struct {
    char name[100];
    int quantity;
} CartItem;

// Ürün veritabanı (örnek ürünler)
Product products[] = {
    {1, "Laptop", 899.99},
    {2, "Smartphone", 499.99},
    {3, "Tablet", 299.99},
    {4, "Headphones", 89.99},
    {5, "Smartwatch", 149.99}
};

CartItem cart[MAX_CART_ITEMS];
int cart_size = 0;

void display_products() {
    printf("\nAvailable products:\n");
    for (int i = 0; i < sizeof(products) / sizeof(products[0]); i++) {
        printf("ID: %d, Name: %s, Price: %.2f\n", products[i].id, products[i].name, products[i].price);
    }
}

void add_to_cart(const char* product_name) {
    // Sepete ürün ekle
    if (cart_size < MAX_CART_ITEMS) {
        for (int i = 0; i < sizeof(products) / sizeof(products[0]); i++) {
            if (strcmp(products[i].name, product_name) == 0) {
                strcpy_s(cart[cart_size].name, sizeof(cart[cart_size].name), product_name);
                cart[cart_size].quantity = 1;  // İlk başta bir adet ekle
                cart_size++;
                printf("Product '%s' added to the cart.\n", product_name);
                return;
            }
        }
        printf("Product not found.\n");
    }
    else {
        printf("Cart is full. Cannot add more items.\n");
    }
}

void remove_from_cart(const char* product_name) {
    for (int i = 0; i < cart_size; i++) {
        if (strcmp(cart[i].name, product_name) == 0) {
            // Ürün bulundu ve silindi
            for (int j = i; j < cart_size - 1; j++) {
                cart[j] = cart[j + 1];  // Silinen ürünün yerini kaydır
            }
            cart_size--;
            printf("Product '%s' removed from the cart.\n", product_name);
            return;
        }
    }
    printf("Product '%s' not found in the cart.\n", product_name);
}

void view_cart() {
    if (cart_size == 0) {
        printf("Your cart is empty.\n");
        return;
    }

    printf("Your cart contains the following items:\n");
    for (int i = 0; i < cart_size; i++) {
        printf("Product: %s, Quantity: %d\n", cart[i].name, cart[i].quantity);
    }
}

void handle_order() {
    char order_message[BUFFER_SIZE];
    char buffer[BUFFER_SIZE];

    // Sepeti gönderme
    sprintf_s(order_message, sizeof(order_message), "Order items: ");
    for (int i = 0; i < cart_size; i++) {
        sprintf_s(buffer, sizeof(buffer), "Product: %s, Quantity: %d\n", cart[i].name, cart[i].quantity);
        strcat_s(order_message, sizeof(order_message), buffer);
    }

    printf("%s\n", order_message);  // Sipariş mesajını ekrana yazdır
    // Sunucuya gönderme işlemi yapılabilir
    printf("Order processed successfully.\n");
    cart_size = 0;
    printf("Cart has been cleared after payment.\n");
}

void print_menu() {
    printf("\nChoose an option:\n");
    printf("1) Show products\n");
    printf("2) Add product to cart\n");
    printf("3) Remove product from cart\n");
    printf("4) View cart\n");
    printf("5) Make payment\n");
    printf("6) Exit\n");
}

int main() {
    char product_name[100];
    int choice;

    // Menüyü başlat
    while (1) {
        print_menu();

        // Seçenek al
        int ret = scanf_s("%d", &choice);

        // Geçerli giriş alınmadıysa, kullanıcıyı bilgilendir ve tekrar menüyü göster
        if (ret != 1) {
            printf("Invalid input. Please enter a valid option.\n");
            while (getchar() != '\n');  // Geçersiz karakterleri temizle
            continue;
        }

        switch (choice) {
        case 1:
            // Ürünleri göster
            display_products();
            break;
        case 2:
            // Sepete ürün ekle
            printf("Enter product name to add to cart: ");
            scanf_s("%s", product_name, sizeof(product_name));
            add_to_cart(product_name);
            break;
        case 3:
            // Sepetten ürün sil
            printf("Enter product name to remove from cart: ");
            scanf_s("%s", product_name, sizeof(product_name));
            remove_from_cart(product_name);
            break;
        case 4:
            // Sepeti göster
            view_cart();
            break;
        case 5:
            // Ödeme yap
            handle_order();
            break;
        case 6:
            // Çıkış
            printf("Exiting...\n");
            return 0;
        default:
            printf("Invalid choice, please try again.\n");
        }
    }

    return 0;
}
