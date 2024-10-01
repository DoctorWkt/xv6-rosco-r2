## RAM Expansion and CH375 Board for Rosco Classic v2

The 680000 doesn't have any built-in memory management, and I didn't want
to write the code to load and relocate an ELF binary so that it could run
at any specific base memory address. I also wanted to support the
[CH375](https://www.electrodragon.com/product/ch375-module-reading-and-writing-u-diskusb-communicate/)
device, which provides an 8-bit parallel interface to USB block devices.

Thus, I have built an expansion card that `xv6` uses. It has two main
components:

 - the CH375 device and associated addressing/interrupt components
 - 1M of expansion RAM along with primitive memory management hardware

In this directory you can find the [KiCad board design](kicad), the
[code](pld) for the two GALs on the board, a PDF of the
[schematic](schematic.pdf), a mock-up [image](ch375_board.jpg)
of the board and some [docs](documentation) of the CH375 device.

## The CH375 Device

The CH375 provides block-level access to a USB via an 8-bit TTL-compatible
parallel interface. We need to decode two I/O addresses for this device.
We also need to convert the active-low interrupt that the CH375 generates
into several lines that make it compatible with the 68000 auto-vector
interrupt protocol.

The [GAL code](pld/ch375logic.pld) that interfaces between the 68000 and the
CH375 does these things:

 - It sets the CH375 chip select low for addresses $00FFF001 (data) and
   $00FFF003 (commands), and lowers DTACK at the same time.
 - When the CH375 sends an interrupt, it drops the 68000 IRQ3 and VPA lines 
   low until the 68000 drops the VMA line. Then both IRQ3 and VPA go back to
    being high-Z.

## The Expansion RAM and the Base Register

The expansion board adds 1M of expansion RAM to the Rosco classic r2 board.
It also provides primitive memory management hardware in the form of a
[base register](https://en.wikipedia.org/wiki/Base_and_bounds). Let's look
at how this works.

The base register is a 4-bit I/O register (initially zero) which can be written
to at address $00FFE001.

The base register's value, multiplied by 64K, is then added to any expansion
RAM address. This allows the "base" of the expansion RAM to be positioned
at any of sixteen 64K-spaced locations.

For example, with the base register set to 0x0, expansion memory address
$00100124 becomes $00100124 + $0000000 => $00100124. But if the base
register was set to 0x4, then address $00100124 becomes
$00100124 + $0040000 => $00140124.

The purpose of the base register is to allow an operating system to load
multiple programs into the expansion RAM, each starting at a different
64K position. By setting the base register suitably, each program will
think that it starts at $00100000 instead of its actual physical location.

In terms of hardware, the register itself is a 74HCT161 4-bit register, and the
[GAL code](pld/ramdecode.pld) in the second GAL decodes its $00FFE001 address.
This is connected to a 74HCT283 4-bit adder. The base register's value is added
to address lines A16...A19 to form the physical address that is then sent to the
two RAM chips.

