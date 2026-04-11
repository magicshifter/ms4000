# MS4000 Firmware

Embedded C/C++ firmware for the MS4000 gesture-controlled LED synthesizer running on ESP8266.

## Quick Start

### Prerequisites

- **PlatformIO Core** or IDE
- **Python 3.7+** with pip
- **ESP8266 board** (esp12e module)
- **USB-to-Serial adapter** (CP210x or similar)

### Building

```bash
cd firmware
make build
```

This will:
1. Generate protocol buffer definitions from `MS4.proto`
2. Copy nanopb libraries
3. Compile firmware using PlatformIO

### Flashing

```bash
make flash    # Using esptool-ck (auto-detects CP210x)
# or
make burn     # Using PlatformIO uploader
```

### Monitoring Serial Output

```bash
make monitor  # Opens serial console at 921600 baud
```

### Development Workflow

```bash
make clean    # Clean build artifacts
make rebuild  # Clean + build
make proto    # Regenerate protocol buffers only
```

## Hardware Configuration

Hardware configuration is centralized in `src/Config/BoardConfig.h`.

### Supported Variants

The firmware supports multiple hardware variants:

- **MS3000_FXOS8700CQ** (default): MS3000 with magnetometer+accelerometer, 16 LEDs
- **MS3000_MMA8452Q**: MS3000 with accelerometer only (early prototypes), 16 LEDs
- **RING**: Ring variant with 160 LEDs
- **SABRE**: Sabre variant with 48 LEDs

### Customizing for Your Hardware

Edit `src/Config/BoardConfig.h` and change the constants:

```cpp
namespace MS4000::BoardConfig {
    // Change LED count
    namespace LEDs {
        constexpr uint16_t COUNT = 32;  // Your custom LED count
    }
    
    // Change pin assignments
    namespace Pins {
        constexpr uint8_t BUTTON_A = 14;  // Your custom pin
    }
}
```

Then rebuild:

```bash
make clean && make build
```

## Architecture

See [ARCHITECTURE.md](ARCHITECTURE.md) for detailed design documentation.

### Key Directories

- `src/` - Source code
  - `HAL/` - Hardware Abstraction Layer (I2C, SPI, GPIO)
  - `Config/` - Centralized board configuration
  - `Hardware/` - Hardware drivers (sensors, LEDs, buttons)
  - `Modes/` - Application modes (POV, light synthesis, MIDI, etc.)
  - `WebServer/` - HTTP server and REST API
- `lib/` - Custom libraries
- `platformio.ini` - PlatformIO configuration

### Build System

The firmware uses a Makefile wrapper around PlatformIO:

- **Protocol Buffers**: Defined in `MS4.proto`, compiled with nanopb
- **Dependencies**: Managed by PlatformIO (see `platformio.ini`)
- **Code Formatting**: clang-format with Allman braces, 100-char limit

## Coding Conventions

- **C99** for HAL/driver code
- **C++17** for application layer
- **No dynamic allocation** in interrupt context (no malloc/new)
- **clang-format enforced** (Allman braces, 100-character limit)
- **Doxygen comments** for all public APIs

## Configuration Options

Edit `src/msConfig.h` for build-time features:

```cpp
#define CONFIG_ENABLE_ACCEL       // Enable accelerometer support
#define CONFIG_ENABLE_MIDI        // Enable MIDI support
#define CONFIG_ENABLE_MIDI_RTP    // Enable RTP-MIDI over WiFi
```

## Troubleshooting

### Build Fails

```bash
# Ensure Python dependencies are installed
pip install -r requirements.txt

# Check PlatformIO is up to date
pio upgrade
```

### Flash Fails

```bash
# Check device is detected
ls /dev/ttyUSB*

# Try slower baud rate
pio run --target upload --upload-port /dev/ttyUSB0 --upload-speed 115200
```

### Serial Monitor Shows Garbage

Ensure baud rate is correct (921600):

```bash
pio device monitor --baud 921600
```

## API Documentation

Generate Doxygen documentation:

```bash
doxygen Doxyfile
open docs/html/index.html
```

## Testing

### On Hardware

1. Flash firmware: `make flash`
2. Monitor serial output: `make monitor`
3. Test boot sequence (LED swipe animation)
4. Test button presses (A, B, Power)
5. Test sensor readings (accelerometer, magnetometer)
6. Test mode switching
7. Test web interface (connect to MS4000 AP, browse to 192.168.4.1)

### Factory Mode

Hold Button A during boot to enter factory mode for hardware testing.

## License

See main repository LICENSE file.

## Support

For issues or questions, see: https://github.com/magicshifter/MS4000
