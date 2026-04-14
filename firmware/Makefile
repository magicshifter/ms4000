# =============================================================================
# MS4000 Firmware Makefile
# vim: set ts=8 sts=8 sw=8 noet ft=make :
# =============================================================================

include ../environment.ms4

# =============================================================================
# Phony targets
# =============================================================================
.PHONY: all build-firmware flash burn monitor proto proto-clean copy-libs \
		python-reqs activate ota ota.mode reset-device clean clean-environment \
		package-assets package-web clean-data package assets build-filesystem uploadfs \
		rebuild help

# =============================================================================
# Default target
# =============================================================================
all: proto package build-filesystem build-firmware
	$(call success,✅ [all] Firmware full cycle completed)
	find .pio/build/ms4000 -name "*.bin" -exec ls -alF {} \;

# =============================================================================
# Core Build Targets
# =============================================================================
build-firmware:
	$(call announce,🔨 [build-firmware] Build firmware with PlatformIO...)
	pio run --upload-port $(MS4_PORT)
	$(call success,✅ [build-firmware] Firmware built successfully)

flash: build-firmware
	$(call announce,⚡ [flash] Flash firmware at $(MS4_BAUD_RATE) baud on $(MS4_PORT)...)
	../tools/esptool-ck/esptool -vv -cd ck -cb $(MS4_BAUD_RATE) -cp $(MS4_PORT) -cf .pio/build/ms4000/firmware.bin
	$(call success,✅ [flash] Firmware flashed successfully)

burn:
	$(call announce,🔥 [burn] PlatformIO upload (burn)...)
	pio run --target upload --upload-port $(MS4_PORT)
	$(call success,✅ [burn] Burn completed)

monitor:
	$(call announce,📡 [monitor] Start serial monitor at $(MS4_BAUD_RATE) baud...)
	pio device monitor -b 921600 --dtr 0 --rts 0 -p $(MS4_PORT)

# =============================================================================
# Proto & Dependencies
# =============================================================================
proto: proto-clean ../MS4.proto
	$(call announce,📡 [proto] Generate Nanopb protobufs from ../MS4.proto...)
	@echo "NANOPB_DIR = $(NANOPB_DIR)"
	cd src/ && $(NANOPB_DIR)/generator/protoc --proto_path=../.. ../../MS4.proto \
		-o ./MS4.pb --nanopb_out=-v:./ && \
		$(MS4_PYTHON) ../../tools/nanopb/generator/nanopb_generator.py ./MS4.pb
	$(call success,✅ [proto] Protobufs generated successfully)

proto-clean:
	$(call announce,🧹 [proto-clean] Clean protobuf artifacts...)
	rm -rf src/MS4.pb.* src/MS4.pb
	$(call success,✅ [proto-clean] Proto clean completed)

