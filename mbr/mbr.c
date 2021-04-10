#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "definitions.h"


int main(int argc, char *argv[]) {
    
    if (argc < 2) {
        printf("Ingrese los parámetros correctos");
        exit(EXIT_FAILURE);
    }
    FILE *filePath = fopen(argv[1], "r");
    if (filePath == NULL) {
        printf("No se pudo abrir el archivo.\n");
        exit(EXIT_FAILURE);
    }
    fread(&mbr, 512, 1, filePath);

    /* ciclo for para buscar la información en las primeras 4 particiones */
    for (int i = 0; i < 4; i++) {
        int inicio = offsetPartitionStart + i * 0x10;
        printf("Analizando Partición: %d\n", i);
        verificaTamanoParticion(inicio + ofssetPartitionSize);
        verificaValoresCHS("Iniciando", inicio + offsetStartCHS);
        verificaValoresCHS("Terminado  ", inicio + offsetEndCHS);
        verificaInicioSector(inicio + offsetStartSector);
        verificaParticionFS(inicio + offsetPartitionDescription);
    }
}

static void verificaValoresCHS(char *str, uint32_t inicio) {
    uint8_t c, h, s;
    c = mbr[inicio];
    h = mbr[inicio + 0x1];
    s = mbr[inicio + 0x2];
    printf("\t%s valores de CHS C=%u, H=%u, S=%u\n", str, c, h, s);
}

static void verificaInicioSector(uint32_t inicio) {
    uint8_t *inicio_loc = &mbr[inicio];
    uint32_t inicio_sec = *(uint32_t *)inicio_loc;
    char *sz_str = conversionTamanoSector(inicio_sec);
    printf("\tLa partición inicia en el número de sector %u (%s en)\n", inicio_sec, sz_str);
}

static uint32_t verificaTamanoParticion(uint32_t inicio) {
    uint8_t *size_loc = &mbr[inicio];
    uint32_t size = *(uint32_t *)size_loc;
    char *sz_str = conversionTamanoSector(size);
    if (size) {
        sz_str = conversionTamanoSector(size);
        printf("\tLa partición tiene %u sectores (%s)\n", size, sz_str);
    }
    else
        puts("\tEsta partición vacía");
    return size;
}

static void verificaParticionFS(uint32_t inicio) {
    uint8_t fs_id = mbr[inicio];
    printf("\tEl ID del sistema de archivos de la partición es: %u\n", fs_id);
}

static char * conversionTamanoSector(uint32_t size) {
    double sz_kb, sz_mb, sz_gb;
    char *sz_str = malloc(10);
    sz_kb = size / 2;
    sz_mb = sz_kb / 1024;
    sz_gb = sz_mb / 1024;
    if (sz_gb >= 1)
        sprintf(sz_str, "%.2fGB", sz_gb);
    else if (sz_mb >= 1)
        sprintf(sz_str, "%.2fMB", sz_mb);
    else
        sprintf(sz_str, "%.2fKB", sz_kb);
    return sz_str;
}