#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/poll.h>
#include <unistd.h>

#include "../include/register.h"
#include "../include/root.h"
#include "intc.h"
#include "system.h"

static int intc_fd;
static void *intc_ptr;

int32_t intc_init(const char devDevice[]) {
  if (is_root() == -1)
    return INTC_ERROR;

  intc_fd = open(devDevice, O_RDWR);
  if (intc_fd == OPEN_ERROR) {
    fprintf(stderr, "Error opening file '%s': %s\n", devDevice,
            strerror(errno));
  }

  intc_ptr =
      mmap(NULL, MMAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, intc_fd, 0);
  if (intc_ptr == MAP_FAILED) {
    fprintf(stderr, "Error map device memory to user space\n");
    goto close_file;
  }

  // Program AXI INTC IER register to allow AXI INTC to begin accepting
  // interrupt inputs signals and software interrupts
  for (int i = 0; i < IRQ_NUMS; i++) {
    SET_BIT(GET_ADDR(AXI_INTC_IER, intc_ptr), 32, i);
  }

  // Program AXI INTC MER register to enable Master IRQ and Hardware Interrupt
  SET_BIT(GET_ADDR(AXI_INTC_MER, intc_ptr), 32, 0);
  SET_BIT(GET_ADDR(AXI_INTC_MER, intc_ptr), 32, 1);

  intc_enable_uio_interrupts();

  return INTC_SUCCESS;

close_file:
  close(intc_fd);
  return INTC_ERROR;
}

void intc_exit() {
  munmap(intc_ptr, MMAP_SIZE);

  close(intc_fd);

  return;
}

uint32_t intc_wait_for_interrupt() {

  uint32_t irq_count = 0;
  read(intc_fd, &irq_count, sizeof(uint32_t));
  return *(uint32_t *)GET_ADDR(AXI_INTC_IPR, intc_ptr);
}

uint32_t intc_pending_nonblocking(int timeout) {

  uint32_t act_ints;
  struct pollfd *intc_fds;
  int time;
  int ret;

  intc_fds->fd = intc_fd;
  intc_fds->events = POLLIN;

  time = poll(intc_fds, 1, timeout);

  if (time > 0) {
    ret = read(intc_fd, &act_ints, 4);
    if (ret == -1) {
      fprintf(stderr, "Read UIO device interrupt error: %d\n", errno);
    }
  } else if (time == 0) {
    fprintf(stderr, "Poll timeout\n");
  } else {
    fprintf(stderr, "Poll Error\n");
  }

  return act_ints;
}

void intc_ack_interrupt(uint32_t irq_mask) {
  uint32_t *irq = (uint32_t*)GET_ADDR(AXI_INTC_IAR, intc_ptr);
  *irq = irq_mask;
  return;
}

void intc_enable_uio_interrupts() {
  int irq_enable = 1;
  int ret;
  ret = write(intc_fd, &irq_enable, sizeof(irq_enable));
  if (ret == -1) {
    fprintf(stderr, "Enable UIO device interrupt error: %d\n", errno);
  }
  return;
}

void intc_irq_enable(uint32_t irq_mask) {
  switch (irq_mask) {
  case SYSTEM_INTC_IRQ_FIT_MASK:
    SET_BIT(GET_ADDR(AXI_INTC_SIE, intc_ptr), 32, 0);
    break;
  case SYSTEM_INTC_IRQ_BUTTONS_MASK:
    SET_BIT(GET_ADDR(AXI_INTC_SIE, intc_ptr), 32, 1);
    break;
  case SYSTEM_INTC_IRQ_SWITCHES_MASK:
    SET_BIT(GET_ADDR(AXI_INTC_SIE, intc_ptr), 32, 2);
    break;
  case SYSTEM_INTC_IRQ_DMA_MASK:
    SET_BIT(GET_ADDR(AXI_INTC_SIE, intc_ptr), 32, 3);
    break;
  default:
    fprintf(stderr, "No valid irq!\n");
    return;
  }
  return;
}

void intc_irq_disable(uint32_t irq_mask) {
  switch (irq_mask) {
  case SYSTEM_INTC_IRQ_FIT_MASK:
    SET_BIT(GET_ADDR(AXI_INTC_CIE, intc_ptr), 32, 0);
    break;
  case SYSTEM_INTC_IRQ_BUTTONS_MASK:
    SET_BIT(GET_ADDR(AXI_INTC_CIE, intc_ptr), 32, 1);
    break;
  case SYSTEM_INTC_IRQ_SWITCHES_MASK:
    SET_BIT(GET_ADDR(AXI_INTC_CIE, intc_ptr), 32, 2);
    break;
  case SYSTEM_INTC_IRQ_DMA_MASK:
    SET_BIT(GET_ADDR(AXI_INTC_CIE, intc_ptr), 32, 3);
    break;
  default:
    fprintf(stderr, "No valid irq!\n");
    return;
  }
  return;
}