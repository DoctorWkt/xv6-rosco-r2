// Access the CH375 block device

#include <xv6/types.h>
#include <xv6/defs.h>
#include <xv6/param.h>
#include <xv6/fs.h>
#include <xv6/buf.h>

// These are in the asmcode.s file
extern void cpu_delay(int ms);
extern void stop_timer(void);
extern void irq3_install();
extern void send_ch375_cmd(unsigned char cmd);
extern void send_ch375_data(unsigned char cmd);
extern unsigned char read_ch375_data(void);
extern unsigned char get_ch375_status(void);

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

  // Stop the timer
  stop_timer();

  // Install the IRQ3 handler
  // and the syscall handler
  irq3_install();

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

// Given a pointer to a 512-byte buffer and
// an LBA number, read the block into the
// buffer. Return 1 on success, 0 otherwise.
unsigned char read_block(unsigned char *buf, uint lba) {
  unsigned char i, status, cnt;

  // Check we have a buffer
  if (buf==NULL) return(0);

  // Send the disk read command followed by the
  // LBA in little-endian format, then ask for
  // one block.
  send_ch375_cmd(CMD_DISK_READ);
  send_ch375_data(lba         & 0xff);
  send_ch375_data((lba >> 8)  & 0xff);
  send_ch375_data((lba >> 16) & 0xff);
  send_ch375_data((lba >> 24) & 0xff);
  send_ch375_data(1);

  // Loop eight times reading in
  // 64 bytes of data each time.
  for (i=0; i<8; i++) {

    // Get the result of the command
    status= get_valid_ch375_status();
    if (status == USB_INT_DISK_READ) {
      // Now read the data
      send_ch375_cmd(CMD_RD_USB_DATA);
      cnt= read_ch375_data();
      while (cnt--) *buf++ = read_ch375_data();
      
      // After 64 bytes, tell the CH375
      // to get the next set of data
      send_ch375_cmd(CMD_DISK_RD_GO);
    }
  }

  // Get the status after reading the block
  status= get_valid_ch375_status();
  if (status==USB_INT_SUCCESS) {
    return(1);
  }
  else
    return(0);
}

// Given a pointer to a 512-byte buffer and
// an LBA number, write the buffer into the
// block. Return 1 on success, 0 otherwise.
unsigned char write_block(unsigned char *buf, uint lba) {
  unsigned char i, status, cnt;

  // Check we have a buffer
  if (buf==NULL) return(0);

  // Send the disk write command followed by the
  // LBA in little-endian format, then ask to
  // send one block.
  send_ch375_cmd(CMD_DISK_WRITE);
  send_ch375_data(lba         & 0xff);
  send_ch375_data((lba >> 8)  & 0xff);
  send_ch375_data((lba >> 16) & 0xff);
  send_ch375_data((lba >> 24) & 0xff);
  send_ch375_data(1);

  // Loop eight times writing out
  // 64 bytes of data each time.
  for (i=0; i<8; i++) {

    // Get the result of the command
    status= get_valid_ch375_status();
    if (status == USB_INT_DISK_WRITE) {
      // Now send the data
      send_ch375_cmd(CMD_WR_USB_DATA);
      cnt= 64; send_ch375_data(cnt);
      while (cnt--) send_ch375_data( *buf++ );
      
      // After 64 bytes, tell the CH375
      // to get the next set of data
      send_ch375_cmd(CMD_DISK_WR_GO);
    }
  }

  // Get the status after reading the block
  status= get_valid_ch375_status();
  if (status==USB_INT_SUCCESS)
    return(1);
  else
    return(0);
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
