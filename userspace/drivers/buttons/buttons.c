#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>

#include "../include/register.h"
#include "../include/root.h"
#include "buttons.h"

static int bt_fd;
static void *bt_ptr;

int32_t buttons_init(const char *devFilePath) {
  if (is_root() == -1)
    return BUTTONS_ERROR;

  bt_fd = open(devFilePath, O_RDWR);
  if (bt_fd == OPEN_ERROR) {
    fprintf(stderr, "Error opening file '%s': %s\n", devFilePath,
            strerror(errno));
    return BUTTONS_ERROR;
  }

  bt_ptr = mmap(NULL, MMAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, bt_fd, 0);
  if (bt_ptr == MAP_FAILED) {
    fprintf(stderr, "Error map device memory to user space\n");
    goto close_file;
  }

  SET_BIT(GET_ADDR(GPIO_GIER_OFFSET, bt_ptr), 32, 31);
  printf("Globle Interrupt Enabled!\n");

  buttons_enable_interrupts();

  return BUTTONS_SUCCESS;

close_file:
  close(bt_fd);
  return BUTTONS_ERROR;
}

void buttons_exit() {
  munmap(bt_ptr, MMAP_SIZE);
  close(bt_fd);
  return;
}

uint8_t buttons_read() {
  uint8_t buttons_state;

  buttons_state = *(uint8_t *)GET_ADDR(GPIO_DATA_OFFSET, bt_ptr);

  return buttons_state;
}

void buttons_enable_interrupts() {

  SET_BIT(GET_ADDR(GPIO_IPIER_OFFSET, bt_ptr), 32, 0);

  return;
}

void buttons_disable_interrupts() {

  CLEAR_BIT(GET_ADDR(GPIO_IPIER_OFFSET, bt_ptr), 32, 0);

  return;
}

bool buttons_interrupt_pending() {
  bool is_interrupt;

  is_interrupt = GET_BIT(GET_ADDR(GPIO_IPISR_OFFSET, bt_ptr), 32, 0);

  return is_interrupt;
}

void buttons_ack_interrupt() {

  SET_BIT(GET_ADDR(GPIO_IPISR_OFFSET, bt_ptr), 32, 0);

  return;
}