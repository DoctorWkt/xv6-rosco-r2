// Access the bit-bang SD card block device

#include <xv6/types.h>
#include <xv6/defs.h>
#include <xv6/param.h>
#include <xv6/fs.h>
#include <xv6/buf.h>

typedef unsigned char uint8_t;
typedef unsigned int uint32_t;

// This are in the asmcode.s file
extern void stop_timer(void);
extern void irq5_install();

typedef enum {
    SD_CARD_TYPE_V1,
    SD_CARD_TYPE_V2,
    SD_CARD_TYPE_SDHC,
    SD_CARD_TYPE_UNKNOWN,
} SDCardType;

typedef enum {
    SD_INIT_OK,
    SD_INIT_IDLE_FAILED,
    SD_INIT_CMD8_FAILED,
    SD_INIT_ACMD41_FAILED,
} SDInitStatus;

typedef struct {
    int             initialized;
    SDCardType      type;
    uint32_t        reserved[4];
} SDCard;

// In sdcard_asm.s
int SD_check_support();
SDInitStatus SD_initialize(SDCard *sd);
int SD_read_block(SDCard *sd, uint32_t block, void *buf);
int SD_write_block(SDCard *sd, uint32_t block, void *buf);
int SD_read_register(SDCard *sd, uint8_t regcmd, void *buf);

// Variables
static SDCard SD;	// SD card details
static uint32_t pbase;	// LBA base of the xv6 partition

// Initialise the SD block device
void blkinit(void) {
  uint8_t buf[512];
  
  // Say hello before we start
  cprintf("About to initialise the SD card\n");

  // Stop the timer
  // stop_timer();

  // Install the IRQ5 handler
  // and the syscall handler
  irq5_install();

  // Initialise the SD card
  if (SD_initialize(&SD) != SD_INIT_OK)
    panic("SD_initialize failed");

  // Read the partition table
  if (SD_read_block(&SD, 0, buf)==0)
    panic("SD read partition table failed");

  // Check that we do have a partition tabled
  if (buf[0x1fe] != 0x55 || buf[0x1ff] != 0xaa)
    panic("No partition table at SD LBA 0");

  // Search for a partition with id 0x07 which
  // is what we are using for xv6
  for (int i= 0x1be; i <= 0x1ee; i+= 0x10) {
    if (buf[i+4] == 0x07) {
      // Found the partition, now get the base LBA
      pbase= buf[i+8]         | (buf[i+9]) << 8 |
	    (buf[i+10]) << 16 | (buf[i+11]) << 24 ;
      cprintf("SD card xv6 partition at 0x%x\n", pbase);
      return;
    }
  }

  panic("No xv6 partition (id 7) found on SD card");
}

// Given a pointer to a 512-byte buffer and
// an LBA number, read the block into the
// buffer. Return 1 on success, 0 otherwise.
unsigned char read_block(unsigned char *buf, uint lba) {

  // Check we have a buffer
  if (buf==NULL) return(0);

  if (SD_read_block(&SD, lba+pbase, buf)==0)
    panic("SD_read_block");

  return(1);
}

// Given a pointer to a 512-byte buffer and
// an LBA number, write the buffer into the
// block. Return 1 on success, 0 otherwise.
unsigned char write_block(unsigned char *buf, uint lba) {

  // Check we have a buffer
  if (buf==NULL) return(0);

  if (SD_write_block(&SD, lba+pbase, buf)==0)
    panic("SD_write_block");
  return(1);
}

// Read/write a buffer.
// If B_DIRTY is set, write buf to disk, clear B_DIRTY, set B_VALID.
// Else if B_VALID is not set, read buf from disk, set B_VALID.
void blkrw(struct buf *b) {
  if ((b->flags & (B_VALID | B_DIRTY)) == B_VALID)
    panic("blkrw1");

  if (b->flags & B_DIRTY) {
    if (write_block(b->data, b->blockno) == 0)
      panic("blkrw2");
    b->flags &= ~B_DIRTY;
  } else {
    if (read_block(b->data, b->blockno) == 0)
      panic("blkrw3");
  }
  b->flags |= B_VALID;
}
