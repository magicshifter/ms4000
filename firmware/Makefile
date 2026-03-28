SHELL := /bin/bash

MS4_PYTHON := $(shell which python3)

MS4_FLASH_BAUD=921600
MS4_FLASH_BAUD=57600

#linux
MS4_PORT=/dev/ttyUSB0
#macos
MS4_PORT=/dev/tty.SLAB_USBtoUART

#'autodetect'
MS4_PORT=`pio device list --json-output | $(MS4_PYTHON) -c 'exec("""\nimport json,sys\n\nobj=json.load(sys.stdin)\nfor y in obj:\n if "10C4:EA60" in y["hwid"].upper():\n  print (y["port"])\n""") ' | head -1`

all:	clean copy-libs build flash monitor

NANOPB_DIR=../tools/nanopb/

CFLAGS += -I$(NANOPB_DIR) -DPB_FIELD_16BIT

CSRC += $(NANOPB_DIR)pb_encode.c
CSRC += $(NANOPB_DIR)pb_decode.c
CSRC += $(NANOPB_DIR)pb_common.c

PROTOC = ../../tools/nanopb/generator/protoc
PROTOC_OPTS += -I../../tools/nanopb/generator/proto
PROTOC_GEN_NANOPB=../../tools/nanopb/generator/protoc-gen-nanopb

ESPOTA = ../tools/ESP8266_ota/espota.py

#!J! Only here for reference during intake - see platformio.ini instead!
pio-deps:
	@echo "#!J! Only here for reference during intake - see platformio.ini instead!"
	@echo "NOTE: installing platform dependencies with PlatformIO"
	pio lib install 222 64 423 35 568 170 567 4271


build:
	platformio run --upload-port ${MS4_PORT}

flash:	.pio/build/ms4000/firmware.bin
	pio run && ../tools/esptool-ck/esptool -vv -cd ck -cb ${MS4_FLASH_BAUD} -cp ${MS4_PORT} -cf .pio/build/ms4000/firmware.bin

uploadfs:
	(pio run -t uploadfs -v || ../tools/esptool-ck/esptool -vv -cd ck -cb 921600 -cp /dev/ttyUSB0 -ca 200000 -cf .pio/build/ms4000/littlefs.bin)
burn:
	platformio run --target upload --upload-port ${MS4_PORT}

monitor:
	pio device monitor -b 921600 --dtr 0 --rts 0 -p ${MS4_PORT}

ota.mode:
	@echo "set ota mode"
	curl http://proto-jv.local/mode?m=7

ota: .pioenvs/ms4000/firmware.bin
	@echo "ota:"
	$(MS4_PYTHON) ${ESPOTA} -i proto-jv.local -p 8266 -f .pioenvs/ms4000/firmware.bin -d -r

reset-device:
	-@echo "^T^R" | pio device monitor --quiet -b 921600 --dtr 0 --rts 1 -p ${MS4_PORT} 
	@echo "(Inappropriate ioctl is actually appropriate for a reset.. everything is okay!)"

copy-libs:
	@cp -rfvp ../tools/nanopb/*.c lib/nanopb/
	@cp -rfvp ../tools/nanopb/*.h lib/nanopb/
	@ls -alF ../tools/nanopb/*.c lib/nanopb/*.c ../tools/nanopb/*.h lib/nanopb/*.h

pio-deps-clean:
	rm -rf .piolibdeps/

test:    proto build flash monitor

python-venv:
	sudo apt install python3-venv

python-environment:
	rm -rf .venv_firmware
	$(MS4_PYTHON) -m venv .venv_firmware
	echo "please run: . .venv_firmware/bin/activate"

python-requirements:
	@echo "Python in-use is: $(MS4_PYTHON)"
	$(MS4_PYTHON) -m pip install -r requirements.txt

# note: there is one single source for the shared MS4.proto file - at the root of the repo!
proto:  proto-clean ../MS4.proto 
	@echo "NANOPB_DIR = $(NANOPB_DIR)"
	@echo "CFLAGS = $(CFLAGS)"	
	cd src/ && $(PROTOC) $(PROTOC_OPTS) --proto_path ../.. ../../MS4.proto  -o ./MS4.pb --nanopb_out=-v:./ && $(MS4_PYTHON) ../../tools/nanopb/generator/nanopb_generator.py ./MS4.pb
 
proto-clean:
	rm -rf src/MS4.pb.? src/MS4.pb

clean-environment: proto-clean
	rm -rf .pioenvs
	rm -rf .venv_firmware

clean:
	pio run -t clean

