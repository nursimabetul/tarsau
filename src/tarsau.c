#include <stdio.h>
#include <string.h>

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
