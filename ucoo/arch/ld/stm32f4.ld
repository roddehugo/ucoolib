MEMORY
{
	rom (rx) : ORIGIN = 0x08000000, LENGTH = 1024K
	ccm (rwx) : ORIGIN = 0x10000000, LENGTH = 64K
	/* SRAM1: 112K
	 * SRAM2: 16K */
	ram (rwx) : ORIGIN = 0x20000000, LENGTH = 128K
}

REGION_ALIAS("nvrammem", rom);
REGION_ALIAS("heapmem", ram);

INCLUDE common.ld

PROVIDE(_stack = ORIGIN(ccm) + LENGTH(ccm));
