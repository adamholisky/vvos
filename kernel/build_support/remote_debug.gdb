set disassembly-flavor att

file /usr/local/osdev/versions/v/build/versionv.bin
directory /usr/local/osdev/versions/v/src

target remote localhost:5894

define qq
  set confirm off
  quit
end

break _asm_kernel_start
#break stage5
#break interrupts.c:107 if interrupt_num == 0x30
#break interrupts.c:107 if interrupt_num == 0x31 
#break interrupts.c:107 if interrupt_num == 0x32 
#break interrupts.c:131