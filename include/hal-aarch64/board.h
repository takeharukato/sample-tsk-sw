/* -*- mode: c; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2014 Takeharu KATO                                      */
/*                                                                    */
/*  Board specific definitions                                        */
/*                                                                    */
/**********************************************************************/
#if !defined(_HAL_BOARD_H)
#define  _HAL_BOARD_H 

#include <hal/asm-macros.h>

/* QEmu virt memory map
 * see hw/arm/virt.c in QEmu source tree.
 */
#define QEMU_VIRT_FLASH_BASE                 (0x0000000000)
#define QEMU_VIRT_FLASH_SIZE                 (0x0008000000)

#define QEMU_VIRT_CPUPERIPHS_BASE            (0x0008000000)
#define QEMU_VIRT_CPUPERIPHS_SIZE            (0x0000020000)

#define QEMU_VIRT_GIC_DIST_BASE              (0x0008000000)
#define QEMU_VIRT_GIC_DIST_SIZE              (0x0000010000)

#define QEMU_VIRT_GIC_CPU_BASE               (0x0008010000)
#define QEMU_VIRT_GIC_CPU_SIZE               (0x0000010000)

#define QEMU_VIRT_GIC_V2M_BASE               (0x0008020000)
#define QEMU_VIRT_GIC_V2M_SIZE               (0x0000001000)

#define QEMU_VIRT_GIC_ITS_BASE               (0x0008080000)
#define QEMU_VIRT_GIC_ITS_SIZE               (0x0000020000)

#define QEMU_VIRT_GIC_REDIST_BASE            (0x00080A0000)
#define QEMU_VIRT_GIC_REDIST_SIZE            (0x0000F60000)

#define QEMU_VIRT_UART_BASE                  (0x0009000000)
#define QEMU_VIRT_UART_SIZE                  (0x0000001000)

#define QEMU_VIRT_RTC_BASE                   (0x0009010000)
#define QEMU_VIRT_RTC_SIZE                   (0x0000001000)

#define QEMU_VIRT_FW_CFG_BASE                (0x0009020000)
#define QEMU_VIRT_FW_CFG_SIZE                (0x0000000018)

#define QEMU_VIRT_GPIO_BASE                  (0x0009030000)
#define QEMU_VIRT_GPIO_SIZE                  (0x0000001000)

#define QEMU_VIRT_SECURE_UART_BASE           (0x0009040000)
#define QEMU_VIRT_SECURE_UART_SIZE           (0x0000001000)

#define QEMU_VIRT_SMMU_BASE                  (0x0009050000)
#define QEMU_VIRT_SMMU_SIZE                  (0x0000020000)

#define QEMU_VIRT_MMIO_BASE                  (0x000a000000)
#define QEMU_VIRT_MMIO_SIZE                  (0x0000000200)

#define QEMU_VIRT_PLATFORM_BUS_BASE          (0x000c000000)
#define QEMU_VIRT_PLATFORM_BUS_SIZE          (0x0002000000)

#define QEMU_VIRT_SECURE_MEM_BASE            (0x000e000000)
#define QEMU_VIRT_SECURE_MEM_SIZE            (0x0001000000)

#define QEMU_VIRT_PCIE_MMIO_BASE             (0x0010000000)
#define QEMU_VIRT_PCIE_MMIO_SIZE             (0x002eff0000)

#define QEMU_VIRT_PCIE_PIO_BASE              (0x003eff0000)
#define QEMU_VIRT_PCIE_PIO_SIZE              (0x0000010000)

#define QEMU_VIRT_PCIE_ECAM_BASE             (0x003f000000)
#define QEMU_VIRT_PCIE_ECAM_SIZE             (0x0001000000)
                                                  
#define QEMU_VIRT_MEM_BASE                   (0x0040000000)
#define QEMU_VIRT_MEM_SIZE                   (0x3fc0000000) /* 255 GiB */

#define QEMU_VIRT_GIC_REDIST2_BASE           (0x4000000000)
#define QEMU_VIRT_GIC_REDIST2_SIZE           (0x0004000000)

#define QEMU_VIRT_PCIE_ECAM_HIGH_BASE        (0x4010000000)
#define QEMU_VIRT_PCIE_ECAM_HIGH_SIZE        (0x0010000000)

#define QEMU_VIRT_PCIE_MMIO_HIGH_BASE        (0x8000000000)
#define QEMU_VIRT_PCIE_MMIO_HIGH_SIZE        (0x8000000000)


/*
 * GIC on QEMU Virt 
 */
#define QEMU_VIRT_GIC_INT_MAX       (64)
#define QEMU_VIRT_GIC_PRIO_MAX      (16)
#define QEMU_VIRT_GIC_INTNO_SGIO    (0)
#define QEMU_VIRT_GIC_INTNO_PPIO    (16)
#define QEMU_VIRT_GIC_INTNO_SPIO    (32)

