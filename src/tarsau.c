#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>

#define MAX_DOSYA_SAYISI 32
#define MAX_DOSYA_BOYUTU (200 * 1024 * 1024)

int ascii_kontrol(const char *dosya_adi);
long dosya_boyutu_al(const char *dosya_adi);

int main(int argc, char *argv[])
{
    if(argc < 2)
    {
        printf("\nKullanim:\n");
        printf("  Birlestirme: tarsau -b dosyalar -o arsiv.sau\n");
        printf("  Acma       : tarsau -a arsiv.sau [dizin]\n\n");
        return 1;
    }

    // Birleştirme Islemi
    if(strcmp(argv[1], "-b") == 0)
    {
        char *arsiv_dosya = "a.sau";

        // -o kontrolü (okuma)
        for(int i = 2; i < argc; i++)
        {
            if(strcmp(argv[i], "-o") == 0 && i + 1 < argc)
            {
                arsiv_dosya = argv[i + 1];
            }
        }

        printf("Birlestirme Islemi yapiliyor.\n");
        printf("Arsiv dosyasi: %s\n", arsiv_dosya);
        
        char *dosyalar[32];
        int dosya_sayisi = 0;

        for(int i = 2; i < argc; i++)
        {
            if(strcmp(argv[i], "-o") == 0)
            {
                break;
            }

            dosyalar[dosya_sayisi] = argv[i];
            dosya_sayisi++;
        }

        printf("Birlestirilecek Dosyalar:\n");
        long toplam_boyut = 0;

        for(int i = 0; i < dosya_sayisi; i++)
        {
            if(access(dosyalar[i], F_OK) == 0)
            {
                printf("  %s bulundu.\n", dosyalar[i]);

                if(!ascii_kontrol(dosyalar[i]))
                {
                    printf("%s giris dosyasinin formati uyumsuz!\n", dosyalar[i]);
                    return 1;
                }

                long boyut = dosya_boyutu_al(dosyalar[i]);

                if(boyut < 0)
                {
                    printf("%s okunamadi!\n", dosyalar[i]);
                    return 1;
                }

                toplam_boyut += boyut;

                printf("  Boyut: %ld byte\n", boyut);
            }
            else
            {
                printf("  %s bulunamadi!\n", dosyalar[i]);
            }
        }

        printf("Toplam dosya sayisi: %d\n", dosya_sayisi);
        printf("Toplam boyut: %ld byte\n", toplam_boyut);

        if(toplam_boyut > MAX_DOSYA_BOYUTU)
        {
            printf("Toplam dosya boyutu 200 MB'i gecemez!\n");
            return 1;
        }
        
        
        
    }

    // Acma Islemi
    else if(strcmp(argv[1], "-a") == 0)
    {
        printf("Acma Islemi yapiliyor.\n");

        if(argc >= 3)
        {
            printf("Arsiv dosyasi: %s\n", argv[2]);
        }

        if(argc >= 4)
        {
            printf("Hedef dizin: %s\n", argv[3]);
        }
    }

    else
    {
        printf("Gecersiz parametre\n");
    }

    return 0;
}


int ascii_kontrol(const char *dosya_adi)// Dosya türü text ise 1 değilse 0 döndürülecek
{
    FILE *fp = fopen(dosya_adi, "rb");

    if(fp == NULL)
    {
        return 0;
    }

    int ch;

    while((ch = fgetc(fp)) != EOF)
    {
        if(ch > 127)
        {
            fclose(fp);
            return 0;
        }
    }

    fclose(fp);

    return 1;
}


long dosya_boyutu_al(const char *dosya_adi)
{
    struct stat st;

    if(stat(dosya_adi, &st) != 0)
    {
        return -1;
    }

    return st.st_size;
}
