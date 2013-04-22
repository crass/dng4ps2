#!/bin/sh
#~ set -x

if [ "$1" = "" ]; then exit 1
fi

MO_NAME="dng4ps-2.mo"

for L in "langs/"*.po; do
    if [ -f "$L" ]; then
        LANG_CODE="$(basename "$L" '.po')"
        mkdir -p "$1/$LANG_CODE" &&
        msgfmt -D "./langs" -o "$1/$LANG_CODE/$MO_NAME" "${LANG_CODE}.po"
    fi
done
