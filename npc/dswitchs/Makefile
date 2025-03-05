FI:=$(wildcard *.v)
FIL:=$(patsubst %.v,%,$(FI))
sim:
	$(call git_commit, "sim RTL") # DO NOT REMOVE THIS LINE!!!
	verilator --cc --trace-fst --exe --build -j 0 -Wall *.cpp *.v
	
	gtkwave waveform.fst
	
clean:
	rm -rf obj_dir
