#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

// Linker defines
extern uint32_t _data_start, _data_end, _code_end, _bss_start, _bss_end;

__attribute__((weak)) void __kinit() {
  // zero .bss
  for (uint32_t *dst = &_bss_start; dst < &_bss_end; *dst++ = 0);
}
