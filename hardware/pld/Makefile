all: ch375logic.jed ramdecode.jed

ch375logic.jed: ch375logic.pld
	galasm ch375logic.pld

ramdecode.jed: ramdecode.pld
	galasm ramdecode.pld

output.vcd: testbed.v
	iverilog testbed.v 
	vvp a.out

clean:
	rm -f output.vcd a.out
	rm -f *.chp *.fus *.jed *.pin
