ARCH := /usr/bin/
ARCH := powerpc-e300c3-linux-gnu-
CC := $(ARCH)g++
STRIP := $(ARCH)strip
CFLAGS:= -Imstp -Iuart
OBJ := mstp/mstpcrc.o 
OBJ += mstp/recvframe.o 
OBJ += mstp/sendframe.o
OBJ += mstp/timeoperator.o
OBJ += mstp/bacdcode.o 
OBJ += mstp/make_apdu.o
OBJ += mstp/npdu.o 
OBJ += mstp/apdu.o 
OBJ += mstp/bacnet.o 
OBJ += uart/uart.o 
OBJ += uart/accesstermios.o
OBJ += uart/comport.o

TARGET := bin/mstpframe.exe
TARGET += bin/testmstp.exe
TARGET += bin/bacnet.exe

target: $(TARGET)
	$(STRIP) $+
	@ls -shl $+

bin/mstpframe.exe: $(OBJ) test/mstpframe.o
	@$(CC)  $+ -o $@
bin/bacnet.exe: $(OBJ) test/bacnet.o
	@$(CC) $(CFLAGS) $+ -o $@
bin/testmstp.exe: $(OBJ) test/testmstp.o
	@$(CC) $(CFLAGS) $+ -o $@
.c.o:
	$(CC) $(CFLAGS) -c $*.c -o $*.o
.cpp.o:
	$(CC) $(CFLAGS) -c $*.cpp -o $*.o

clean:
	@rm -rfv $(TARGET) $(OBJ) test/*.o
