# =============================================================================
# MS4000 Firmware Makefile
# vim: set ts=8 sts=8 sw=8 noet ft=make :
# =============================================================================


include ../environment.ms4

# =============================================================================
# Default target
# =============================================================================
all: clean copy-libs proto package build-filesystem build-firmware flash 
	$(call success,Firmware full cycle completed)

# =============================================================================
# Core Build Targets
# =============================================================================
.pio/build/ms4000/firmware.bin:
build-firmware:
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
        $(MS4_PYTHON) ../../tools/nanopb/generator/nanopb_generator.py ./MS4.pb
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
	$(call success,Clean completed)

clean-environment: proto-clean
	$(call announce,🗑️  Full environment clean...)
	rm -rf .pioenvs .piolibdeps .venv_ms4000
	$(call success,Environment cleaned)

# Packaged Assets: pre-existing things we want to package in the filesystem
package-assets:
	$(call announce,📦 Packaging firmware assets [.magicBitmaps/etc.] ...)
	mkdir -p data/
	cp -rfv assets/* data/
	$(call success,📦 Packaging firmware assets ...)

# Packaged Web: Assets, plus a fresh build of the Web App
# Note: web/app has dependencies on packaged assets!
package-web:	package-assets
	$(call announce,📦 Packaging web+firmware assets to data/ ...)
	make -C ../web/app clean
	make -C ../web/app build
	make -C ../web/app package
	$(call success,📦 Packaging web+firmware assets to data/ ...)

# Completely clean data/
clean-data:
	$(call announce,🗑️  ALL assets cleaned from data/ ...)
	rm -rf data/*
	$(call success,Environment cleaned)

# Make a package from fresh assets and web
package:	package-assets package-web
	$(call success,📦 Packaging web+firmware assets to data/ ...)
  
# buildfs: Build the filesystem from the current contents of the existing data/ directory
build-filesystem:
	$(call announce,📦 Building filesystem.bin from current contents of data/ ...)
	(pio run -t buildfs -v)
	find .pio/build/ms4000 -name "*.bin" -exec ls -alF {} \;

# Upload filesystem to connected shifter
uploadfs:
	$(call announce,📁 Uploading LittleFS filesystem...)
	(pio run -t uploadfs -v || ../tools/esptool-ck/esptool -vv -cd ck -cb 921600 -cp /dev/ttyUSB0 -ca 200000 -cf .pio/build/ms4000/littlefs.bin)
	$(call success,Filesystem uploaded)

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
	@echo "  buildfs            📦  Build filesystem.bin from assets in data/"
	@echo "  assets             📦  Package clean assets from data/ data/"
	@echo "  copy-libs          📚  Copy NanoPB sources"
	@echo "  rebuild            ♻️   Clean + full build"
	@echo "  clean              🧹  Clean build artifacts"
	@echo "  clean-data         🧹  Clean data/ assets (Achtung!)"
	@echo "  reset-device       🔄  Reset the device"
	@echo -e "$(CYAN)════════════════════════════════════════════════════════════$(RESET)"

# =============================================================================
# Phony targets (important for CLion and correctness)
# =============================================================================
.PHONY: all build flash burn uploadfs monitor proto proto-clean copy-libs \
        python-reqs activate ota ota.mode reset-device clean clean-environment \
        rebuild help

# vim: set ts=8 sts=8 sw=8 noet ft=make :
