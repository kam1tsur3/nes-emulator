CC	= g++
LIBS	= -lpthread
OPENGL	= -framework GLUT -framework OpenGL
TARGET	= emulator

SOURCESDIR	=	./modules
SOURCES	=	$(wildcard $(SOURCESDIR)/*.cpp)
OBJS	=	$(SOURCES:.cpp=.o)

all:	clean $(TARGET)

$(TARGET):	main.cpp $(SOURCES)
	$(CC) $(LIBS) main.cpp ./modules/cassette.cpp ./modules/controller.cpp ./modules/cpu.cpp ./modules/cpu_bus.cpp ./modules/ppu_bus.cpp ./modules/ram.cpp ./modules/ppu.cpp ./modules/dma.cpp -o $(TARGET) $(OPENGL)

#$(TARGET):	main.o $(OBJS)
#	$(CC) main.o $(OBJS) $(LIBS) $(OPENGL) -o $(TARGET)
#
#$(TARGET):	main.cpp $(SOURCES)
#	$(CC) $(LIBS) main.cpp $(SOURCES) -o $(TARGET) $(OPENGL) 
#
#%.o:	%.cpp
#	$(CC) -c $< -o $@

clean:
	rm -f $(SOURCESDIR)/*.o main.o *~ $(TARGET)