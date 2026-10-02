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
```

## 3. a.sql / users.txt koda bağlı değil

`ECommerceService/ECommerceServer/ECommerceServer/a.sql` içinde
`users`, `products`, `cart` tablolarını tanımlayan bir şema bulunur ve
aynı dizinde `users.txt` adında düz metin bir dosya vardır, ama ne
`ECommerceServer.c` ne de `ECommerceClient.c` içinde `sqlite` veya
`fopen` ile bu dosyalara referans verilmez. Bu dosyalar koddan
bağımsız, kullanılmayan taslaklardır; kalıcı depolama **yoktur**
(bkz. [[01-mimari]] "Yapılmamış/eksik olanlar").

## 4. İstemcide aynı ürün ikinci kez eklenince miktar artmıyor

`ECommerceClient.c` içindeki `add_to_cart()`, eklenecek ürünü yalnızca
`products[]` dizisinde adına göre arar; `cart[]` içinde zaten var olup
olmadığını kontrol etmez ve miktarı her zaman `1` ile yeni bir satır
olarak ekler. Aynı ürün tekrar eklendiğinde mevcut satırın miktarı
artırılmaz, sepette aynı ürün için ayrı satırlar oluşur.
