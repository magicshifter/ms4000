MS_PORT ?= /dev/ttyUSB0
BUILDER_NAME ?= ms4000-builder

MS4_PYTHON := $(shell which python3)

reqs-debian:
	apt install -y docker.io docker-compose 

builder: 
	docker build -t $(BUILDER_NAME) .

builder-burn:
	docker run --device /dev/ttyUSB0 -v $(shell pwd):/usr/local/ms4000-builder $(BUILDER_NAME) sh -c "make tools && cd firmware && make clean && make proto && make" 

builder-shell:
	docker run -it --device /dev/ttyUSB0 -v $(shell pwd):/usr/local/ms4000-builder $(BUILDER_NAME) /bin/bash

tools:
	. .venv_firmware/bin/activate && (\
		which pio; \
		make -C tools/esptool-ck\
	)

factory:
	make -C tools/factoryFlashing
#
# Note! The Dockerfile will use its own internal python - however, for development
# please create and use your own local python .venv with these targets:
#
install-python-venv:
	sudo apt install -y python3-venv

new-python-environment:
	rm -rf .venv_firmware
	$(MS4_PYTHON) -m venv .venv_firmware
	echo "IMPORTANT: please run . .venv_firmware/bin/activate to use the MS4000-local python environment!"

python-requirements:
	. .venv_firmware/bin/activate && ( \
		@echo "Python in-use is: $(MS4_PYTHON)"; \
		$(MS4_PYTHON) -m pip install -r firmware/requirements.txt;\
	)

activate:
	. .venv_firmware/bin/activate && (\
		which pio \
	)

python-tooling:	new-python-environment activate python-requirements
   
.PHONY: tools

all:	python-tooling tools builder factory

clean:
	make -C firmware proto-clean clean
	make -C web/app clean
