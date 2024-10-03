#ifndef A_OUT_H
#define A_OUT_H

#include <stdint.h>

// The xv6 a.out header

#define AOUT_MAGIC 0x600E

struct aout {
  uint16_t a_magic;		// Magic number
  uint16_t a_pad;		// Padding
  uint32_t a_code_end;		// End address of code
  uint32_t a_bss_end;		// End address of bss
  uint32_t a_total_size;	// Total process size in bytes
};
#endif