copy-libs:
	$(call announce,📚 [copy-libs] Copy NanoPB library files...)
	@cp -rfvp ../tools/nanopb/*.c lib/nanopb/
	@cp -rfvp ../tools/nanopb/*.h lib/nanopb/
	@ls -alF ../tools/nanopb/*.c lib/nanopb/*.c ../tools/nanopb/*.h lib/nanopb/*.h 2>/dev/null || true
	$(call success,✅ [copy-libs] NanoPB libraries copied)

# =============================================================================
# Python & Environment
# =============================================================================
python-reqs:
	$(call announce,📦 [python-reqs] Install Python requirements...)
	. ../.venv_ms4000/bin/activate && pip install -r requirements.txt
	$(call success,✅ [python-reqs] Python requirements installed)

activate:
	$(call announce,🔌 [activate] Activation instructions...)
	@echo "$(GREEN)Run:$(RESET)"
	@echo "$(CYAN)	source ../.venv_ms4000/bin/activate$(RESET)"

# =============================================================================
# OTA & Reset
# =============================================================================
ota:
	$(call announce,📡 [ota] Perform OTA update to proto-jv.local...)
	$(call warning,Make sure the device is in OTA mode first!)
	$(MS4_PYTHON) $(ESPOTA) -i proto-jv.local -p 8266 -f .pio/build/ms4000/firmware.bin -d -r
	$(call success,✅ [ota] OTA update completed)

ota.mode:
	$(call announce,🔄 [ota.mode] Set device to OTA mode...)
	curl http://proto-jv.local/mode?m=7 || echo "Failed to set OTA mode (device may be offline)"

reset-device:
	$(call announce,🔄 [reset-device] Reset device via serial...)
	-@echo "^T^R" | pio device monitor --quiet -b $(MS4_BAUD_RATE) --dtr 0 --rts 1 -p $(MS4_PORT) 2>/dev/null || true
	@echo "(Inappropriate ioctl for reset is expected and harmless)"

# =============================================================================
# Maintenance
# =============================================================================
clean:
	$(call announce,🧹 [clean] Clean PlatformIO build artifacts...)
	-pio run -t clean
	$(call success,✅ [clean] Clean completed)

clean-environment: proto-clean
	$(call announce,🗑️ [clean-environment] Full environment clean...)
	rm -rf .pio .pioenvs .piolibdeps .venv_ms4000
	$(call success,✅ [clean-environment] Environment cleaned)

# =============================================================================
# Assets & Packaging
# =============================================================================
package-assets:
	$(call announce,📦 [package-assets] Package firmware assets [.magicBitmaps/etc.] ...)
	mkdir -p data/
	cp -rfv assets/* data/
	$(call success,✅ [package-assets] Package firmware assets completed)

package-web: package-assets
	$(call announce,📦 [package-web] Package web+firmware assets to data/ ...)
	make -C ../web/app clean
	make -C ../web/app build
	make -C ../web/app package
	$(call success,✅ [package-web] Package web+firmware assets completed)

clean-data:
	$(call announce,🗑️ [clean-data] ALL assets cleaned from data/ ...)
	rm -rf data/*
	$(call success,✅ [clean-data] Data directory cleaned)

assets: package-assets
	$(call announce,📦 [assets] Alias for packaging firmware assets...)
	$(call success,✅ [assets] Assets packaged (alias used))

package: package-assets package-web
	$(call success,✅ [package] Package web+firmware assets to data/ completed)

build-filesystem: package
	$(call announce,📦 [build-filesystem] Build littlefs.bin from current contents of data/ ...)
	pio run -t buildfs -v
	find .pio/build/ms4000 -name "*.bin" -exec ls -alF {} \;
	$(call success,✅ [build-filesystem] Filesystem image built)

uploadfs:
	$(call announce,📁 [uploadfs] Upload littlefs.bin filesystem...)
	pio run -t uploadfs -v || ../tools/esptool-ck/esptool -vv -cd ck -cb $(MS4_BAUD_RATE) -cp /dev/ttyUSB0 -ca 0x200000 -cf .pio/build/ms4000/littlefs.bin
	$(call success,✅ [uploadfs] Filesystem uploaded)

rebuild: clean all

# =============================================================================
# Help
# =============================================================================
help:
	@echo "$(CYAN)═════════════════════════════════════════════════════════════════════════════════$(RESET)"
	@echo "$(WHITE)MS4000 Firmware Makefile Help$(RESET)"
	@echo "$(CYAN)═════════════════════════════════════════════════════════════════════════════════$(RESET)"
	@echo ""
	@echo "Purpose: Build MagicShifter firmware, protobufs, assets, and LittleFS image."
	@echo "         Called from top-level Makefile for full release."
	@echo ""
	@echo "Usage: make <target>"
	@echo ""
	@echo "	🐍 [all]                  Full cycle: package → build-filesystem → build-firmware"
	@echo "	🔨 [build-firmware]       Build firmware with PlatformIO"
	@echo "	⚡ [flash]                Flash firmware using esptool"
	@echo "	🔥 [burn]                 PlatformIO upload"
	@echo "	📡 [monitor]              Start serial monitor"
	@echo "	📡 [proto]                Generate Nanopb protobufs"
	@echo "	🧹 [proto-clean]          Clean protobuf artifacts"
	@echo "	📚 [copy-libs]            Copy NanoPB library files"
	@echo "	📦 [python-reqs]          Install Python requirements"
	@echo "	🔌 [activate]             Show venv activation command"
	@echo "	📡 [ota]                  Perform OTA update"
	@echo "	🔄 [ota.mode]             Set device to OTA mode"
	@echo "	🔄 [reset-device]         Reset device via serial"
	@echo "	🧹 [clean]                Clean PlatformIO build artifacts"
	@echo "	🗑️ [clean-environment]    Full environment clean"
	@echo "	📦 [package-assets]       Copy assets to data/"
	@echo "	📦 [package-web]          Build & package web assets"
	@echo "	📦 [assets]               Alias: package firmware assets"
	@echo "	📦 [package]              Package everything to data/"
	@echo "	📦 [build-filesystem]     Build LittleFS image"
	@echo "	📁 [uploadfs]             Upload LittleFS image"
	@echo "	✅ [rebuild]              clean + all"
	@echo ""
	@echo "$(CYAN)═════════════════════════════════════════════════════════════════════════════════$(RESET)"
	@echo "Python: $(MS4_PYTHON)"
	@echo "Project root: $(PROJECT_ROOT)"
	@echo "$(CYAN)═════════════════════════════════════════════════════════════════════════════════$(RESET)"

# vim: set ts=8 sts=8 sw=8 noet ft=make :
