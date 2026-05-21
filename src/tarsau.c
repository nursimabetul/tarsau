#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>

#define MAX_DOSYA_SAYISI 32
#define MAX_DOSYA_BOYUTU (200 * 1024 * 1024)

int ascii_kontrol(const char *dosya_adi);
long dosya_boyutu_al(const char *dosya_adi);
int dosya_izin_oku(const char *dosya_adi);
void arsiv_olustur(int dosya_sayisi, char *dosyalar[], char *arsiv_adi);
void arsiv_ac(const char *arsiv_adi, const char *hedef_dizin);

typedef struct
{
    char dosya_adi[256];
    long boyut;
    int izin;
} DosyaBilgisi;


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
                i++; // -o'dan sonra gelen arşiv adını atla
                break;
            }

           if(dosya_sayisi >= MAX_DOSYA_SAYISI)
            {
                printf("En fazla 32 dosya arsivlenebilir...\n");
                return 1;
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
                return 1; // -> Eksik dosya varsa program sorunsuz hata verip çık
            }
        }

        printf("Toplam dosya sayisi: %d\n", dosya_sayisi);
        printf("Toplam boyut: %ld byte\n", toplam_boyut);

        if(toplam_boyut > MAX_DOSYA_BOYUTU)
        {
            printf("Toplam dosya boyutu 200 MB'i gecemez!\n");
            return 1;
        }
        
        arsiv_olustur(dosya_sayisi, dosyalar, arsiv_dosya);
        
    }

    // Acma Islemi
    else if(strcmp(argv[1], "-a") == 0)
    {
        printf("Acma Islemi yapiliyor.\n");


        if(argc < 3)
        {
            printf("Arsiv dosyasi eksik.\n");
            return 1;
        }
        
        if(argc > 4)
        {
            printf("Fazla parametre girdiniz!\n");
            return 1;
        }
        
        char *arsiv_dosya = argv[2];
        char *hedef_dizin = NULL;

        printf("Arsiv dosyasi: %s\n", arsiv_dosya);
 
        if(argc >= 4)
        {
            hedef_dizin = argv[3];
            printf("Hedef dizin: %s\n", hedef_dizin);
        }
        else
        {
            printf("Hedef dizin girilmedi, mevcut dizine aciliyor...\n");
        }
        arsiv_ac(arsiv_dosya, hedef_dizin);    
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
        if((ch < 32 || ch > 126) &&
            ch != '\n' &&
            ch != '\r' &&
            ch != '\t')
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


int dosya_izin_oku(const char *dosya_adi)
{
    struct stat st;

    if(stat(dosya_adi, &st) != 0)
    {
        return -1;
    }

    return st.st_mode & 0777;
}


void arsiv_olustur(int dosya_sayisi, char *dosyalar[], char *arsiv_adi)
{
    FILE *arsiv = fopen(arsiv_adi, "wb");

    if(arsiv == NULL)
    {
        printf("Arsiv dosyasi olusturulamadi!\n");
        return;
    }

    DosyaBilgisi dosya_bilgisi[MAX_DOSYA_SAYISI];
    char metadata[16384] = "|"; // Taşma (Buffer Overflow) riskini önlemek için 16KB
    
    for(int i = 0; i < dosya_sayisi; i++)
    {
        long boyut = dosya_boyutu_al(dosyalar[i]);
        int izin = dosya_izin_oku(dosyalar[i]);
        
        
        strcpy(dosya_bilgisi[i].dosya_adi, dosyalar[i]);
        dosya_bilgisi[i].boyut = boyut;
        dosya_bilgisi[i].izin = izin;
        
        char temp[512];
        
        sprintf(temp,
                "%s,%o,%ld|",  //  |Dosya adı, izinler, boyut|
                dosya_bilgisi[i].dosya_adi,
                dosya_bilgisi[i].izin,
                dosya_bilgisi[i].boyut
                );

        strncat(metadata, temp, sizeof(metadata) - strlen(metadata) - 1);
    }

    long metadata_boyut = strlen(metadata);
    
    fprintf(arsiv, "%010ld", metadata_boyut);// İlk 10 bayt ASCII formatında metadata boyutu

    fwrite(metadata, 1, metadata_boyut, arsiv);

    // Dosya içeriklerini ardışık olarak yaz
    for(int i = 0; i < dosya_sayisi; i++)
    {
        FILE *fp = fopen(dosyalar[i], "rb");

        if(fp == NULL)
        {
            printf("%s acilamadi!\n", dosyalar[i]);
            continue;
        }

        int ch;

        while((ch = fgetc(fp)) != EOF)
        {
            fputc(ch, arsiv);
        }

        fclose(fp);
    }

    fclose(arsiv);

    printf("Dosyalar birlestirildi.\n");
}


int dosya_var_mi(const char *dosya_adi)
{
    return access(dosya_adi, F_OK) == 0;
}

void arsiv_ac(const char *arsiv_adi, const char *hedef_dizin)
{
    FILE *arsiv = fopen(arsiv_adi, "rb");

    
    if(arsiv == NULL)
    {
        printf("Arsiv dosyasi uygunsuz veya bozuk!\n");
        return;
    }

    // .sau uzanti kontrolu
    char *uzanti = strrchr(arsiv_adi, '.');

    if(uzanti == NULL || strcmp(uzanti, ".sau") != 0)
    {
        printf("Arsiv dosyasi uygunsuz veya bozuk!\n");
        fclose(arsiv);
        return;
    }

    // metadata boyutu oku
    char boyut_str[11];

    if(fread(boyut_str, 1, 10, arsiv) != 10)
    {
        printf("Arsiv dosyasi uygunsuz veya bozuk!\n");
        fclose(arsiv);
        return;
    }

    boyut_str[10] = '\0';

    long metadata_boyut = atol(boyut_str);

    if(metadata_boyut <= 0)
    {
        printf("Arsiv dosyasi uygunsuz veya bozuk!\n");
        fclose(arsiv);
        return;
    }

    // metadata oku
    char *metadata = malloc(metadata_boyut + 1);

    if(metadata == NULL)
    {
        fclose(arsiv);
        return;
    }

    if(fread(metadata, 1, metadata_boyut, arsiv) != metadata_boyut)
    {
        printf("Arsiv dosyasi uygunsuz veya bozuk!\n");

        free(metadata);
        fclose(arsiv);
        return;
    }
    metadata[metadata_boyut] = '\0';

    // dizin olustur
    if(hedef_dizin != NULL)
    {
        mkdir(hedef_dizin, 0755);
    }

    // metadata ayristirma
    char *parca = strtok(metadata, "|");

    while(parca != NULL)
    {
        char dosya_adi[256];
        int izin;
        long boyut;


        if(sscanf(parca,
                  "%255[^,],%o,%ld",
                  dosya_adi,
                  &izin,
                  &boyut) != 3)
        {
            printf("Arsiv dosyasi uygunsuz veya bozuk!\n");

            free(metadata);
            fclose(arsiv);
            return;
        }

        char yol[512];

        if(hedef_dizin != NULL)
        {
            sprintf(yol, "%s/%s", hedef_dizin, dosya_adi);
        }
        else
        {
            sprintf(yol, "%s", dosya_adi);
        }

        FILE *cikis = fopen(yol, "wb");

        if(cikis == NULL)
        {
            parca = strtok(NULL, "|");
            continue;
        }

        for(long i = 0; i < boyut; i++)
        {
            int ch = fgetc(arsiv);

            if(ch == EOF)
            {
                break;
            }

            fputc(ch, cikis);
        }

        fclose(cikis);

        chmod(yol, izin);

        parca = strtok(NULL, "|");
    }

    free(metadata);

    fclose(arsiv);

    printf("Dosyalar acildi.\n");
}
