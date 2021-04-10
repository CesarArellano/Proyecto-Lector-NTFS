/* Desplazamiento dentro de MBR */
#define offsetPartitionStart 0x1BE

/* Desplazamiento dentro del registro de la partición */
#define offsetStartCHS 0x1
#define offsetPartitionDescription 0x4
#define offsetEndCHS 0x5
#define offsetStartSector 0x8
#define ofssetPartitionSize 0xC

/* Clúster de lectura MBR */
static uint8_t mbr[512];

static void verificaValoresCHS(char *, uint32_t);
static uint32_t verificaTamanoParticion(uint32_t);
static void verificaInicioSector(uint32_t);
static void verificaParticionFS(uint32_t);
static char *conversionTamanoSector(uint32_t);