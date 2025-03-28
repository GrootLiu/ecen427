// GPIO REGISTERS
#define GPIO_DATA_OFFSET 0x0000
#define GPIO_TRI_OFFSET 0x0004
#define GPIO_GIER_OFFSET 0x00011C
#define GPIO_IPIER_OFFSET 0x000128
#define GPIO_IPISR_OFFSET 0x000120

// AXI INTC REGISTERS
#define AXI_INTC_ISR 0x00
#define AXI_INTC_IPR 0x04
#define AXI_INTC_IER 0x08
#define AXI_INTC_IAR 0x0C
#define AXI_INTC_SIE 0x10
#define AXI_INTC_CIE 0x14
#define AXI_INTC_IVR 0x18
#define AXI_INTC_MER 0x1C
#define AXI_INTC_IMR 0x20
#define AXI_INTC_ILR 0x24

#define IRQ_NUMS 4

#define OPEN_ERROR -1

#define MMAP_SIZE 0x200

#define GET_ADDR(GPIO_OFFSET, ptr) ((ptr) + (GPIO_OFFSET))

#define GET_BIT(DATA_P, data_size, x)                                          \
  ({                                                                           \
    uint32_t result = 0;                                                       \
    if (data_size == 8) {                                                      \
      result = *(uint8_t *)(DATA_P) & (0x1 << (x));                            \
    } else if (data_size == 16) {                                              \
      result = *(uint16_t *)(DATA_P) & (0x1 << (x));                           \
    } else if (data_size == 32) {                                              \
      result = *(uint32_t *)(DATA_P) & (0x1 << (x));                           \
    }                                                                          \
    result;                                                                    \
  })

#define SET_BIT(DATA_P, data_size, x)                                          \
  do {                                                                         \
    if (data_size == 8) {                                                      \
      *(uint8_t *)(DATA_P) |= (0x1 << (x));                                    \
    } else if (data_size == 16) {                                              \
      *(uint16_t *)(DATA_P) |= (0x1 << (x));                                   \
    } else if (data_size == 32) {                                              \
      *(uint32_t *)(DATA_P) |= (0x1 << (x));                                   \
    }                                                                          \
  } while (0)

#define CLEAR_BIT(DATA_P, data_size, x)                                        \
  do {                                                                         \
    if (data_size == 8) {                                                      \
      *(uint8_t *)(DATA_P) &= ~(0x1 << (x));                                   \
    } else if (data_size == 16) {                                              \
      *(uint16_t *)(DATA_P) &= ~(0x1 << (x));                                  \
    } else if (data_size == 32) {                                              \
      *(uint32_t *)(DATA_P) &= ~(0x1 << (x));                                  \
    }                                                                          \
  } while (0)
