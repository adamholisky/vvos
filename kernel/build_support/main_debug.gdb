set disassembly-flavor att

tui enable
tui new-layout main {-horizontal { {-horizontal src 3 asm 2} 4 cmd 1 } 5  regs 1 } 1 status 0
set tui border-kind acs
layout main
winheight src -5 
focus cmd

file /usr/local/osdev/versions/v/build/versionv.bin
directory /usr/local/osdev/versions/v/kernel

define qq
  set confirm off
  quit
end

define ds
  set $i = 10
  if $argc == 1
    set $i = $arg0
  end

  eval "x/%d%c $esp", $i, 'x'
end

define dm 
  set $i = 10
  if $argc == 2
    set $i = $arg1
  end

  eval "x/%d%c %d", $i, 'x', $arg0
end

#display_struct <struct_name> <memory address>
define display_struct
  if $argc == 2 
    p *(($arg0 *)($arg1))
  end
end

#break _asm_kernel_start
#break stage5
#break interrupts.c:107 if interrupt_num == 0x30
#break interrupts.c:107 if interrupt_num == 0x31 
#break *0x0010aaaa
#break debug.cpp:30
#break debug.cpp:39
#break sched_yield_to_asm
break interrupt_0x2C
break interrupt_0x21

target remote localhost:5894