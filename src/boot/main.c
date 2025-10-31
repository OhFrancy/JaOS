#include <efi.h>
#include <efilib.h>

EFI_STATUS 
EFIAPI efi_main(EFI_HANDLE img_handle, EFI_SYSTEM_TABLE *syst) {
	InitializeLib(img_handle, syst);

	uefi_call_wrapper(syst->ConOut->ClearScreen, 1, syst->ConOut);
	Print(L"Hello world\n");
	
	while (1) {
		asm("hlt");
	}

	return EFI_SUCCESS;
}
