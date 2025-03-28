#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>

#include "switches.h"
#include "../include/register.h"
#include "../include/root.h"

static int sw_fd;
static void *sw_ptr;

int32_t switches_init(const char *devFilePath) {
 
  if (is_root() == -1 )
    return SWITCHES_ERROR;

  sw_fd = open(devFilePath, O_RDWR);
  if (sw_fd == OPEN_ERROR) {
    fprintf(stderr, "Error opening file '%s': %s\n", devFilePath,
            strerror(errno));
    return SWITCHES_ERROR;
  }

  sw_ptr = mmap(NULL, MMAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, sw_fd, 0);
  if (sw_ptr == MAP_FAILED) {
    fprintf(stderr, "Error map device memory to user space\n");
    goto close_file;
  }

  SET_BIT(GET_ADDR(GPIO_GIER_OFFSET, sw_ptr), 32, 31);
  printf("Globle Interrupt Enabled!\n");

  switches_enable_interrupts();

  return SWITCHES_SUCCESS;

close_file:
  close(sw_fd);
  return SWITCHES_ERROR;
}

void switches_exit() {
  munmap(sw_ptr, MMAP_SIZE);

  close(sw_fd);

  return;
}

uint8_t switches_read() {
  uint8_t switches_state;

  switches_state = *(uint8_t *)GET_ADDR(GPIO_DATA_OFFSET, sw_ptr);

  return switches_state;
}

void switches_enable_interrupts() {
  SET_BIT(GET_ADDR(GPIO_IPIER_OFFSET, sw_ptr), 32, 0);

  return;
}

void switches_disable_interrupts() {
  CLEAR_BIT(GET_ADDR(GPIO_IPIER_OFFSET, sw_ptr), 32, 0);

  return;
}

bool switches_interrupt_pending() {
  bool is_interrupt;

  is_interrupt = GET_BIT(GET_ADDR(GPIO_IPISR_OFFSET, sw_ptr), 32, 0);

  return is_interrupt;
}

void switches_ack_interrupt() {

  SET_BIT(GET_ADDR(GPIO_IPISR_OFFSET, sw_ptr), 32, 0);

  return;
}