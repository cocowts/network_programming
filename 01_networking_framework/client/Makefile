CC=gcc
 
DEFINE =
INCLUDE = -I.
 
SOURCES = $(wildcard *.c)
OBJS = $(patsubst %.c,%.o,$(SOURCES))
 
TARGET  = a.out
 
All:$(OBJS)
	$(CC) -o $(TARGET) $(OBJS) -lpthread 
 
%.o:%.c
	$(CC) -c $(CFLAGS_WARN) $(DEFINE) $(INCLUDE) $< -o $@	
clean:	
	rm -f $(OBJS)
	rm -f $(TARGET)