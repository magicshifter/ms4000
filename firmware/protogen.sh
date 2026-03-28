#!/bin/bash
make clean && make python-environment && . .venv_firmware/bin/activate && make python-requirements && make proto
