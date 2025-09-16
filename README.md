# STM32F103C8T6

无名科技STM32F103C8T6带屏幕HAL库例程

# 手动修改
startup_stm32f104xb.s

bl main 改成 bl entry

Makefile和platform.ini

不要包含 -IMiddlewares/Third_Party/RealThread_RTOS_RT-Thread/include/libc/extension

STM32F103C8Tx_FLASH.ld

自己看吧，这个section哪里不一样
```
  /* The program code and other data goes into FLASH */
  .text :
  {
    . = ALIGN(4);
    *(.text)           /* .text sections (code) */
    *(.text*)          /* .text* sections (code) */
    *(.glue_7)         /* glue arm to thumb code */
    *(.glue_7t)        /* glue thumb to arm code */
    *(.eh_frame)

    KEEP (*(.init))
    KEEP (*(.fini))

    /* section information for finsh shell */
    . = ALIGN(4);
    __fsymtab_start = .;
    KEEP(*(FSymTab))
    __fsymtab_end = .;
    . = ALIGN(4);
    __vsymtab_start = .;
    KEEP(*(VSymTab))
    __vsymtab_end = .;
    . = ALIGN(4);
    
    /* section information for initial. */
    . = ALIGN(4);
    __rt_init_start = .;
    KEEP(*(SORT(.rti_fn*)))
    __rt_init_end = .;
    . = ALIGN(4);
    _etext = .;        /* define a global symbols at end of code */
  } >FLASH
```