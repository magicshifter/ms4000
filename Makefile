MS_PORT ?= /dev/ttyUSB0
BUILDER_NAME ?= ms4000-builder
PWD := $(shell pwd)
USER := $(shell id -u)
GROUP := $(shell id -g)

SYS_PYTHON := $(shell which python3)
MS4_PYTHON = .venv_ms4000/bin/python3

builder:
	docker build -t $(BUILDER_NAME) .

builder-burn:
	docker run 	--device /dev/ttyUSB0 \
			-v $(PWD):/home/builder/workspace \
			--user $(USER):$(GROUP) \
                        --group-add dialout \
                        -e PLATFORMIO_CORE_DIR=/home/builder/.platformio \
			$(BUILDER_NAME) \
			sh -c "\
			make tools \
			&& cd firmware \
			&& make clean \
			&& make proto \
			&& make \
			"

builder-shell:
	docker run 	-it \
			--device /dev/ttyUSB0 \
			-v $(PWD):/home/builder/workspace \
			--user $(USER):$(GROUP) \
			--group-add dialout \
			-e PLATFORMIO_CORE_DIR=/home/builder/.platformio \
			$(BUILDER_NAME) \
			/bin/bash

builder-test:
	docker run 	-it \
			--device /dev/ttyUSB0 \
			-v $(PWD):/home/builder/workspace \
			--user $(USER):$(GROUP) \
			--group-add dialout \
			$(BUILDER_NAME) \
			/bin/bash


reqs-debian:	install-python-venv new-python-environment python-requirements
	-apt install -y docker.io docker-compose docker-buildx
 
tools:
	which pio; \
	make -C tools/esptool-ck\

factory:
	make -C tools/factoryFlashing
#
# Note! The Dockerfile will use its own internal python - however, for development
# please create and use your own local python .venv with these targets:
#
install-python-venv:
	-apt install -y python3-venv

new-python-environment:	install-python-venv
	rm -rf .venv_ms4000
	$(SYS_PYTHON) -m venv .venv_ms4000
	echo "IMPORTANT: please run . .venv_ms4000/bin/activate to use the MS4000-local python environment!"

$(MS4_PYTHON):	new-python-environment

python-requirements: $(MS4_PYTHON)
	$(MS4_PYTHON) -m pip install -r firmware/requirements.txt;\

activate:
	. .venv_ms4000/bin/activate && (\
		which pio \
	)

.PHONY: tools

all:	python-tooling tools builder factory

clean:
	make -C firmware/ clean proto-clean
	make -C web/app clean
