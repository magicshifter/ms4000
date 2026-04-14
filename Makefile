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
	$(call success,[release] MS4000 top-level release build completed)

# =============================================================================
# Docker Builder Targets
# =============================================================================
 
install-python-venv:
	$(call announce,[install-python-venv] 🐍 Installing python3-venv...)
	-apt install -y python3-venv

new-python-environment: install-python-venv
	$(call announce,[new-python-environment] 🌱 Creating fresh Python virtual environment...)
	rm -rf .venv_ms4000
	$(SYS_PYTHON) -m venv .venv_ms4000
	$(call success,[new-python-environment] Virtual environment created at .venv_ms4000)
	@echo -e "$(YELLOW)IMPORTANT: Run 'source .venv_ms4000/bin/activate' to activate$(RESET)"
 
python-requirements: $(MS4_PYTHON)
	$(call announce,📥 Installing Python requirements from firmware/requirements.txt...)
	$(MS4_PYTHON) -m pip install -r firmware/requirements.txt
	$(call success,Python dependencies installed)
 
builder:
	$(call announce,[builder] 🔨 Building Docker image '$(BUILDER_NAME)'...)
	docker build \
        --build-arg UID=$(USER) \
        --build-arg GID=$(GROUP) \
        -t $(BUILDER_NAME) .
	$(call success,[builder] Docker image '$(BUILDER_NAME)' built successfully)

# Optional: force a full rebuild when you really need it
builder-force:
	docker build --no-cache -t $(BUILDER_NAME) .

builder-clean:
	$(call announce,[builder-clean] 🧹  Cleaning Docker image '$(BUILDER_NAME)'...)
	docker rmi -f $(BUILDER_NAME) 2>/dev/null || true
	docker image prune -f --filter "label=stage=builder" 2>/dev/null || true
	$(call success,[builder-clean] Docker image '$(BUILDER_NAME)' cleaned successfully)

builder-burn:
	$(call announce,[builder-burn] 🔥 Starting Docker build + burn to $(MS4_PORT)...)
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
	$(call success,[builder-burn] Firmware built and flashed via Docker)

builder-shell:
	$(call announce,[builder-shell] 🐚 Starting interactive Docker shell...)
	docker run -it \
        --device $(MS4_PORT) \
        -v $(PWD):/home/builder/workspace \
        --user $(USER):$(GROUP) \
        --group-add dialout \
        -e PLATFORMIO_CORE_DIR=/home/builder/.platformio \
        $(BUILDER_NAME) \
        /bin/bash

builder-test: builder-shell
	$(call announce,[builder-test] 🧪 builder-test alias used (same as builder-shell for now...))

# =============================================================================
# Local Development Targets
# =============================================================================
reqs-debian: install-python-venv new-python-environment python-requirements
	$(call announce,[reqs-debian] 📦 Installing Debian system requirements...)
	-apt install -y docker.io docker-compose docker-buildx protobuf-compiler
	$(call success,[reqs-debian] Debian requirements installed)


activate:
	$(call announce,🔌 Activation instructions...)
	@echo -e "$(GREEN)Run in your shell:$(RESET)"
	@echo -e "$(CYAN)    source .venv_ms4000/bin/activate$(RESET)"
	@echo -e "$(YELLOW)Then check PlatformIO: 'which pio'$(RESET)"

# =============================================================================
# Tools and Factory
# =============================================================================
tools:
	$(call announce,🛠️  Building tools...)
	which pio || $(call announce, PlatformIO pio not found in PATH)
	make -C tools/esptool-ck
	$(call success,Tools built)

factory:
	$(call announce,🏭 Building factory flashing tools...)
	make -C tools/factoryFlashing
	$(call success,Factory flashing tools built)

# =============================================================================
# Sub-project Targets (for CLion visibility)
# =============================================================================
firmware:
	$(call announce,📟 Building firmware...)
	make -C firmware
	$(call success,Firmware build completed)

firmware-clean:
	$(call announce,🧹 Cleaning firmware...)
	make -C firmware clean

firmware-proto:
	$(call announce,📡 Generating firmware protobufs...)
	make -C firmware proto

web-deps:
	$(call announce,🌐 Installing web dependencies ...)
	make -C web/app deps
	$(call success,Web app built)

web:	web-deps
	$(call announce,🌐 Building web application...)
	make -C web/app
	make -C web/app package
	$(call success,Web app built)

web-clean:
	$(call announce,🧹 Cleaning web app...)
	make -C web/app clean

# =============================================================================
# Maintenance
# =============================================================================
clean:
	$(call announce,🧹 Starting full clean...)
	make -C firmware clean proto-clean
	make -C web/app clean
	$(call success,Full clean completed)

assets:
	$(call announce, 📦  Create firmware data/ assets (including web/app bundle))
	make -C firmware/ assets

package:	web assets
	$(call announce, 📦  Build firmware package (includes web/app bundle plus magicBitmaps, etc.))
	make -C firmware 

rebuild: clean all

help:
	@echo -e "$(CYAN)════════════════════════════════════════════════════════════$(RESET)"
	@echo -e "$(WHITE)MS4000 Makefile Help$(RESET)"
	@echo -e "$(CYAN)════════════════════════════════════════════════════════════$(RESET)"
	@echo "  all                🔨  Build everything (default)"
	@echo "  builder            🐳  Build Docker builder image"
	@echo "  builder-burn       🔥  Docker build + flash firmware"
	@echo "  builder-shell      🐚  Interactive Docker shell"
	@echo "  tools              🛠️   Build development tools"
	@echo "  factory            🏭  Build factory flashing tools"
	@echo "  firmware           📟  Build firmware only"
	@echo "  web                🌐  Build web app"
	@echo "  assets             📦  Install firmware data/ assets (including web/app bundle)"
	@echo "  clean              🧹  Clean all artifacts"
	@echo "  rebuild            ♻️   Clean + full build"
	@echo "  python-requirements 📥 Install local Python deps"
	@echo "  reqs-debian        📦  Install system packages"
	@echo -e "$(CYAN)════════════════════════════════════════════════════════════$(RESET)"

# =============================================================================
# Phony targets (essential for CLion and make correctness)
# =============================================================================
.PHONY: all builder builder-burn builder-shell builder-test \
        reqs-debian install-python-venv new-python-environment \
        python-requirements activate \
        tools factory firmware firmware-clean firmware-proto \
        web web-clean clean rebuild help assets

# vim: set ts=8 sts=8 sw=8 noet ft=make :
