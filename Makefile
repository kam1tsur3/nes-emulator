emulator: 
	g++ -lpthread ./main.cpp ./cassette/cassette.cpp ./cpu/cpu.cpp ./bus/cpu_bus.cpp ./bus/ppu_bus.cpp ./ram/ram.cpp ./ppu/ppu.cpp -o emulator -framework GLUT -framework OpenGL 

clean:
	rm emulator