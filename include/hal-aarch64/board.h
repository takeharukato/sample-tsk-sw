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

#include <kern/freestanding.h>
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

#define AARCH64_TIMER_IRQ           (27)  /** Timer IRQ  */


#endif  /*  _HAL_BOARD_H   */