#define GIC_BASE                    (QEMU_VIRT_GIC_DIST_BASE)
#define GIC_INT_MAX                 (QEMU_VIRT_GIC_INT_MAX)
#define GIC_PRIO_MAX                (QEMU_VIRT_GIC_PRIO_MAX)
#define GIC_INTNO_SGI0		    (QEMU_VIRT_GIC_INTNO_SGIO)
#define GIC_INTNO_PPI0		    (QEMU_VIRT_GIC_INTNO_PPIO)
#define GIC_INTNO_SPI0		    (QEMU_VIRT_GIC_INTNO_SPIO)

#define GIC_PRI_SHIFT               (4)
#define GIC_PRI_MASK                (0x0f)

#define UART_BASE                   (QEMU_VIRT_UART_BASE)
#define AARCH64_UART_IRQ            (GIC_INTNO_SPI0+1)  /*< UART IRQ ( SPI no. 1) */
#define AARCH64_TIMER_IRQ           (27) /*< Timer IRQ  */

/*
 *  UART IMSC (Interrupt Mask Set)
 */
#define UART_IMSC_RXIM	(0x0010)		/*< RX mask */
#define UART_IMSC_TXIM	(0x0020)		/*< TX mask */

/*
 * UART FR(Flag Register)
 */
#define UART_FR_RXFE	  (0x10)                /*< RX FIFO Empty */
#define UART_FR_TXFF	  (0x20)                /*< TX FIFO Full */

/*
 *  UART_LCRH
 */
#define UART_LCRH_PEN		(0x02)	/*< Parity Enable */
#define UART_LCRH_EPS		(0x04)	/*< Even Parity */
#define UART_LCRH_STP2		(0x08)	/*< 2 Stop bit */
#define UART_LCRH_FEN		(0x10)	/*< FIFO Enable */
#define UART_LCRH_WLEN8	        (0x60)	/*< 8bit length */

/*
 *  UART_CR
 */
#define UART_CR_UARTEN	        (0x0001)  /*< UART Enable */
#define UART_CR_TXE		(0x0100)  /*< TX Enable  */
#define UART_CR_RXE		(0x0200)  /*< RX Enable  */

#define UART_CLR_ALL_INTR ( (1<<11) - 1 )  /*< All interrupts mask  */

/*
 * BPS on UART 
 * See 3.3.6 Fractional baud rate register, UARTFBRD and 
 * Table 3-9 Typical baud rates and integer and fractional divisors
 * in PrimeCell UART (PL011) Technical Reference Manual.
 */
#if defined(CONFIG_CON_SERIAL_BPS_230400)
#define UART_IBRD_VAL  (0x1)
#define UART_FBRD_VAL  (0x5)
#elif defined(CONFIG_CON_SERIAL_BPS_115200)
#define UART_IBRD_VAL  (0x2)
#define UART_FBRD_VAL  (0x8)
#elif defined(CONFIG_CON_SERIAL_BPS_76800)
#define UART_IBRD_VAL  (0x3)
#define UART_FBRD_VAL  (0x10)
#elif defined(CONFIG_CON_SERIAL_BPS_38400)
#define UART_IBRD_VAL  (0x6)
#define UART_FBRD_VAL  (0x21)
#elif defined(CONFIG_CON_SERIAL_BPS_14400)
#define UART_IBRD_VAL  (0x11)
#define UART_FBRD_VAL  (0x17)
#elif defined(CONFIG_CON_SERIAL_BPS_2400)
#define UART_IBRD_VAL  (0x68)
#define UART_FBRD_VAL  (0x8)
#elif defined(CONFIG_CON_SERIAL_BPS_110)
#define UART_IBRD_VAL  (0x8e0)
#define UART_FBRD_VAL  (0x2f)
#else  /*  Default 115200  */
#define UART_IBRD_VAL  (0x2)
#define UART_FBRD_VAL  (0x8)
#endif  /*  CONFIG_CON_SERIAL_BPS_230400  */

#if !defined(ASM_FILE)

#include <kern/freestanding.h>

#include <hal/kmap.h>

#define UART_DR     ((volatile uint32_t *)PHY_TO_KERN_STRAIGHT(UART_BASE))
#define UART_FR     ((volatile uint32_t *)PHY_TO_KERN_STRAIGHT((UART_BASE + 0x18) ))
#define UART_IBRD   ((volatile uint32_t *)PHY_TO_KERN_STRAIGHT((UART_BASE + 0x24) ))
#define UART_FBRD   ((volatile uint32_t *)PHY_TO_KERN_STRAIGHT((UART_BASE + 0x28) ))
#define UART_LCRH   ((volatile uint32_t *)PHY_TO_KERN_STRAIGHT((UART_BASE + 0x2c) ))
#define UART_CR     ((volatile uint32_t *)PHY_TO_KERN_STRAIGHT((UART_BASE + 0x30) ))
#define UART_RIS    ((volatile uint32_t *)PHY_TO_KERN_STRAIGHT((UART_BASE + 0x3c) ))
#define UART_IMSC   ((volatile uint32_t *)PHY_TO_KERN_STRAIGHT((UART_BASE + 0x38) ))
#define UART_ICR    ((volatile uint32_t *)PHY_TO_KERN_STRAIGHT((UART_BASE + 0x44) ))
#endif  /* !ASM_FILE  */
#endif  /*  _HAL_BOARD_H   */
