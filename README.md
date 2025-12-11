# Testing display modules with ZMK

This is an attempt to test popular SSD1306 0.96" 128x64 OLED and ST7735S 0.96" 160x80 IPS LCD display modules with nRF52840 and RP2040 hardware based boards.
* [SuperMini NRF52840](https://github.com/joric/nrfmicro/wiki/Alternatives#supermini-nrf52840) - well known nice!nano v2 clone
* [YD-RP2040](https://circuitpython.org/board/vcc_gnd_yd_rp2040/) - Raspberry Pi Pico clone

The firmware is based on the [ZMK](https://github.com/zmkfirmware). Only ZMK __v0.3.0__ and Zephyr __v3.5__ are supported.

## Building via GitHub action

Althought the github-action specific configuration files do exest this type of build is not tested or maintained.

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

* Build the firmware for nice!nano_v2 with 128x64 OLED module:

        docker run --interactive --tty --name zmk-3.5 --workdir /zmk \
            --volume "./config:/zmk-config" \
            --volume "./zmk:/zmk" \
            --volume ".:/boards" \
            --user="$(id -u):$(id -g)" \
            zmkfirmware/zmk-dev-arm:3.5 \
            west build /zmk/app --pristine --board "rpi_pico" \
            -- -DSHIELD="rp2040_display ssd1306" -DZMK_CONFIG="/zmk-config" \
            -DZMK_EXTRA_MODULES="/boards" 

* Finnaly - copy the firmware file and remove the container:

        docker cp zmk-3.5:/zmk/build/zephyr/zmk.uf2 ./rp2040_ssd1306.uf2
        docker container rm zmk-3.5

For convenience, `build.sh` script, which contains the above two steps is given. Example usage:

    ./build.sh "rp2040_display" "ssd1306"
    ./build.sh "nrf52_display" "st7735s"
    ./build.sh "rp2040_display"

