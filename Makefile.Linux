
CC   = ../wine/tools/winegcc/winegcc -D__DEBUG__
CC2   = ../wine/tools/winegcc/winegcc -D__DEBUG__ -B../wine/tools/winebuild ../wine/libs/port/libwine_port.a
OBJ  = ./obj/main.o ./obj/rtl_ccmd.o ./obj/rtl_cmd.o ./obj/rtl_dbgwin.o ./obj/rtl_error.o ./obj/rtl_mem.o ./obj/rtl_parse.o ./obj/rtl_vars.o ./obj/rtl_stack.o ./obj/rtl_rstack.o ./obj/rtl_mathparser.o ./obj/vcl_form.o ./obj/rtl_string.o ./obj/rtl_float.o ./obj/rtl_misc.o ./obj/rtl_for.o ./obj/rtl_loops.o ./obj/rtl_callback.o
LINKOBJ  = ./obj/main.o ./obj/rtl_ccmd.o ./obj/rtl_cmd.o ./obj/rtl_dbgwin.o ./obj/rtl_error.o ./obj/rtl_mem.o ./obj/rtl_parse.o ./obj/rtl_vars.o ./obj/rtl_stack.o ./obj/rtl_rstack.o ./obj/rtl_mathparser.o ./obj/vcl_form.o ./obj/rtl_string.o ./obj/rtl_float.o ./obj/rtl_misc.o ./obj/rtl_for.o ./obj/rtl_loops.o ./obj/rtl_callback.o
LIBS =   -mwindows -lwinmm -lkernel32 -luser32 -lgdi32 -s
INCS =  -I"../wine/include"
BIN  = runtime
CFLAGS = $(INCS)   -fexpensive-optimizations -O3 -g3 -march=i586
RM = rm -f

.PHONY: all all-before all-after clean clean-custom

all: all-before runtime all-after


clean: clean-custom
	${RM} $(OBJ) $(BIN)

$(BIN): $(OBJ)
	echo "blubb"
	$(CC2) $(LINKOBJ) -o "./bin/runtime" $(LIBS)

./obj/main.o: main.c
	$(CC) -c main.c -o ./obj/main.o $(CFLAGS)

./obj/rtl_ccmd.o: rtl_ccmd.c
	$(CC) -c rtl_ccmd.c -o ./obj/rtl_ccmd.o $(CFLAGS)

./obj/rtl_cmd.o: rtl_cmd.c
	$(CC) -c rtl_cmd.c -o ./obj/rtl_cmd.o $(CFLAGS)

./obj/rtl_dbgwin.o: rtl_dbgwin.c
	$(CC) -c rtl_dbgwin.c -o ./obj/rtl_dbgwin.o $(CFLAGS)

./obj/rtl_error.o: rtl_error.c
	$(CC) -c rtl_error.c -o ./obj/rtl_error.o $(CFLAGS)

./obj/rtl_mem.o: rtl_mem.c
	$(CC) -c rtl_mem.c -o ./obj/rtl_mem.o $(CFLAGS)

./obj/rtl_parse.o: rtl_parse.c
	$(CC) -c rtl_parse.c -o ./obj/rtl_parse.o $(CFLAGS)

./obj/rtl_vars.o: rtl_vars.c
	$(CC) -c rtl_vars.c -o ./obj/rtl_vars.o $(CFLAGS)

./obj/rtl_stack.o: rtl_stack.c
	$(CC) -c rtl_stack.c -o ./obj/rtl_stack.o $(CFLAGS)

./obj/rtl_rstack.o: rtl_rstack.c
	$(CC) -c rtl_rstack.c -o ./obj/rtl_rstack.o $(CFLAGS)

./obj/rtl_mathparser.o: rtl_mathparser.c
	$(CC) -c rtl_mathparser.c -o ./obj/rtl_mathparser.o $(CFLAGS)

./obj/vcl_form.o: vcl_form.c
	$(CC) -c vcl_form.c -o ./obj/vcl_form.o $(CFLAGS)

./obj/rtl_string.o: rtl_string.c
	$(CC) -c rtl_string.c -o ./obj/rtl_string.o $(CFLAGS)

./obj/rtl_float.o: rtl_float.c
	$(CC) -c rtl_float.c -o ./obj/rtl_float.o $(CFLAGS)

./obj/rtl_misc.o: rtl_misc.c
	$(CC) -c rtl_misc.c -o ./obj/rtl_misc.o $(CFLAGS)

./obj/rtl_for.o: rtl_for.c
	$(CC) -c rtl_for.c -o ./obj/rtl_for.o $(CFLAGS)

./obj/rtl_loops.o: rtl_loops.c
	$(CC) -c rtl_loops.c -o ./obj/rtl_loops.o $(CFLAGS)

./obj/rtl_callback.o: rtl_callback.c
	$(CC) -c rtl_callback.c -o ./obj/rtl_callback.o $(CFLAGS)

#./obj/runtime_private.res: runtime_private.rc 
#	$(WINDRES) -i runtime_private.rc --input-format=rc -o ./obj/runtime_private.res -O coff 
