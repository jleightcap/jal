%.bin: %.s
	vasm6502_oldstyle -dotdir -wdc02 -Fbin $^ -o $@

# TEST=[testfile] make test
test:
	x6502 $(TEST)
