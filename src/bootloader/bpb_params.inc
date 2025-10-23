bpb_oem:                    db "MSWIN4.1"          ; Standard OEM name
bpb_bytes_per_sector:       dw 512
bpb_sectors_per_cluster:    db 1
bpb_reserved_sectors:       dw 32
bpb_fat_count:              db 2
bpb_dir_entries_count:      dw 0                  
bpb_total_sectors:          dw 0                  
bpb_media_descriptor_type:  db 0xF8 
bpb_sectors_per_fat:        dw 0                  
bpb_sectors_per_track:      dw 63
bpb_heads:                  dw 16                  
bpb_hidden_sectors:         dd 0
bpb_large_sector_count:     dd 204800              

fat32_sectors_per_fat:      dd 1600                
fat32_flags:                dw 0
fat32_fat_version_number:   dw 0
fat32_rootdir_cluster:      dd 2                   
fat32_fsinfo_sector:        dw 1                   
fat32_backup_boot_sector:   dw 6                   
fat32_reserved:             times 12 db 0        
ebr_drive_number:           db 0x80                ; 0x80 = HDD
                            db 0                  
ebr_signature:              db 0x29               
ebr_volume_id:              dd 0x12345678 
ebr_volume_label:           db 'JAOS       '       ; 11 bytes padded 
ebr_system_id:              db 'FAT32   '         

