#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

// Bibliotecas agregadas
#include <curses.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#include "definitions.h"
#include "estructuraArchivos.h"

void leerArchivoBinario(char *filename);
char *mapeandoArchivo(char *filename);
void leerValoresdeParticion();
void mostrarArchivos(int);
void RefrescarPantalla(int);
void ImprimeTipoNombre(ATTR_FILENAME, struct InfoArchivo *);
void imprimirTamanoResidente(BYTE Res, LONGLONG Size);
void extraerTiempoArchivos(const int64_t time, struct tm *ts);
void imprimirTiempoArchivos(struct tm ts);
void hexEdit(int initParticion, struct InfoArchivo arch);
int leeChar();
int hex_to_int(char c);
int hex_to_decimal(char c, char d);


int main(int argc, char *argv[]) {
	int particion, opcion;
    if (argc < 2) {
        printf("Ingrese los parámetros correctos\n");
        exit(EXIT_FAILURE);
    }
    leerArchivoBinario(argv[1]);
    
    
    map = mapeandoArchivo(argv[1]);
    if (map == NULL) {
        exit(EXIT_FAILURE);
    }

    do {
        system("clear");
        printf("1.- Leer valores de particiones\n");
        printf("2.- Leer los directorios de cada partición\n");
        printf("3.- Leer archivos en hexadecimal\n");
        printf("4.- Salir del programa\n");
        printf("Ingresa una opcion-> ");
        scanf(" %d", &opcion);
        system("clear");
        switch (opcion)
        {
            case 1:
                leerValoresdeParticion();
                break;
            case 2 :
                // Valores obtenidos por el programa al leer particiones en el caso 1.
                // Particion 1: 1048576
                // Particion 2: 536870912
                printf("Ingresa la particion: ");
                scanf(" %d", &particion);
                if(particion == 1) {
                    mostrarArchivos(1048576);
                } else {
                    mostrarArchivos(536870912);
                }

                break;
            case 3:
                
                break;
            case 4 :
                exit(1);
                break;            
            default:
                printf("Opción inválida");                
                break;
        }
        printf("\nPresione Enter para continuar... ");
        __fpurge(stdin);
        getchar();
    } while(opcion != 4);    
}

void leerArchivoBinario(char *filename) {
    FILE *filePath = fopen(filename, "r");
    if (filePath == NULL) {
        printf("No se pudo abrir el archivo.\n");
        exit(EXIT_FAILURE);
    }
    fread(&mbr, 512, 1, filePath);
}

