# =============================================================================
# MS4000 Top-Level Makefile
# vim: set ts=8 sts=8 sw=8 noet ft=make :
# =============================================================================

include environment.ms4

# =============================================================================
# Default target
# =============================================================================
release: tools builder package
	make -C firmware/ flash
	$(call success,✅ [release] MS4000 top-level release build completed)

# =============================================================================
# Docker Builder Targets
# =============================================================================

install-python-venv:
	$(call announce,🐍 [install-python-venv] Install python3-venv...)
	-apt install -y python3-venv

new-python-environment: install-python-venv
	$(call announce,🌱 [new-python-environment] Create fresh Python virtual environment...)
	rm -rf .venv_ms4000
	$(SYS_PYTHON) -m venv .venv_ms4000
	$(call success,✅ [new-python-environment] Virtual environment created at .venv_ms4000)
	@echo "$(YELLOW)IMPORTANT: Run 'source .venv_ms4000/bin/activate' to activate$(RESET)"

python-requirements: $(MS4_PYTHON)
	$(call announce,📥 [python-requirements] Install Python requirements from firmware/requirements.txt...)
	$(MS4_PYTHON) -m pip install -r firmware/requirements.txt
	$(call success,✅ [python-requirements] Python dependencies installed)

builder:
	$(call announce,🔨 [builder] Build Docker image '$(BUILDER_NAME)'...)
	docker build \
		--build-arg UID=$(USER) \
		--build-arg GID=$(GROUP) \
		-t $(BUILDER_NAME) .
	$(call success,✅ [builder] Docker image '$(BUILDER_NAME)' built successfully)

# Optional: force a full rebuild when you really need it
builder-force:
	$(call announce,🔄 [builder-force] Force full Docker rebuild (no cache)...)
	docker build --no-cache -t $(BUILDER_NAME) .

builder-clean:
	$(call announce,🧹 [builder-clean] Clean Docker image '$(BUILDER_NAME)'...)
	docker rmi -f $(BUILDER_NAME) 2>/dev/null || true
	docker image prune -f --filter "label=stage=builder" 2>/dev/null || true
	$(call success,✅ [builder-clean] Docker image '$(BUILDER_NAME)' cleaned successfully)

builder-burn:
	$(call announce,🔥 [builder-burn] Start Docker build + burn to $(MS4_PORT)...)
	docker run \
		--device $(MS4_PORT) \
		-v $(PWD):/home/builder/workspace \
		--user $(USER):$(GROUP) \
		--group-add dialout \
		-e PLATFORMIO_CORE_DIR=/home/builder/.platformio \
		$(BUILDER_NAME) \
		sh -c "\
			make -C tools/esptool-ck clean && \
			make -C tools/esptool-ck && \
			make -C firmware clean && \
			make -C firmware proto && \
			make -C web/app clean && \
			make -C web/app deps && \
			make -C web/app build && \
			make -C web/app package && \
			make -C firmware assets && \
			make -C firmware flash \
		"
	$(call success,✅ [builder-burn] Firmware built and flashed via Docker)

builder-shell:
	$(call announce,🐚 [builder-shell] Start interactive Docker shell...)
	docker run -it \
		--device $(MS4_PORT) \
		-v $(PWD):/home/builder/workspace \
		--user $(USER):$(GROUP) \
		--group-add dialout \
		-e PLATFORMIO_CORE_DIR=/home/builder/.platformio \
		$(BUILDER_NAME) \
		/bin/bash

builder-test: builder-shell
	$(call announce,🧪 [builder-test] builder-test alias used (same as builder-shell for now...))

# =============================================================================
# Local Development Targets
# =============================================================================
reqs-debian: install-python-venv new-python-environment python-requirements
	$(call announce,📦 [reqs-debian] Install Debian system requirements...)
	-apt install -y docker.io docker-compose docker-buildx protobuf-compiler
	$(call success,✅ [reqs-debian] Debian requirements installed)

activate:
	$(call announce,🔌 [activate] Activation instructions...)
	@echo "$(GREEN)Run in your shell:$(RESET)"
	@echo "$(CYAN)	 source .venv_ms4000/bin/activate$(RESET)"
	@echo "$(YELLOW)Then check PlatformIO: 'which pio'$(RESET)"

# =============================================================================
# Tools and Factory
# =============================================================================
tools:
	$(call announce,🛠️ [tools] Build the build tools...)
	@which pio || $(call announce,⚠️ [tools] PlatformIO pio not found in PATH)
	make -C tools/esptool-ck
	$(call success,✅ [tools] Tools built)

factory:
	$(call announce,🏭 [factory] Build factory flashing tools...)
	make -C tools/factoryFlashing
	$(call success,✅ [factory] Factory flashing tools built)

