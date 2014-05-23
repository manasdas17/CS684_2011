OBJ_DIR = ./objs
STAGE_DIR=../stage

all: map.so client
map.so: ACC.o kcg_types.o
	g++ -shared $(OBJ_DIR)/ACC.o -o $(STAGE_DIR)/map.so

ACC.o: ACC.c stage.hh kcg_consts.h kcg_types.h kcg_sensors.h
	g++ -c ACC.c -o $(OBJ_DIR)/ACC.o

kcg_types.o: kcg_types.c kcg_types.h
	g++ -c kcg_types.c -o $(OBJ_DIR)/kcg_types.o

client: client.c client.h
	g++ client.c -o client

clean:
	rm $(OBJ_DIR)/*.o
	rm $(STAGE_DIR)/map.so
	rm client
run-sim:
	make map.so
	stage $(STAGE_DIR)/ACC.world
run-cli:
	make client
	chmod a+x client
	xterm "/mnt/win_e/mtp/stage/bharat/Stage-3.2.2-Source/worlds/ex/acc/wifi"
run-tcli:
	make client
	chmod a+x client
	./client &
run:
	make
	chmod a+x client
	xterm "/mnt/win_e/mtp/stage/bharat/Stage-3.2.2-Source/worlds/ex/acc/wifi" &
	#./client &
	stage $(STAGE_DIR)/ACC.world
