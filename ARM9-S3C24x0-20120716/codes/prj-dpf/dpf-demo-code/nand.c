#define S3C2440//#define S3C2410#ifdef S3C2440#define NFCONF		(*(volatile unsigned int *)(0x4E000000))#define NFCONT		(*(volatile unsigned int *)(0x4E000004))#define NFCMMD		(*(volatile unsigned char *)(0x4E000008))#define NFADDR		(*(volatile unsigned char *)(0x4E00000C))#define NFDATA		(*(volatile unsigned char *)(0x4E000010))#define NFSTAT		(*(volatile unsigned char *)(0x4E000020))void nand_init(void){	// TACLS [13:12]	// TWRPH0 [10:8]	// TWRPH1 [6:4]	// AdvFlash (Read only) [3] 0: Support   256 or   512 byte/page NAND flash memory	// PageSize (Read only) [2]	1: 512 Bytes/page	// AddrCycle (Read only) [1]	1: 4 address cycle	// BusWidth (R/W) [0]		0: 8-bit bus	NFCONF = 3<<12 | 7<<8 | 7<<4;		// Reg_nCE [1]		0: Force nFCE to low (Enable chip select)	// MODE [0]		1: NAND flash controller enable		NFCONT = 1<<0 | 0<<1;		return;}#endif#ifdef S3C2410#define NFCONF	(*(volatile unsigned int *)0x4e000000)//#define NFCONT	(*(volatile unsigned int *)0x4e000004)#define NFCMMD	(*(volatile unsigned char *)0x4e000004)#define NFADDR	(*(volatile unsigned char *)0x4e000008)#define NFDATA	(*(volatile unsigned char *)0x4e00000c)#define NFSTAT	(*(volatile unsigned char *)0x4e000010)void nand_init(void){	//NFCONF = 0x1400;	//NFCONT = 0x01;		// TWRPH0 [6:4] TWRPH0 duration setting value (0~7)	// Duration = HCLK * (TWRPH0 + 1)	NFCONF |= 1<<6;		// Enable/Disable [15] NAND Flash controller enable/disable	// 0 = Disable NAND Flash Controller	// 1 = Enable NAND Flash Controller	NFCONF |= 1<<15;		// TACLS [10:8] CLE & ALE duration setting value (0~7)	// Duration = HCLK * (TACLS + 1)	//NFCONF |= 1<<8;		// NAND Flash Memory	chip enable  	//	[11]   NAND flash memory nFCE control  	//	0 : NAND flash nFCE = L (active)	//	1 : NAND flash nFCE = H (inactive)  	//	(After auto-boot, nFCE will be inactive.)	  	NFCONF &= ~(1<<11);}#endifint nand_read_id(void){		int id = 0;		// see k9f1208.pdf  P27	// write CMD = 0x90	NFCMMD = 0x90;		// write ADDR = 0x00	NFADDR = 0x00;				id = NFDATA << 24 | NFDATA << 16 | NFDATA << 8 | NFDATA << 0;		return id;}void nand_read_page(int addr, char * buf){	int i = 0;		// see k9f1208.pdf  P22	// write CMD = 0x00	NFCMMD = 0x00;		// write 4 ADDR = 0x00	NFADDR = addr & 0xFF;				// A0-A7	NFADDR = (addr >> 9) & 0xFF;			// A9-A16	NFADDR = (addr >> 17) & 0xFF;			// A17-A24	NFADDR = (addr >> 25) & 0x1;			// A25		// wait for Ready	while ((NFSTAT & 0x1) == 0)		;		// read data	for (i = 0; i < 512; i++)		*buf++ = NFDATA;			return;}void nand_read(int nand_addr, int sdram_addr, int size){		int pages;	int i;	char * buf = (char *)sdram_addr;	if (size <= 0)		return;	// get how many pages to be read	pages = (size - 1) / 512 + 1;	for (i = 0; i < pages; i++)		nand_read_page(nand_addr+i*512, buf+i*512);}