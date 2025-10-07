#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mbrdump.h"

void dump_assembly(uint8_t *bytes, int len)
{
	FILE* tmp = fopen("/tmp/mbr_bootstrap.bin", "wb");
    if (!tmp)
		return;

	if (fwrite(bytes, 1, len, tmp) != len) {
		fclose(tmp);
		return;
	}

	fclose(tmp);

	system("objdump -D -Mintel,i8086,addr16,data16 -b binary -m i386 /tmp/mbr_bootstrap.bin");
}

void dump_partition(struct partition part)
{
	printf("Attributes: %hhx %s\n", part.attributes,
		   (part.attributes & 0x80) == 0x80 ? "(active)" : "");

	printf("System ID: 0x%hhx\n", part.system_id);

	printf("Possible Partition Type:\n");
	for (const char **id = system_ids[part.system_id]; *id; id++)
		puts(*id);

	printf("Starting head: %d\n", part.start_head);
	printf("Starting cylinder: %d\n", part.start_cylinder);
	printf("Starting sector: %d\n", part.start_sector);

	printf("Ending head: %d\n", part.end_head);
	printf("Ending cylinder: %d\n", part.end_cylinder);
	printf("Ending sector: %d\n", part.end_sector);

	printf("Starting LBA: %d\n", part.start_lba);
	printf("Sector count: %d\n", part.sectors);
}

int main(int argc, char **argv)
{
	if (argc != 2) {
		fprintf(stderr, "Usage: %s file\n", argv[0]);
		return 1;
	}

	struct mbr MBR;

	FILE *fp = fopen(argv[1], "rb");
	if (!fp) {
		perror("Failed to open file");
		return 1;
	}

	if (fread(&MBR, sizeof(MBR), 1, fp) != 1) {
		fprintf(stderr, "Could not read the MBR\n");
		fclose(fp);
		return 1;
	}

	uint8_t valid[2] = { 0x55, 0xAA };
	printf("Signature: 0x%02hhX 0x%02hhX (%s bootsector)\n",
			MBR.signature[0], MBR.signature[1],
			!memcmp(MBR.signature, valid, 2) ? "Valid" : "Invalid");

	printf("Unique Disk ID: 0x%08x\n", MBR.unique_id);
	printf("Reserved: 0x%04hx\n", MBR.reserved);

	for (int i = 0; i < 4; i++) {
		printf("Partition %d\n", i+1);
		dump_partition(MBR.table[i]);
		puts("");
	}

	printf("Bootstrap code:\n");
	dump_assembly((uint8_t *)MBR.bootstrap, 440);

	fclose(fp);
	return 0;
}
