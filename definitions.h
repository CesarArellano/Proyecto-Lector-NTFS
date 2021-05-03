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
static void verificaClusters_MFT(uint32_t);


// DWORD	A 32-bit unsigned integer
// WORD	 A 16-bit unsigned integer.
// LONGLONG	A 64-bit signed integer. 
// Referencia https://docs.microsoft.com/en-us/windows/win32/winprog/windows-data-types?redirectedfrom=MSDN