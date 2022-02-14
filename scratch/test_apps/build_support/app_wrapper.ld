ENTRY(__start)

SECTIONS
{
	. = 0x00000000;

	_app_start = .;

	.text
	{
		*(.text)
	}

	.rodata 
	{
		*(.rodata)
	}

	.data
	{
		*(.data)
		module_data_start = .;
		INCLUDE test_apps/build_temp/apps_insert.ld
		module_data_end = .;
	}

	.bss
	{
		*(COMMON)
		*(.bss)
	}

	_app_end = .;
}
