.PHONY: all test clean reallyclean

MOSQUITTO_PATH=${MOSQUITTO_SRC_PATH}

CFLAGS=-I$(MOSQUITTO_PATH)/lib -I$(MOSQUITTO_PATH)/src -Wall -Werror

all : auth_plugin.so

auth_plugin.so : auth_plugin.c
	$(CC) ${CFLAGS} -fPIC -shared $^ -o $@ 

reallyclean : clean
	-rm -f *.orig

clean :
	rm -f *.so *.test
