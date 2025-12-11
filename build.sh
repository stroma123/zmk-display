#!/bin/bash

SHIELD1="${1:-rp2040_display}"
shift
SHIELD2="$1"

if [ "${SHIELD1:0:6}" = "rp2040" ]; then
    BOARD=rpi_pico
elif [ "${SHIELD1:0:5}" = "nrf52" ]; then
    BOARD=nice_nano_v2
    EXTRA_MACRO="-DUSE_SOC_NRF52840=y"
fi

if [ -z "$BOARD" ]; then
        echo "build.sh [base shield] [display shield]" >&2
        echo "example : build.sh rp2040_display ssd1306" >&2
        echo "  valid base shields:"
        echo "    rp2040_display, uses 'rpi_pico' board"
        echo "    nrf52_display, uses 'nice_nano_v2' board"
        echo "  valid display shields:"
        echo "    ssd1306"
        echo "    st7735s"
    exit 0
fi

FILENAME=$SHIELD1
if [ -n "$SHIELD2" ]; then
    FILENAME="${SHIELD1%_*}_${SHIELD2}"
fi

SHIELDS="${SHIELD1} ${SHIELD2}"

echo "BUILD command: west build /zmk/app -p -b $BOARD -- -DSHIELD=\"$SHIELDS\" $EXTRA_MACRO -DZMK_CONFIG=\"/zmk-config\" -DZMK_EXTRA_MODULES=\"/boards\""

docker run --interactive --tty --name zmk-3.5 --workdir /zmk \
--volume "./config:/zmk-config" \
--volume "./zmk:/zmk" \
--volume ".:/boards" \
--user="$(id -u):$(id -g)" \
zmkfirmware/zmk-dev-arm:3.5 \
west build /zmk/app -p -b $BOARD -- -DSHIELD="$SHIELDS" $EXTRA_MACRO -DZMK_CONFIG="/zmk-config" -DZMK_EXTRA_MODULES="/boards"

#west build -t menuconfig

docker cp zmk-3.5:/zmk/build/zephyr/zmk.uf2 "./${FILENAME}.uf2"

docker container rm zmk-3.5
