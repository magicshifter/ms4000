## MS4000 - the return of MagicShifter (not -3000)!

This is the source code for the MS4000 - the next-generation version of the MagicShifter Light Synthesizer!  (http://magicshifter.net/)

This is a mono-repo.. This repository contains the Magicshifter OS, built with platformio, and includes assets and tooling to get a MagicShifter firmware built and installed.

## How to build?
 * [install platformio](http://platformio.org/#!/get-started) - you will need the command-line (pio) tooling, but may have success with the PlatformIO IDE
 * Developers: Edit the firmware in firmware/src, Build the firmware: `cd firmware && make` - see also other targets in firmware/Makefile (i.e. 'make flash', etc.)
 * We are using protocol-buffers to synchronize state between the firmware and the web - the web interface is generated from the protobuf definition, which is included as .h/.c in the firmware.  'make proto' targets abound!

## Dependencies
 * remember to read the Makefile
 * `make pio-deps` can get you started... don't forget to see the 'copy-libs' target, too: nanopb is generated from our MS4.proto file
 * default make target copies libraries (copy-libs) that are auto-generated from the protobuffer target, into lib/nanopb

## Example set up for Ubuntu:

	* we get ubuntu ready:
		`apt install protobuf-compiler python-protobuf python3-pip git build-essential`

	* (recommended) set up a python venv and activate it:
		`mkdir MS4000_workdir && python3 -m venv .venv && . .venv/bin/activate`

	* we install some python tools - platformio and protocol buffers:
		`pip3 install platformio python3-protobuf grpcio_tools`

	* clone the repo:
		`git clone https://github.com/magicshifter/MS4000.git MS4000.clone`

	* set up nanopb:
		`cd MS4000.clone/tools/nanopb/generator/proto && make && cd -`

	* build the MS3KOS - default target also flashes to a connected MagicShifter:
		`cd MS4000.clone/firmware/ && make`

	* you may need to give yourself access to the callout group, where USERNAME is of course your own user name:
		`sudo adduser USERNAME dialout`

	* Please note, depending on your needs, you may need to do:

		`export PROTOCOL_BUFFERS_PYTHON_IMPLEMENTATION=python make proto # may be necessary`

	* Note: the firmware uses protocol buffers to communicate between firmware and web instances.  To buid proto:

		`make proto`

## You will need PlatformIO:

PlatformIO allows us to very easily manage project dependencies and complete fully cross-platform builds for multiple system types, which should - theoretically - be automatically set up when you first run a make on the newly cloned repository.

	http://platformio.org

	* MacOS:
		`brew install platformio` 

	* Linux: Set up venv, install python 3.7 (or so..), then:
		`pip install platformio`

## Device for the MagicShifter

Since we use the ESP8266, we have a nice UART available.  Perhaps you will need a driver to get the serial device for your system - please install the popular siLABs usb-uart driver, its what we use also (not necessary on Linux):  

	https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers

On Linux, the device shows up automatically as /dev/ttyUSB0, on MacOS, it might be something different (e.g. "/dev/tty.SLAB_USBtoUART") - but either way, the command:

		`pio device list`

.. will show you what your system sees, device-wise.
	
## Development Environment

Linux is great, but we also take care to make sure we can also still use MacOS to do builds, too - after all, platformIO solves a lot of the tooling issues.  If you have a contribution, please keep in mind our desire to keep things cross-platform; and we'd love to have tooling for Windows, alas none of us use it.

## To Factory-flash your MagicShifter firmware:

	In this current directory, type: `make factory`

## Debugging Monitor:

	make -C firmware monitor

## Pull Requests Welcome!

This is an open source project - we'd love to see you  do something with src/firmware/Modes/ - but anything you'd like to fix, is also great!
