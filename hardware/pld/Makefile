all: ch375logic.jed ramdecode.jed

# Build the two JED files for the GALs
#
ch375logic.jed: ch375logic.pld
	galasm ch375logic.pld

ramdecode.jed: ramdecode.pld
	galasm ramdecode.pld

# Run the Verilog simulation of
# the CH375 logic GAL
#
output.vcd: testbed.v
	iverilog testbed.v 
	vvp a.out

# Burn the CH375 logic GAL
#
chburn:
	minipro -p 'ATF22V10C(UES)' -w ch375logic.jed

# Burn the RAM decode GAL
#
ramburn:
	minipro -p 'ATF22V10C(UES)' -w ramdecode.jed

clean:
	rm -f output.vcd a.out
	rm -f *.chp *.fus *.jed *.pin
