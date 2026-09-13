/* STM32F103C6 Linker Script
 * Created by: mohamed aldreamly 
 */

ENTRY(Reset_Handler)

MEMORY
{
	FLASH(RX)	: ORIGIN = 0x08000000, LENGTH = 32K
	SRAM(RWX) 	: ORIGIN = 0x20000000, LENGTH = 10K
}

SECTIONS
{		
	.text :
	{
		*(.vectors*)
		*(.text*)
		*(.rodata)
		_E_text = . ; 
	}> FLASH
	.data :
	{
		_S_DATA = . ;
		*(.data)
		. = ALIGN(4);
		_E_DATA = . ;
	}>SRAM AT> FLASH

	.bss :
	{
		_S_BSS = . ;
		*(.bss*)
		_E_BSS = . ;
		. = ALIGN(4);
		. = . + 0x1000 ; 
		_stack_ptr = . ;
	}> SRAM
}