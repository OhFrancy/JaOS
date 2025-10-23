set architecture i8086 	
display/i $pc
target remote localhost:1234
hbreak *0x7C00
continue



