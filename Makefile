emulator: 
	g++ ./main.cpp ./cassette/cassette.cpp ./cpu/cpu.cpp -o emulator

clean:
	rm emulator