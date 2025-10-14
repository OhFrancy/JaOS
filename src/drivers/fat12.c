/*
 * Implementation of the File system (FAT12) driver, so that we can manage files
 *
 *         Copyright (c) 2025 Francesco Lauro. All rights reserved.
 *         SPDX-License-Identifier: MIT
 */

#include <stdio.h>
#include <log.h>
#include <drivers/fat.h>

enum FAT {
	FAT12 = (1 << 0),
	FAT16 = (1 << 1),
	FAT32 = (1 << 2),
};

typedef struct {
	uint16_t bytes_per_sector;
	uint8_t sectors_per_cluster; 
	uint16_t sectors_reserved;
	uint8_t fat_tables;
	uint16_t rootdir_entries;
	uint16_t sectors_count;
	uint8_t media_type;
	uint16_t sectors_per_fat;
	uint16_t sectors_per_track;
	uint16_t heads_count;
	uint32_t hidden_sectors;
	uint32_t large_sectors;

	uint32_t rootdir_sectors;
	uint32_t f_data_sector;
	uint32_t f_rootdir_sector;

} __attribute__((packed)) fat12_info_t;

static void fat12_info_fill(fat12_info_t *fat12_i);

void fat12_init()
{
	floppy_init();

	fat12_info_t fat12_i;
	fat12_info_fill(&fat12_i);

	uint8_t data_sector[512];
	floppy_read(fat12_i.f_rootdir_sector, data_sector);
	for (int i = 0; i < 512; ++i)
		printf("%c", GRAY, data_sector[i]);
	
}

static void fat12_info_fill(fat12_info_t *fat12_i)
{
	if (fat12_i == NULL) {
		klog(ERROR, "FAT12", "couldn't fill BPB struct while initializing the FAT12 driver.");
		return;
	}
	(fat12_i->bytes_per_sector) = 512;
	(fat12_i->sectors_per_cluster) = 1;
	(fat12_i->sectors_reserved) = 1;
	(fat12_i->fat_tables) = 2;
	(fat12_i->rootdir_entries) = 224;
	(fat12_i->sectors_count) = 2880;
	(fat12_i->media_type) = 0xF0;
	(fat12_i->sectors_per_fat) = 9;
	(fat12_i->sectors_per_track) = 18;
	(fat12_i->heads_count) = 2;
	(fat12_i->hidden_sectors) = 0;
	(fat12_i->large_sectors) = 0;

	(fat12_i->rootdir_sectors) = ((fat12_i->rootdir_entries * 32) + (fat12_i->bytes_per_sector - 1)) / fat12_i->bytes_per_sector;
	(fat12_i->f_data_sector) = fat12_i->sectors_reserved + fat12_i->rootdir_sectors + (fat12_i->fat_tables * fat12_i->sectors_per_fat);
	(fat12_i->f_rootdir_sector) = fat12_i->f_data_sector - fat12_i->rootdir_sectors;

}