# =============================================================================
# Sub-project Targets
# ============================================================================
firmware:
	$(call announce,📟 [firmware] Build firmware...)
	make -C firmware all
	$(call success,✅ [firmware] Firmware build completed)

firmware-clean:
	$(call announce,🧹 [firmware-clean] Clean firmware...)
	make -C firmware clean

firmware-proto:
	$(call announce,📡 [firmware-proto] Generate firmware protobufs...)
	make -C firmware proto

web-deps:
	$(call announce,🌐 [web-deps] Install web dependencies...)
	make -C web/app deps
	$(call success,✅ [web-deps] Web app built)

web: web-deps
	$(call announce,🌐 [web] Build web application...)
	make -C web/app
	make -C web/app package
	$(call success,✅ [web] Web app built)

web-clean:
	$(call announce,🧹 [web-clean] Clean web app...)
	make -C web/app clean

# =============================================================================
# Maintenance
# =============================================================================
clean:
	$(call announce,🧹 [clean] Start full clean...)
	make -C firmware clean proto-clean
	make -C web/app clean
	$(call success,✅ [clean] Full clean completed)

assets:
	$(call announce,📦 [assets] Create firmware data/ assets (including web/app bundle))
	make -C firmware/ assets

package: web assets
	$(call announce,📦 [package] Build firmware package (includes web/app bundle plus magicBitmaps, etc.))
	make -C firmware 

rebuild: clean all

# =============================================================================
# Help
# =============================================================================
help:
	@echo "$(CYAN)═════════════════════════════════════════════════════════════════════════════════$(RESET)"
	@echo "$(WHITE)MS4000 Main Project Makefile Help$(RESET)"
	@echo "$(CYAN)═════════════════════════════════════════════════════════════════════════════════$(RESET)"
	@echo ""
	@echo "$(GREEN)═════════════════════════════════════════════════════════════════════════════════$(RESET)"
	@echo "Purpose: To build the builder environment (Docker), tools, and the"
	@echo "         complete MagicShifter OS: firmware, assets, web app."
	@echo "$(GREEN)═════════════════════════════════════════════════════════════════════════════════$(RESET)"
	@echo ""
	@echo "Usage: make <target>"
	@echo ""
	@echo "	🐍 [install-python-venv] Install python3-venv"
	@echo "	🌱 [new-python-environment] Create fresh Python virtual environment"
	@echo "	📥 [python-requirements] Install Python requirements from firmware/requirements.txt"
	@echo "	🔨 [builder] Build Docker image '$(BUILDER_NAME)'"
	@echo "	🔄 [builder-force] Force full Docker rebuild (no cache)"
	@echo "	🧹 [builder-clean] Clean Docker image '$(BUILDER_NAME)'"
	@echo "	🔥 [builder-burn] Start Docker build + burn to $(MS4_PORT)"
	@echo "	🐚 [builder-shell] Start interactive Docker shell"
	@echo "	🧪 [builder-test] builder-test alias used (same as builder-shell for now)"
	@echo "	📦 [reqs-debian] Install Debian system requirements"
	@echo "	🔌 [activate] Activation instructions"
	@echo "	🛠️  [tools] Build the build tools"
	@echo "	🏭 [factory] Build factory flashing tools"
	@echo "	📟 [firmware] Build firmware"
	@echo "	🧹 [firmware-clean] Clean firmware"
	@echo "	📡 [firmware-proto] Generate firmware protobufs"
	@echo "	🌐 [web-deps] Install web dependencies"
	@echo "	🌐 [web] Build web application"
	@echo "	🧹 [web-clean] Clean web app"
	@echo "	🧹 [clean] Start full clean"
	@echo "	📦 [assets] Create firmware data/ assets (including web/app bundle)"
	@echo "	📦 [package] Build firmware package (includes web/app bundle plus magicBitmaps, etc.)"
	@echo "	✅ [release] MS4000 top-level release build completed"
	@echo "\n$(CYAN)═════════════════════════════════════════════════════════════════════════════════$(RESET)"
	@echo "(Python found at: $(MS4_PYTHON)"
	@echo " .. would use $(MS4_VENV_PYTHON) at "
	@echo " .. project root: $(PROJECT_ROOT) if possible)"
	@echo "\n$(CYAN)═════════════════════════════════════════════════════════════════════════════════$(RESET)"

# =============================================================================
# Phony targets (essential for CLion and make correctness)
# =============================================================================
.PHONY: all builder builder-burn builder-shell builder-test builder-force \
		reqs-debian install-python-venv new-python-environment \
		python-requirements activate \
		tools factory firmware firmware-clean firmware-proto \
		web web-clean web-deps clean rebuild help assets package

# vim: set ts=8 sts=8 sw=8 noet ft=make :
