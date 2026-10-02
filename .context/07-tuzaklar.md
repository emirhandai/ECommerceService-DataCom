# Tuzaklar

Bu liste `ECommerceServer.c` ve `ECommerceClient.c` kaynak kodu
okunarak doğrulanmıştır; spekülasyon içermez. Bkz. [[01-mimari]].

## 1. Client, sunucuya gerçekten bağlanmıyor

`ECommerceClient.c` winsock2 header'ını include eder ve `ws2_32.lib`'i
linkler, ama `socket`, `connect`, `send`, `recv`, `WSAStartup`
çağrılarından HİÇBİRİ kullanılmaz. `handle_order()` içindeki
"Sunucuya gönderme işlemi yapılabilir" yorumu yanıltıcıdır — hiçbir
ağ trafiği üretilmez, sipariş yalnızca konsola yazdırılıp sepet
sıfırlanır. "Client/Server" sistemi çalıştırıldığında ikisi birbirinden
habersiz, tamamen bağımsız iki süreçtir. Entegrasyon eklenecekse önce
bu kopukluk giderilmelidir.

## 2. Sunucuda thread'e stack değişkeninin adresi veriliyor (race condition)

`main()` döngüsünde:
```c
client_socket = accept(...);
_beginthread(handle_client, 0, (void*)&client_socket);
