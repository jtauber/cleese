global loader           ; making entry point visible to linker
extern kmain             ; main is defined elsewhere

; setting up the Multiboot header - see GRUB docs for details
MODULEALIGN equ  1<<0                   ; align loaded modules on page boundaries
MEMINFO     equ  1<<1                   ; provide memory map
FLAGS       equ  MODULEALIGN | MEMINFO  ; this is the Multiboot 'flag' field
MAGIC       equ    0x1BADB002           ; 'magic number' lets bootloader find the header
CHECKSUM    equ -(MAGIC + FLAGS)        ; checksum required

section .text
align 4
MultiBootHeader:
   dd MAGIC
   dd FLAGS
   dd CHECKSUM

; reserve initial kernel stack space
STACKSIZE equ 0x4000                  ; that's 16k.

loader:
   mov esp, stack+STACKSIZE           ; set up the stack
   push eax                           ; pass Multiboot magic number
   push ebx                           ; pass Multiboot info structure

   call kmain                         ; call kernel proper
   hlt                                ; halt machine should kernel return

section .bss
align 32
stack:
   resb STACKSIZE                     ; reserve 16k stack on a quadword boundary