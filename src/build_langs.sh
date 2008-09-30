#!/bin/sh

if [ "$1" = "" ]; then exit 1
fi

mkdir -p "$1/langs"
mkdir -p "$1/langs/en"
mkdir -p "$1/langs/ru"
mkdir -p "$1/langs/nn"
mkdir -p "$1/langs/de"
mkdir -p "$1/langs/es"
mkdir -p "$1/langs/it"
mkdir -p "$1/langs/zh"
mkdir -p "$1/langs/fi"
mkdir -p "$1/langs/pl"
mkdir -p "$1/langs/fr"

msgfmt -D "./langs" -o "$1/langs/en/dng4ps-2.mo" en.po
msgfmt -D "./langs" -o "$1/langs/ru/dng4ps-2.mo" ru.po
msgfmt -D "./langs" -o "$1/langs/nn/dng4ps-2.mo" nn.po
msgfmt -D "./langs" -o "$1/langs/de/dng4ps-2.mo" de.po
msgfmt -D "./langs" -o "$1/langs/es/dng4ps-2.mo" es.po
msgfmt -D "./langs" -o "$1/langs/it/dng4ps-2.mo" it.po
msgfmt -D "./langs" -o "$1/langs/zh/dng4ps-2.mo" zh.po
msgfmt -D "./langs" -o "$1/langs/fi/dng4ps-2.mo" fi.po
msgfmt -D "./langs" -o "$1/langs/pl/dng4ps-2.mo" pl.po
msgfmt -D "./langs" -o "$1/langs/fr/dng4ps-2.mo" fr.po
