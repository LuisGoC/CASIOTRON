#---gdb settings
set pagination off
set confirm off
set verbose off
set height 0
set width 0

#---connect and load program
target remote localhost:3333
mon arm semihosting enable
load
mon reset halt
break main
continue

define reset
    mon reset halt
end


