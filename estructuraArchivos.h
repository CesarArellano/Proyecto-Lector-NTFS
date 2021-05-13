// Donde empieza y acaba el MBR
#define MBRstart 0x1BE
#define MBRend 0x1FE
#define NTFS_TIME_OFFSET ((int64_t)(369*365+89)*24*3600*10000000)

// Typedefs Tipo de Variables NTFS
typedef unsigned char BYTE;
typedef unsigned short int WORD;
typedef unsigned int DWORD;
typedef long long LONGLONG;

struct InfoArchivo{
	char nombre[512];
	BYTE noResidente;
	LONGLONG Tamanio;
	LONGLONG Creacion;
	LONGLONG DataLength;
	LONGLONG DataOff;
};
struct DatosParticion{
	WORD *BytesPerSector;
	BYTE *SectorsPerCluster;
	BYTE *MediaDescriptor;
	WORD *SectosPerTrack;
	WORD *NumberOfHeads;
	DWORD *HiddenSectors;
	LONGLONG *TotalSectors;
	LONGLONG *DMFT;
	LONGLONG *DMMFT;
	DWORD *ClustersPerFRS;
	BYTE *ClustersPerIB;
};

struct NTFS_MFT_FILE
{
  char		szSignature[4];		// Signature "FILE"
  WORD		wFixupOffset;		// offset to fixup pattern
  WORD		wFixupSize;			// Size of fixup-list +1
  LONGLONG	n64LogSeqNumber;	// log file seq number
  WORD		wSequence;			// sequence nr in MFT
  WORD		wHardLinks;			// Hard-link count
  WORD		wAttribOffset;		// Offset to seq of Attributes
  WORD		wFlags;				// 0x01 = NonRes; 0x02 = Dir
  DWORD		dwRecLength;		// Real size of the record
  DWORD		dwAllLength;		// Allocated size of the record
  LONGLONG	n64BaseMftRec;		// ptr to base MFT rec or 0
  WORD		wNextAttrID;		// Minimum Identificator +1
  WORD		wFixupPattern;		// Current fixup pattern
  DWORD		dwMFTRecNumber;		// Number of this MFT Record
                // followed by resident and∫  								// part of non-res attributes
};

typedef struct	// if resident then + RESIDENT
{					//  else + NONRESIDENT
  DWORD	dwType;
  DWORD	dwFullLength;
  BYTE	uchNonResFlag;
  BYTE	uchNameLength;
  WORD	wNameOffset;
  WORD	wFlags;
  WORD	wID;
  
  union ATTR
  {
    struct RESIDENT
    {
      DWORD	dwLength;
      WORD	wAttrOffset;
      BYTE	uchIndexedTag;
      BYTE	uchPadding;
    } Resident;
  
    struct NONRESIDENT
    {
      LONGLONG	n64StartVCN;
      LONGLONG	n64EndVCN;
      WORD		wDatarunOffset;
      WORD		wCompressionSize; // compression unit size
      BYTE		uchPadding[4];
      LONGLONG	n64AllocSize;
      LONGLONG	n64RealSize;
      LONGLONG	n64StreamSize;
      // data runs...
    }NonResident;
  }Attr;
} NTFS_ATTRIBUTE;

typedef struct
{
  LONGLONG	n64Create;		// Creation time
  LONGLONG	n64Modify;		// Last Modify time
  LONGLONG	n64Modfil;		// Last modify of record
  LONGLONG	n64Access;		// Last Access time
  DWORD		dwFATAttributes;// As FAT + 0x800 = compressed
  DWORD		dwReserved1;	// unknown
} ATTR_STANDARD;

typedef struct
{
  LONGLONG	dwMftParentDir;            // Seq-nr parent-dir MFT entry
  LONGLONG	n64Create;                  // Creation time
  LONGLONG	n64Modify;                  // Last Modify time
  LONGLONG	n64Modfil;                  // Last modify of record
  LONGLONG	n64Access;                  // Last Access time
  LONGLONG	n64Allocated;               // Allocated disk space
  LONGLONG	n64RealSize;                // Size of the file
  DWORD		dwFlags;					// attribute
  DWORD		dwEAsReparsTag;				// Used by EAs and Reparse
  BYTE		chFileNameLength;
  BYTE		chFileNameType;            // 8.3 / Unicode
  WORD		wFilename[512];             // Name (in Unicode ?)
  
} ATTR_FILENAME; 

char *map;
struct DatosParticion DP;
struct NTFS_ATTRIBUTE *ntfsAttr;
struct NTFS_MFT_FILE ntfsMFT;