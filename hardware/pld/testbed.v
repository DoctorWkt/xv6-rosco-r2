// Test module for CH375 GAL logic
// (c) 2024 Warren Toomey, GPL3

`timescale 1ns/1ps
`default_nettype none

module testbed;

  reg   clk;	// Master clock
  reg   int_n;	// Interrupt line from CH375
  reg   vma_n;	// VMA line from CPU
  reg	S0;	// State registers
  reg	S1;
  wire  vpa_n;	// VPA output to CPU
  wire  irq3_n;	// Interrupt output to CPU

  // We go through three S1/S0 states with these transitions:
  // 0/0: int_n is high. When int_n goes low we move to ...
  // 0/1: int_n is now low. When vma_n goes low we move to ...
  // 1/1: int_n is still low. When int_n goes high we move to ...
  // 0/0
  //
  // The trick is to consider what the register values S0
  // and S1_were_ before the rising clock edge, but consider
  // what the signal values _are_ at the rising clock edge.

  always @(posedge clk) begin

    // S0 can be high when
    // a) we are in 0/0 and int_n is low
    // b) we are in 0/1 (thus, remain in 0/1)
    // c) we are in 1/1 and int_n is low
    S0 <=   ( !S0 & !S1 & !int_n)
	  | (  S0 & !S1 )
	  | (  S0 &  S1 & !int_n);

    // S1 can be high when
    // a) we are in 0/1 and vma_n is low
    // b) we are in 1/1 and int_n is low
    S1 <=   ( S0 & !vma_n)
	  | ( S0 & S1 & !int_n);
  end

  // The two outputs are enabled only in state 0/1:
  //   irq_3 goes low, otherwise high
  //   vpa_n goes low, otherwise high-Z
  assign irq3_n = (S0 & !S1) ? 1'b0 : 1'b1;
  assign vpa_n  = (S0 & !S1) ? 1'b0 : 1'bz;

  // Clock generator: 10MHz, so we
  // toggle the clock every 50ns
  always begin
    #50 clk = ~clk;
  end

  initial begin

    $dumpfile("output.vcd");
    $dumpvars;
    clk=   0;
    int_n= 1;
    vma_n= 1;
    S0=    0;
    S1=    0;

    // Lower the interrupt line after 140ns
    #125 int_n= 0;

    // Lower VMA after 300ns
    #375 vma_n= 0;

    // Raise VMA after 400ns
    #400 vma_n= 1;

    // Raise the interrupt line after 500ns
    #500 int_n= 1;

    #2000 $finish;      // Terminate simulation
  end

endmodule
