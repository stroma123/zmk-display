#!/bin/bash

SHIELD1="${1:-rp2040_display}"
SHIELD2="$2"
ORIENTATION="$3"

DTS_MACROS=""
if [ "${SHIELD1:0:6}" = "rp2040" ]; then
    BOARD=rpi_pico
elif [ "${SHIELD1:0:5}" = "nrf52" ]; then
    BOARD=nice_nano_v2
    # devicetree preprocessor macro: shared display overlays take their
    # nRF52840 branches
    DTS_MACROS="-DUSE_SOC_NRF52840"
fi

if [ -z "$BOARD" ]; then
        echo "build.sh [base shield] [display shield] [orientation]" >&2
        echo "example : build.sh rp2040_display ssd1306" >&2
        echo "          build.sh nrf52_display st7735s landscape" >&2
        echo "  valid base shields:"
        echo "    rp2040_display, uses 'rpi_pico' board"
        echo "    nrf52_display, uses 'nice_nano_v2' board"
        echo "  valid display shields:"
        echo "    ssd1306"
        echo "    st7735s"
        echo "  orientation (st7735s only):"
        echo "    portrait (default)"
        echo "    landscape"
    exit 0
fi

if [ "$ORIENTATION" = "landscape" ]; then
    DTS_MACROS="${DTS_MACROS:+${DTS_MACROS};}-DST7735S_LANDSCAPE"
fi

EXTRA_MACRO=""
if [ -n "$DTS_MACROS" ]; then
    EXTRA_MACRO="-DDTS_EXTRA_CPPFLAGS=${DTS_MACROS}"
fi

FILENAME=$SHIELD1
if [ -n "$SHIELD2" ]; then
    FILENAME="${SHIELD1%_*}_${SHIELD2}"
fi
if [ "$ORIENTATION" = "landscape" ]; then
    FILENAME="${FILENAME}_landscape"
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