char *mapeandoArchivo(char *filename) {
    int fd = open(filename, O_RDWR);
    if (fd == -1) {
        perror("Error abriendo el archivo");
        return(NULL);
    }
    struct stat st;
    fstat(fd,&st);
    int fs = st.st_size;

    char *map = mmap(0, fs, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (map == MAP_FAILED) {
        close(fd);
        perror("Error mapeando el archivo");
        return(NULL);
    }

    return map;
}

void leerValoresdeParticion() {
    for (int i = 0; i < 4; i++){
        int inicio = offsetPartitionStart + i * 0x10;
        printf("Analizando Partición: %d\n", i + 1);
        printf("\tID: NTFS\n");
        if (!verificaTamanoParticion(inicio + ofssetPartitionSize))
            continue;
        verificaValoresCHS("Iniciando", inicio + offsetStartCHS);
        verificaValoresCHS("Terminado", inicio + offsetEndCHS);
        verificaInicioSector(inicio + offsetStartSector);
        verificaParticionFS(inicio + offsetPartitionDescription);
        verificaClusters_MFT(inicio);
    }
}

static void verificaClusters_MFT(uint32_t inicio) {
    uint8_t sectorPerCluster, clustersPerIB, mediaDescriptor;
    uint16_t sectorPerTrack, numHeads;
    uint32_t hiddenSectors, clusterPerFRS;
    long long int MFTAddress, MFTMirrorAddress;
    sectorPerCluster = mbr[inicio + 0xD];
    mediaDescriptor = mbr[inicio + 0x15];
    sectorPerTrack = mbr[inicio + 0x18];
    numHeads = mbr[inicio + 0x1A];
    hiddenSectors = mbr[inicio + 0x1C];
    MFTAddress = mbr[inicio + 0x30];
    MFTMirrorAddress = mbr[inicio + 0x38];
    clusterPerFRS = mbr[inicio + 0x40];    
    clustersPerIB = mbr[inicio + 0x44];
    printf("\tMedio fijo: %hhu\n", mediaDescriptor);
    printf("\tSectores por clúster: %u\n",sectorPerCluster);
    printf("\tSectores por pista: %u\n",sectorPerTrack);
    printf("\tNúmero de cabezas: %u\n",numHeads);
    printf("\tDirección MFT: %lld\n",MFTAddress);
    printf("\tDirección Espejo MFT: %lld\n",MFTMirrorAddress);
    printf("\tClusters Per File Record Segment: %u\n",clusterPerFRS);
    printf("\tClusters Per Index Buffer: %u\n", clustersPerIB);
}

static void verificaValoresCHS(char *str, uint32_t inicio) {
    uint8_t c, h, s;
    c = mbr[inicio + 0x2];
    h = mbr[inicio];
    s = mbr[inicio + 0x1];
    printf("\t%s valores de CHS C = %u, H = %u, S = %u\n", str, c, h, s);
}

static void verificaInicioSector(uint32_t inicio) {
    uint8_t *inicio_loc = &mbr[inicio];
    uint32_t inicio_sec = *(uint32_t *)inicio_loc;
    char *sz_str = conversionTamanoSector(inicio_sec);
    printf("\tTamaño del Sector: %u (%s)\n", inicio_sec, sz_str);
}

static uint32_t verificaTamanoParticion(uint32_t inicio) {
    uint8_t *size_loc = &mbr[inicio];
    uint32_t size = *(uint32_t *)size_loc;
    char *sz_str = conversionTamanoSector(size);
    if (size) {
        sz_str = conversionTamanoSector(size);
        printf("\tTotal de sectores: %u (%s)\n", size, sz_str);
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

void mostrarArchivos(int initParticion){
	int flag = 0, flag2 = 0, flagF = 0;
	struct tm ts;
	BYTE Res = 0, BytesLength, BytesOffset, Bytes;
	WORD DataOff, DataClust;
	LONGLONG Size;
	struct InfoArchivo a[50];
	NTFS_ATTRIBUTE Attr, DataRun; 
	ATTR_STANDARD AttrStd; 
	ATTR_FILENAME AttrFN; 
	char nomsimple[256]; 
    DP.BytesPerSector = (WORD*)&map[initParticion + 0x0B];
    DP.DMFT = (LONGLONG*) &map[initParticion + 0x30];
    DP.SectorsPerCluster=(BYTE*)&map[initParticion + 0x0D];
	char *dir = &map[initParticion + (*DP.BytesPerSector)*(*DP.DMFT)*(*DP.SectorsPerCluster)];
	int k, i, j, m;
	m = k = i = j = 0;
    printf("\n\nNombre\t\tTipo\tResidente\tTamaño\t\tCreado\n");
    do {
        memcpy((void*)&ntfsMFT,&dir[m],sizeof(struct NTFS_MFT_FILE));
        j = m + ntfsMFT.wAttribOffset;
        flag2 = 1;
        do {
            memcpy((void*)&Attr,&dir[j],sizeof(NTFS_ATTRIBUTE));
            switch(Attr.dwType) {
                case 0x30:
                    printf("\n");
                    flag = 1;
                    flagF = 1;
                    if(Attr.uchNonResFlag == 0x0) {
                        memcpy((void*)&AttrFN,&dir[j+Attr.Attr.Resident.wAttrOffset],sizeof(ATTR_FILENAME));
                        ImprimeTipoNombre(AttrFN, &a[k]);
                    }
                    k++;
                    break;
                case 0x80:
                    Res = Attr.uchNonResFlag;    
                    if(Attr.uchNonResFlag == 0x0) {
                        Size = AttrFN.n64RealSize;
                        a[k-1].DataOff = (*DP.BytesPerSector)*(*DP.DMFT)*(*DP.SectorsPerCluster) + j + Attr.Attr.Resident.wAttrOffset;
                        a[k-1].DataLength = Attr.Attr.Resident.dwLength;
                        Size = a[k-1].DataLength;
                    } else {
                        memcpy((void*)&Bytes,&dir[j+Attr.Attr.NonResident.wDatarunOffset],sizeof(BYTE));
                        BytesLength = Bytes%16;
                        BytesOffset = (Bytes - BytesLength)/16;
                        memcpy((void*)&DataClust,&dir[j + Attr.Attr.NonResident.wDatarunOffset + 1],BytesLength*sizeof(BYTE));
                        memcpy((void*)&DataOff,&dir[j + Attr.Attr.NonResident.wDatarunOffset + BytesLength*sizeof(BYTE) + 1],BytesOffset*sizeof(BYTE));
                        a[k-1].DataLength = DataClust*(*DP.BytesPerSector)*(*DP.SectorsPerCluster);
                        a[k-1].DataOff = DataOff*(*DP.BytesPerSector)*(*DP.SectorsPerCluster);
                        Size = Attr.Attr.NonResident.n64RealSize;
                    }
                    if (flag == 1) {                    
                        imprimirTamanoResidente(Res, Size);
                        flag = 0;
                        flag2 = 0;
                    }
                    break;
            }

            j = j + Attr.dwFullLength;
        } while(Attr.dwFullLength);
        
        if(flag == 1 && flag2 == 1){
            printf("Sí\t\t0\t\t");
        }

        if(flagF == 1){
            extraerTiempoArchivos(AttrFN.n64Create, &ts);
            imprimirTiempoArchivos(ts);
        }
        
        flag = 0;
        flag2 = 0;
        flagF = 0;
        m = m + ntfsMFT.dwAllLength;

    } while(ntfsMFT.dwAllLength);
}

void ImprimeTipoNombre(ATTR_FILENAME AttrFN, struct InfoArchivo *arch){
    
	for(int i=0; i<AttrFN.chFileNameLength;i++){
		arch->nombre[i]=AttrFN.wFilename[i];
		arch->nombre[i+1]=0;
	
	}
	for(int i=0; i<AttrFN.chFileNameLength;i++){
		printf("%c", AttrFN.wFilename[i]);
		if ( i > 8){
			break;
        }
	}
	printf("\t");
	if (AttrFN.chFileNameLength < 8)
		printf("\t");
	if (AttrFN.dwFlags==0x0006){
		printf("Sys\t");

	}else if(AttrFN.dwFlags==0x0020 || AttrFN.dwFlags==0x0080){

		printf("Arch\t");

	}else if(AttrFN.dwFlags==0x10000006){
		printf("Dir\t");
	}
	else if(AttrFN.dwFlags==0x20000006){
		printf("Sys\t");
	}
	else{
		printf("Arch\t");
	}
}

void imprimirTamanoResidente(BYTE Res, LONGLONG Size){
	if(Res)
		printf("No\t\t");
	else
		printf("Sí\t\t");
	
	printf("%lld\t\t", Size);
}

void extraerTiempoArchivos(const int64_t time, struct tm *ts){
	int64_t t = (int64_t)(time - NTFS_TIME_OFFSET);
	t /= 10000000;
	struct tm tr = *localtime(&t);
	memcpy(ts, &tr, sizeof(struct tm));
}

void imprimirTiempoArchivos(struct tm ts){
	int year, month, day, hour, min, sec;
	year = 1900 + ts.tm_year;
	printf("%4d-%02d-%02d %02d:%02d:%02d", year, ts.tm_mon, ts.tm_mday, ts.tm_hour, ts.tm_min, ts.tm_sec);
}

/**
 * HEX EDITOR
 * */

char *hazLinea(char *base, int dir) {
	char linea[100]; // La linea es mas pequeña
	int o=0;
	// Muestra 16 caracteres por cada linea
	o += sprintf(linea,"%08x ",dir); // offset en hexadecimal
	for(int i=0; i < 4; i++) {
		unsigned char a,b,c,d;
		a = base[dir+4*i+0];
		b = base[dir+4*i+1];
		c = base[dir+4*i+2];
		d = base[dir+4*i+3];
		o += sprintf(&linea[o],"%02x %02x %02x %02x ", a, b, c, d);
	}
	for(int i=0; i < 16; i++) {
		if (isprint(base[dir+i])) {
			o += sprintf(&linea[o],"%c",base[dir+i]);
		}
		else {
			o += sprintf(&linea[o],".");
		}
	}
	sprintf(&linea[o],"\n");

	return(strdup(linea));
}

int leeChar() {
  int chars[5];
  int ch,i=0;
  nodelay(stdscr, TRUE);
  while((ch = getch()) == ERR); /* Espera activa */
  ungetch(ch);
  while((ch = getch()) != ERR) {
    chars[i++]=ch;
  }
  /* convierte a numero con todo lo leido */
  int res=0;
  for(int j=0;j<i;j++) {
    res <<=8;
    res |= chars[j];
  }
  return res;
}