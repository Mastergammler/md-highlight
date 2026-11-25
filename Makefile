default: build

.PHONEY: run
run: build
	.build/mdhigh wiki/tickets/'⮜ 001 ⮞  Basic Colorings.md'

build:
	ninja .build/mdhigh 

debug: 
	gdb --args .build/mdhigh 


