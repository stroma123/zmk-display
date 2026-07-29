# Testing display modules with ZMK

Popular SSD1306 0.96" 128x64 OLED and ST7735S 0.96" 160x80 IPS LCD display modules
driven by ZMK firmware on nRF52840 and RP2040 hardware based boards:

* [SuperMini NRF52840](https://github.com/joric/nrfmicro/wiki/Alternatives#supermini-nrf52840) - well known nice!nano v2 clone
* [YD-RP2040](https://circuitpython.org/board/vcc_gnd_yd_rp2040/) - Raspberry Pi Pico clone

The firmware is based on [ZMK](https://github.com/zmkfirmware). Only ZMK __v0.3.0__ and Zephyr __v3.5__ are supported.

## Status

All four board/display combinations build from this tree and are hardware validated
(July 2026) with a custom LVGL status screen (R/G/B color test bars, canvas drawing
primitives, image widget, live layer label):

|                              | ssd1306 | st7735s |
|------------------------------|---------|---------|
| **rpi_pico** (YD-RP2040)     | OK      | OK (landscape + portrait) |
| **nice_nano_v2** (SuperMini) | OK      | OK (portrait) |

## Shields

The module defines four shields in `boards/shields/display/`:

* `rp2040_display`, `nrf52_display` - base shields: 3x4 key matrix, USB HID,
  USB CDC console + ZMK Studio RPC UART.
* `ssd1306`, `st7735s` - display add-on shields, combined with a base shield
  at build time: `-DSHIELD="<base> <display>"`.

Wiring diagrams for every module/board combination are in the comment blocks of
`boards/shields/display/ssd1306.overlay` and `st7735s.overlay`.

The ST7735S panel orientation defaults to portrait 80x160; for landscape 160x80
build with `-DDTS_EXTRA_CPPFLAGS=-DST7735S_LANDSCAPE` (combine with the nRF52840
macro as a semicolon-separated list: `"-DUSE_SOC_NRF52840;-DST7735S_LANDSCAPE"`)
or pass `landscape` as the third argument of `build.sh`. The status screen layout
adapts at runtime.

nRF52840 builds require `-DDTS_EXTRA_CPPFLAGS=-DUSE_SOC_NRF52840` so the shared
display overlays take their nRF52840 pin branches (a missing flag fails the build
with an explicit `#error`). `build.sh` adds the flag automatically and `build.yaml`
carries it for GitHub Actions.

## Building via GitHub action

The GitHub Actions configuration (`build.yaml`, `.github/workflows/build.yml`) is
kept in sync with the local build flags, but these builds are not regularly
exercised.

## Building locally using Docker image

This option describes Docker container method to build the firmware, only native Docker CLI used.
It assumes you already have Docker installed and your user has permission to run `docker`. If you need to run `docker` as the **root** user, please remember to prepend `sudo` to the docker commands below.

This is **not** Docker Desktop/VS Code approach described in ZMK [Container](https://zmk.dev/docs/development/local-toolchain/setup/container) setup.

* Clone the [ZMK](https://github.com/zmkfirmware) project into this repo.

        git clone --branch v0.3.0 --depth 1 https://github.com/zmkfirmware/zmk

* Now initialize and update the ZMK. Note this step is needed only once and may take some time until it is completed.

        docker run --rm --interactive --tty --name zmk-3.5 --workdir /zmk \
            --volume "./config:/zmk-config" \
            --volume "./zmk:/zmk" \
            --volume ".:/boards" \
            --user="$(id -u):$(id -g)" \
            zmkfirmware/zmk-dev-arm:3.5 \
            sh -c 'west init -l /zmk/app/; west update'

* Build the firmware, e.g. for the rpi_pico based board with the 128x64 OLED module:

        docker run --interactive --tty --name zmk-3.5 --workdir /zmk \
            --volume "./config:/zmk-config" \
            --volume "./zmk:/zmk" \
            --volume ".:/boards" \
            --user="$(id -u):$(id -g)" \
            zmkfirmware/zmk-dev-arm:3.5 \
            west build /zmk/app --pristine --board "rpi_pico" \
            -- -DSHIELD="rp2040_display ssd1306" -DZMK_CONFIG="/zmk-config" \
            -DZMK_EXTRA_MODULES="/boards"

    For `nice_nano_v2` builds add `-DDTS_EXTRA_CPPFLAGS=-DUSE_SOC_NRF52840` to the
    arguments after `--`.

* Finally - copy the firmware file and remove the container:

        docker cp zmk-3.5:/zmk/build/zephyr/zmk.uf2 ./rp2040_ssd1306.uf2
        docker container rm zmk-3.5

For convenience, `build.sh` script, which contains the above two steps (and adds the
nRF52840 flag automatically) is given. Example usage:

    ./build.sh "rp2040_display" "ssd1306"
    ./build.sh "rp2040_display" "st7735s"
    ./build.sh "nrf52_display" "ssd1306"
    ./build.sh "nrf52_display" "st7735s"
    ./build.sh "rp2040_display"
