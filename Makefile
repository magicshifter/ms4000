# =============================================================================
# MS4000 Top-Level Makefile
# vim: set ts=8 sts=8 sw=8 noet ft=make :
# =============================================================================

include environment.ms4

# =============================================================================
# Phony targets
# =============================================================================
.PHONY: all release builder builder-force builder-clean builder-burn builder-shell builder-test \
		reqs-debian install-python-venv new-python-environment python-requirements activate \
		tools factory firmware firmware-clean firmware-proto \
		web web-deps web-clean assets package clean rebuild help

# =============================================================================
# Default target (you can change this to "release" if preferred)
# =============================================================================
all: release

# =============================================================================
# Top-Level Release Target
# =============================================================================
release: tools builder package
	make -C firmware flash
	$(call success,✅ [release] MS4000 top-level release build + flash completed)

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
	$(call announce,📥 [python-requirements] Install Python requirements...)
	$(MS4_PYTHON) -m pip install -r firmware/requirements.txt
	$(call success,✅ [python-requirements] Python dependencies installed)

builder:
	$(call announce,🔨 [builder] Build Docker image '$(BUILDER_NAME)'...)
	docker build \
		--build-arg UID=$(USER) \
		--build-arg GID=$(GROUP) \
		-t $(BUILDER_NAME) .
	$(call success,✅ [builder] Docker image '$(BUILDER_NAME)' built successfully)

builder-force:
	$(call announce,🔄 [builder-force] Force full Docker rebuild (no cache)...)
	docker build --no-cache -t $(BUILDER_NAME) .
	$(call success,✅ [builder-force] Docker image rebuilt without cache)

builder-clean:
	$(call announce,🧹 [builder-clean] Clean Docker image '$(BUILDER_NAME)'...)
	docker rmi -f $(BUILDER_NAME) 2>/dev/null || true
	docker image prune -f --filter "label=stage=builder" 2>/dev/null || true
	$(call success,✅ [builder-clean] Docker image cleaned)

builder-burn:
	$(call announce,🔥 [builder-burn] Docker build + flash to $(MS4_PORT)...)
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
			make -C web/app deps && \
			make -C firmware && \
			make -C firmware flash && \
			make -C firmware uploadfs \
		"
	$(call success,✅ [builder-burn] Firmware built and flashed via Docker)

builder-shell:
	$(call announce,🐚 [builder-shell] Starting interactive Docker shell...)
	docker run -it \
		--device $(MS4_PORT) \
		-v $(PWD):/home/builder/workspace \
		--user $(USER):$(GROUP) \
		--group-add dialout \
		-e PLATFORMIO_CORE_DIR=/home/builder/.platformio \
		$(BUILDER_NAME) \
		/bin/bash

builder-test: builder-shell
	$(call announce,🧪 [builder-test] (alias for builder-shell))

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
	@echo "$(CYAN)	source .venv_ms4000/bin/activate$(RESET)"
	@echo "$(YELLOW)Then verify: 'which pio'$(RESET)"

# =============================================================================
# Tools and Factory
# =============================================================================
tools:
	$(call announce,🛠️  [tools] Building build tools...)
	make -C tools/esptool-ck
	$(call success,✅ [tools] Tools built successfully)

factory:
	$(call announce,🏭 [factory] Building factory flashing tools...)
	make -C tools/factoryFlashing
	$(call success,✅ [factory] Factory flashing tools built)

# =============================================================================
# Sub-project Targets
# =============================================================================
firmware:
	$(call announce,📟 [firmware] Building firmware subsystem...)
	make -C firmware all
	$(call success,✅ [firmware] Firmware build completed)

firmware-clean:
	$(call announce,🧹 [firmware-clean] Cleaning firmware...)
	make -C firmware clean

firmware-proto:
	$(call announce,📡 [firmware-proto] Generating protobufs...)
	make -C firmware proto

web-deps:
	$(call announce,🌐 [web-deps] Installing web dependencies...)
	make -C web/app deps
	$(call success,✅ [web-deps] Web dependencies installed)

web: web-deps
	$(call announce,🌐 [web] Building web application...)
	make -C web/app
	make -C web/app package
	$(call success,✅ [web] Web application built and packaged)

web-clean:
	$(call announce,🧹 [web-clean] Cleaning web app...)
	make -C web/app clean

# =============================================================================
# Maintenance & Packaging
# =============================================================================
clean:
	$(call announce,🧹 [clean] Full project clean...)
	make -C firmware clean
	make -C web/app clean
	$(call success,✅ [clean] Full clean completed)

assets:
	$(call announce,📦 [assets] Packaging firmware assets (including web bundle)... )
	make -C firmware assets
	$(call success,✅ [assets] Assets prepared)

package: web assets
	$(call announce,📦 [package] Building complete firmware package...)
	make -C firmware
	$(call success,✅ [package] Firmware package completed)

rebuild: clean release

# =============================================================================
# Help
# =============================================================================
help:
	@echo "$(CYAN)═════════════════════════════════════════════════════════════════════════════════$(RESET)"
	@echo "$(WHITE)MS4000 Top-Level Makefile Help$(RESET)"
	@echo "$(CYAN)═════════════════════════════════════════════════════════════════════════════════$(RESET)"
	@echo ""
	@echo "Purpose: Manage Docker builder, tools, web app, and full firmware release."
	@echo ""
	@echo "Usage: make <target>"
	@echo ""
	@echo " 🐍 [install-python-venv]     Install python3-venv"
	@echo " 🌱 [new-python-environment]  Create fresh Python venv"
	@echo " 📥 [python-requirements]     Install Python requirements"
	@echo " 🔨 [builder]                 Build Docker builder image"
	@echo " 🔄 [builder-force]           Force Docker rebuild (no cache)"
	@echo " 🧹 [builder-clean]           Remove Docker builder image"
	@echo " 🔥 [builder-burn]            Docker build + flash directly"
	@echo " 🐚 [builder-shell]           Interactive Docker shell"
	@echo " 📦 [reqs-debian]             Install Debian system packages"
	@echo " 🔌 [activate]                Show venv activation command"
	@echo " 🛠️  [tools]                   Build esptool-ck and tools"
	@echo " 🏭 [factory]                 Build factory flashing tools"
	@echo " 📟 [firmware]                Build firmware (full)"
	@echo " 🌐 [web]                     Build and package web app"
	@echo " 📦 [assets]                  Package assets into firmware data/"
	@echo " 📦 [package]                 Full package (web + assets + firmware)"
	@echo " ✅ [release]                 Full release: tools + builder + package + flash"
	@echo " 🧹 [clean]                   Clean firmware and web"
	@echo " ✅ [rebuild]                 clean + release"
	@echo ""
	@echo "$(CYAN)═════════════════════════════════════════════════════════════════════════════════$(RESET)"
	@echo "Python: $(MS4_PYTHON)"
	@echo "Project root: $(PROJECT_ROOT)"
	@echo "$(CYAN)═════════════════════════════════════════════════════════════════════════════════$(RESET)"

dry-run:
	$(call announce, --- DRY-RUN BUILD PATH - TOOLING:)
	@make -n -e | grep -i "\[ms4000-build"  | sh
	$(call announce, --- DRY-RUN BUILD PATH - FIRMWARE:)
	@make -n -e -C firmware/ | grep -i "\[ms4000-build"  | sh
# vim: set ts=8 sts=8 sw=8 noet ft=make :
