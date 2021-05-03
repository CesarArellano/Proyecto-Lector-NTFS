#define nombreArchivo 0xDA
#define tamanoMFT 100
#define inicioArchivo 0x114000
#define limiteArchivo 0X46

static void mostrarDirectorioNTFS(char *imagenNTFS);
static void obtenerNombresDeArchivo(uint8_t *inicioInfo, uint8_t longitud);

void mostrarDirectorioNTFS(char *imagenNTFS) {
    FILE *img = fopen(imagenNTFS, "r");
    if (!img) {
        printf("No se pudo abrir imagen\n");
        exit(EXIT_FAILURE);
    }

    uint8_t mft[tamanoMFT];
    int index = 0;
    
    while (1) {
        uint32_t inicio = inicioArchivo + index;
        fseek(img, inicio, SEEK_SET);
        fread(mft, tamanoMFT, 1, img);
        
        if (mft[0] != limiteArchivo) {
            break;
        }

        uint8_t nombreArchivoLongitud;
        fseek(img, inicio + 0xD8, SEEK_SET);
        fread(mft, 1, 1, img);
        nombreArchivoLongitud = mft[0] * 2;

        fseek(img, inicio + nombreArchivo, SEEK_SET);
        fread(mft, nombreArchivoLongitud, 1, img);
        obtenerNombresDeArchivo(mft, nombreArchivoLongitud);        

        index = index + 0x400;
        printf("\n");
    }

    fclose(img);
}


void obtenerNombresDeArchivo(uint8_t *inicioInfo, uint8_t longitud) {
    for (int i = 0; i < longitud; i++) {
        printf("%c", inicioInfo[i]);
    }
}