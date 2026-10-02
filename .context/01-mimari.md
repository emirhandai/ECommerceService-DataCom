# Mimari

## Genel bakış

Repo, Visual Studio (v143 toolset, Windows 10 SDK) ile derlenen iki ayrı
**C** konsol uygulaması içerir. Her biri kendi `.sln`/`.vcxproj`'una sahiptir,
ortak bir kütüphane veya paylaşılan proje yoktur:

- `ECommerceService/ECommerceServer/` — `ECommerceServer.c`
- `ECommerceService/ECommerceClient/` — `ECommerceClient.c`

İsimlerine rağmen bu ikisi **birbirine bağlı bir client-server sistemi
değildir** — bkz. [[07-tuzaklar]] madde 1.

## ECommerceServer.c

- Winsock2 (`ws2_32.lib`) tabanlı, TCP, `PORT 8080`, `INADDR_ANY` üzerinde
  dinleyen çok-iş-parçacıklı bir soket sunucusu.
- `main()`: `WSAStartup` → `socket` → `bind` → `listen(backlog=MAX_CLIENTS=10)`
  → sonsuz `accept()` döngüsü. Her kabul edilen bağlantı için
  `_beginthread(handle_client, 0, (void*)&client_socket)` ile yeni bir
  iş parçacığı başlatılır (senkronizasyon/mutex yok).
- `handle_client()`: bağlanan istemciye önce sabit bir karşılama mesajı,
  ardından statik `products[]` dizisindeki 5 ürünü satır satır `send()`
  ile gönderir. Sonra sonsuz bir `recv()` döngüsüne girer; her gelen
  mesajı konsola yazdırır (`"Received order: %s\n"`) ve sabit bir
  onay metniyle cevap verir. Gerçek bir sipariş/sepet mantığı YOK —
  sunucu tarafında yalnızca metni ekrana basar.
- Veri modeli: `Product { int id; char name[100]; float price; }`,
  bellek-içi sabit dizi (`products[]`), kalıcı depolama (dosya/DB) yok.

## ECommerceClient.c

- Winsock2 header'ı include edilmiş ve `ws2_32.lib` linklenmiş olsa da,
  **hiçbir soket/ağ çağrısı (`socket`, `connect`, `send`, `recv`,
  `WSAStartup`) kullanılmaz.** Client tamamen yerel/offline bir
  konsol menüsüdür.
- Aynı `Product products[]` dizisi (5 sabit ürün) client tarafında da
  ayrıca tanımlı — sunucudakiyle birebir aynı veriler elle
  kopyalanmış durumda, tek kaynak yok.
- Sepet durumu: `CartItem cart[MAX_CART_ITEMS=10]` + `cart_size` —
  global, süreç-içi, kalıcı değil.
- Menü döngüsü (`main`): `scanf_s` ile seçim alınır, `switch` ile
  dallanır:
  1. `display_products()` — ürünleri listeler.
  2. `add_to_cart(name)` — isme göre `products[]` içinde arar, bulursa
     `cart[]`'a ekler (miktar her zaman 1 ile başlar, ikinci eklemede
     miktar artırılmaz — ayrı satır olarak eklenmeye çalışılır, bkz.
     [[07-tuzaklar]]).
  3. `remove_from_cart(name)` — bulduğu ilk eşleşmeyi silip diziyi
     sola kaydırır.
  4. `view_cart()` — sepeti yazdırır.
  5. `handle_order()` — sepet içeriğini tek bir metne birleştirir,
     konsola yazdırır, `cart_size = 0` ile sepeti temizler. Yorumda
     "Sunucuya gönderme işlemi yapılabilir" denir ama bu adım
     **yapılmaz** — sunucuya hiçbir veri gitmez.
  6. Çıkış.

## Derleme / çalıştırma

- Platform: Windows, Visual Studio 2022 (v143), Windows 10 SDK.
- Her proje bağımsız derlenir (`ECommerceServer.sln`,
  `ECommerceClient.sln`); birini çalıştırmak diğerini beklemez çünkü
  aralarında gerçek bir ağ bağlantısı yok.
- `ws2_32.lib` her iki projede de `#pragma comment(lib, "ws2_32.lib")`
  ile linklenir (proje ayarlarında ekstra linker girdisi aramaya
  gerek yok).

## Yapılmamış / eksik olanlar

- Client'ın sunucuya gerçekten bağlanıp sipariş göndermesi.
- Kalıcı veri deposu (ürün/sipariş veritabanı, dosya).
- Kimlik doğrulama, oturum, çoklu kullanıcı ayrımı.
- Sunucu tarafında `_beginthread` dönüş değeri kontrolü ve iş
  parçacığı/soket kapatma senkronizasyonu.
