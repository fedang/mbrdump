#ifndef MBRDUMP_H
#define MBRDUMP_H

#include <stdint.h>

// TODO: Bitfields are unreliable

struct partition {
	uint8_t  attributes;
	uint8_t  start_head;
	uint16_t start_sector : 6,
			 start_cylinder : 10;
	uint8_t  system_id;
	uint8_t  end_head;
	uint16_t end_sector : 6,
			 end_cylinder : 10;
	uint32_t start_lba;
	uint32_t sectors;
} __attribute__((packed));

struct mbr {
	uint8_t bootstrap[440];
	uint32_t unique_id;
	uint16_t reserved;
	struct partition table[4];
	uint8_t signature[2];
} __attribute__((packed));

extern const char *system_ids[][256];

#endif
