# =============================================================================
# MS4000 Firmware Makefile
# Colorful build with timestamps and emojis - CLion compatible
# vim: set ts=8 sts=8 sw=8 noet ft=make :
# =============================================================================

SHELL := /bin/bash

MS4_PYTHON = $(shell which python3)

# Configuration
MS4_FLASH_BAUD ?= 921600
MS4_PORT ?= /dev/ttyUSB0

# Auto-detect port for CP210x (Silabs) devices
MS4_PORT=`pio device list --json-output | $(MS4_PYTHON) -c 'exec("""\nimport json,sys\n\nobj=json.load(sys.stdin)\nfor y in obj:\n if "10C4:EA60" in y["hwid"].upper():\n  print (y["port"])\n""") ' | head -1`

# Fallback to default if detection fails
MS4_PORT := $(if $(MS4_PORT),$(MS4_PORT),/dev/ttyUSB0)

NANOPB_DIR = $(pwd)/../tools/nanopb/
ESPOTA = ../tools/ESP8266_ota/espota.py

# =============================================================================
# Color and Announce Macros (same style as top-level)
# =============================================================================
RED     = \033[1;31m
GREEN   = \033[1;32m
YELLOW  = \033[1;33m
CYAN    = \033[1;36m
RESET   = \033[0m

define announce
    @echo -e "$(CYAN)[$$(date '+%Y-%m-%d %H:%M:%S')] $(1)$(RESET)"
endef

define success
    @echo -e "$(GREEN)[$$(date '+%Y-%m-%d %H:%M:%S')] ✅ SUCCESS: $(1)$(RESET)"
endef

define warning
    @echo -e "$(YELLOW)[$$(date '+%Y-%m-%d %H:%M:%S')] ⚠️  $(1)$(RESET)"
endef

# =============================================================================
# Default target
# =============================================================================
all: clean copy-libs proto build flash monitor
	$(call success,Firmware full cycle completed)

# =============================================================================
# Core Build Targets
# =============================================================================
.pio/build/ms4000/firmware.bin:
build:
	$(call announce,🔨 [build] Building firmware with PlatformIO...)
	pio run --upload-port $(MS4_PORT)
	$(call success,Firmware built successfully)

flash: .pio/build/ms4000/firmware.bin
	$(call announce,⚡ Flashing firmware at $(MS4_FLASH_BAUD) baud on $(MS4_PORT)...)
	pio run && ../tools/esptool-ck/esptool -vv -cd ck -cb $(MS4_FLASH_BAUD) -cp $(MS4_PORT) -cf .pio/build/ms4000/firmware.bin
	$(call success,Firmware flashed successfully)

burn:
	$(call announce,🔥 PlatformIO upload (burn)...)
	pio run --target upload --upload-port $(MS4_PORT)
	$(call success,Burn completed)

uploadfs:
	$(call announce,📁 Uploading LittleFS filesystem...)
	(pio run -t uploadfs -v || ../tools/esptool-ck/esptool -vv -cd ck -cb 921600 -cp /dev/ttyUSB0 -ca 200000 -cf .pio/build/ms4000/littlefs.bin)
	$(call success,Filesystem uploaded)

monitor:
	$(call announce,📡 Starting serial monitor at 921600 baud...)
	pio device monitor -b 921600 --dtr 0 --rts 0 -p $(MS4_PORT)

# =============================================================================
# Proto & Dependencies
# =============================================================================
proto: proto-clean ../MS4.proto
	$(call announce,📡 Generating Nanopb protobufs from ../MS4.proto...)
	@echo "NANOPB_DIR = $(NANOPB_DIR)"
	cd src/ && ../$(NANOPB_DIR)/generator/protoc --proto_path=../.. ../../MS4.proto \
        -o ./MS4.pb --nanopb_out=-v:./ && \
        python3 ../../tools/nanopb/generator/nanopb_generator.py ./MS4.pb
	$(call success,Protobufs generated successfully)

proto-clean:
	$(call announce,🧹 Cleaning protobuf artifacts...)
	rm -rf src/MS4.pb.* src/MS4.pb
	$(call success,Proto clean completed)

copy-libs:
	$(call announce,📚 Copying NanoPB library files...)
	@cp -rfvp ../tools/nanopb/*.c lib/nanopb/
	@cp -rfvp ../tools/nanopb/*.h lib/nanopb/
	@ls -alF ../tools/nanopb/*.c lib/nanopb/*.c ../tools/nanopb/*.h lib/nanopb/*.h 2>/dev/null || true
	$(call success,NanoPB libraries copied)

# =============================================================================
# Python & Environment
# =============================================================================
python-reqs:
	$(call announce,📦 Installing Python requirements...)
	. ../.venv_ms4000/bin/activate && pip install -r requirements.txt
	$(call success,Python requirements installed)

activate:
	$(call announce,🔌 Activation instructions...)
	@echo -e "$(GREEN)Run:$(RESET)"
	@echo -e "$(CYAN)    source ../.venv_ms4000/bin/activate$(RESET)"

# =============================================================================
# OTA & Reset
# =============================================================================
ota:
	$(call announce,📡 Performing OTA update to proto-jv.local...)
	$(call warning,Make sure the device is in OTA mode first!)
	$(MS4_PYTHON) $(ESPOTA) -i proto-jv.local -p 8266 -f .pio/build/ms4000/firmware.bin -d -r
	$(call success,OTA update completed)

ota.mode:
	$(call announce,🔄 Setting device to OTA mode...)
	curl http://proto-jv.local/mode?m=7 || echo "Failed to set OTA mode (device may be offline)"

reset-device:
	$(call announce,🔄 Resetting device via serial...)
	-@echo "^T^R" | pio device monitor --quiet -b 921600 --dtr 0 --rts 1 -p $(MS4_PORT) 2>/dev/null || true
	@echo "(Inappropriate ioctl for reset is expected and harmless)"

# =============================================================================
# Maintenance
# =============================================================================
clean:
	$(call announce,🧹 Cleaning PlatformIO build artifacts...)
	-pio run -t clean
	rm -rf data/*
	$(call success,Clean completed)

clean-environment: proto-clean
	$(call announce,🗑️  Full environment clean...)
	rm -rf .pioenvs .piolibdeps .venv_ms4000
	$(call success,Environment cleaned)

package-assets:
	$(call announce,📦 Packaging firmware assets [.magicBitmaps/etc.] ...)
	mkdir -p data/
	cp -rfv assets/* data/
	$(call success,📦 Packaging firmware assets ...)

package:	package-assets
	$(call announce,📦 Packaging web+firmware assets to data ...)
	make -C ../web/app clean
	make -C ../web/app build
	make -C ../web/app package
	$(call success,📦 Packaging web+firmware assets to data ...)

assets:	package	uploadfs
	$(call announce,📦 Packaginge data assets and uploading ...)

rebuild: clean all

help:
	@echo -e "$(CYAN)════════════════════════════════════════════════════════════$(RESET)"
	@echo -e "$(WHITE)MS4000 Firmware Makefile Help$(RESET)"
	@echo -e "$(CYAN)════════════════════════════════════════════════════════════$(RESET)"
	@echo "  all                🔄  Full cycle: clean → proto → build → flash → monitor"
	@echo "  build              🔨  Compile firmware with PlatformIO"
	@echo "  flash              ⚡  Flash firmware using esptool"
	@echo "  burn               🔥  PlatformIO upload"
	@echo "  monitor            📡  Open serial monitor"
	@echo "  proto              📡  Generate Nanopb protobufs"
	@echo "  copy-libs          📚  Copy NanoPB sources"
	@echo "  clean              🧹  Clean build artifacts"
	@echo "  rebuild            ♻️   Clean + full build"
	@echo "  reset-device       🔄  Reset the device"
	@echo -e "$(CYAN)════════════════════════════════════════════════════════════$(RESET)"

# =============================================================================
# Phony targets (important for CLion and correctness)
# =============================================================================
.PHONY: all build flash burn uploadfs monitor proto proto-clean copy-libs \
        python-reqs activate ota ota.mode reset-device clean clean-environment \
        rebuild help

