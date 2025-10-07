#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define BYTE "0x%02hhX"
#define WORD "0x%02hhX"
#define DWRD "0x%02hhX"

extern const char *system_ids[256];

__attribute__((packed))
struct partition {
	uint8_t attributes;
	uint8_t start_head;
	uint16_t start_sector : 6,
			 start_cylinder : 10;
	uint8_t system_id;
	uint8_t  end_head;
	uint16_t end_sector : 6,
			 end_cylinder : 10;
	uint32_t start_lba;
	uint32_t sectors;
};

struct {
	uint8_t bootstrap[440];
	uint32_t unique_id;
	uint16_t reserved;
	struct partition table[4];
	uint8_t signature[2];
}
__attribute__((packed))
MBR;

#define MBR_SIZE 512

void hexprint(uint8_t *bytes, int len)
{
	for (int i = 0; i < len; i++) {
		if (i) putchar(' ');
		printf("0x%02hhx", bytes[i]);
	}
}

void dump_partition(struct partition part)
{
	printf("Attributes: %hhx %s\n", part.attributes,
		   (part.attributes & 0x80) == 0x80 ? "(active)" : "");

	printf("System ID: 0x%hhx %s\n", part.system_id, system_ids[part.system_id]);

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

	FILE *fp = fopen(argv[1], "rb");
	if (!fp) {
		perror("Failed to open file");
		return 1;
	}

	if (fread(&MBR, MBR_SIZE, 1, fp) != 1) {
		fprintf(stderr, "Could not read the MBR\n");
		fclose(fp);
		return 1;
	}

	//for (int i = 0; i < MBR_SIZE / 16; i++) {
	//	int off = i*16;
	//	uint8_t *ptr = (uint8_t*)&MBR;
	//	hexprint(ptr + off, 16);
	//	putchar('\n');
	//}

	uint8_t valid[2] = { 0x55, 0xAA };

	printf("Signature: "BYTE" "BYTE" (%s bootsector)\n",
			MBR.signature[0], MBR.signature[1],
			!memcmp(MBR.signature, valid, 2) ? "Valid" : "Invalid");

	printf("Unique Disk ID: 0x%08x\n", MBR.unique_id);
	printf("Reserved: 0x%04hx\n", MBR.reserved);

	for (int i = 0; i < 4; i++) {
		printf("Partition %d\n", i+1);
		dump_partition(MBR.table[i]);
		puts("");
	}

	fclose(fp);
	return 0;
}
