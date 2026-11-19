default: build

.PHONEY: run
run: build
	.build/mdhigh testfiles/basic.md

build:
	ninja .build/mdhigh 

debug: 
	gdb --args .build/mdhigh 


