// Access the CH375 block device

#include <xv6/types.h>
#include <xv6/defs.h>
#include <xv6/param.h>
#include <xv6/fs.h>
#include <xv6/buf.h>

// CH375 commands
#define CMD_RESET_ALL    	0x05
#define CMD_SET_USB_MODE 	0x15
#define CMD_GET_STATUS   	0x22
#define CMD_RD_USB_DATA  	0x28
#define CMD_WR_USB_DATA  	0x2B
#define CMD_DISK_INIT    	0x51
#define CMD_DISK_SIZE    	0x53
#define CMD_DISK_READ    	0x54
#define CMD_DISK_RD_GO   	0x55
#define CMD_DISK_WRITE   	0x56
#define CMD_DISK_WR_GO   	0x57
#define CMD_DISK_READY   	0x59

// CH375 status results
#define USB_INT_SUCCESS    0x14
#define USB_INT_CONNECT    0x15
#define USB_INT_DISCONNECT 0x16
#define USB_INT_DISK_READ  0x1D
#define USB_INT_DISK_WRITE 0x1E

// Wait to get a valid status from the CH375.
static unsigned char get_valid_ch375_status(void) {
  unsigned char status;

  while (1) {
    status= get_ch375_status();
    if (status != 0xff) break;
  }
  return(status);
}

// Initialise the CH375 block device
void blkinit(void) {
  unsigned char status;
  
  // Say hello before we start
  cprintf("About to initialise the CH375\n");

  // Install the IRQ5 handler
  // and the syscall handler
  irq5_install();

  // Send the reset command and wait 50mS
  send_ch375_cmd(CMD_RESET_ALL);
  cpu_delay(50);

  // Now set the USB mode to 6. This should
  // cause a level 3 interrupt which will
  // update the CH375 status in memory.
  send_ch375_cmd(CMD_SET_USB_MODE);
  send_ch375_data(6);

  // Get out the CH375 status.
  // We expect to get USB_INT_CONNECT
  status= get_valid_ch375_status();
  if (status != USB_INT_CONNECT) {
    panic("ch375_init1");
  }

  // Now initialise the disk. In the real world, this
  // might return USB_INT_DISCONNECT. In this case,
  // the code would prompt the user to attach a USB key
  // and try again.
  send_ch375_cmd(CMD_DISK_INIT);
  status= get_valid_ch375_status();
  if (status != USB_INT_SUCCESS) {
    panic("ch375_init2");
  }
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
