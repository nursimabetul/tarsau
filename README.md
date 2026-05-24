# tarsau
Sakarya Üniversitesi Bilgisayar Mühendisliği Bölümü
Sistem Programlama Dersi
2025-2026 Bahar Dönem Projesi

Tarsau, Linux ortamında C dili ile geliştirilmiş, metin dosyalarını birleştiren (sıkıştırma yapmadan) ve açabilen basit bir komut satırı uygulamasıdır. Dosyaları birleştirerek *.sau saklar ve gerektiğinde birleştirilmiş dosya yapısını geri oluşturur.


## Derleme

```bash
make
```

---

## Kullanım

### Arşiv Oluşturma (`-b`)

```bash
./tarsau -b dosya1.txt dosya2.txt -o arsiv.sau
```

- `-o` parametresi belirtilmezse çıktı dosyası `a.sau` olarak oluşturulur.
- En fazla 32 dosya eklenebilir.
- Toplam dosya boyutu 200 MB'yi aşamaz.
- Dosya izinleri (okuma, yazma, çalıştırma) arşive kaydedilir.

### Arşiv Açma (`-a`)

```bash
./tarsau -a arsiv.sau [hedef_dizin]
```

- Hedef dizin belirtilmezse dosyalar mevcut dizine çıkarılır.
- Hedef dizin mevcut değilse otomatik olarak oluşturulur.
- Dosyalar orijinal isimleri ve izinleriyle geri yüklenir.

---

## `.sau` Dosya Yapısı

| Bölüm | Açıklama |
|---|---|
| İlk 10 bayt | Metadata bölümünün toplam uzunluğu (ASCII) |
| Metadata | `\|dosyaadi,izinler,boyut\|` formatında, `\|` ile ayrılmış kayıtlar |
| İçerik | Dosya içerikleri ardışık binary formatta, ayırıcı kullanılmadan |

Örnek metadata: `0000000077|t1,711,13|t2,664,4|`

---

## Kısıtlamalar

- Yalnızca ASCII karakter içeren metin dosyaları desteklenir.
- Maksimum 32 dosya, toplam maksimum 200 MB.

---

## Kaynak Kod

[github.com/nursimabetul/tarsau](https://github.com/nursimabetul/tarsau)

---

**Sakarya Üniversitesi — BSM308 Sistem Programlama, 2025-2026 Bahar**

