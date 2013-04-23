#!/bin/sh
#~ set -x

if [ "$1" = "" ]; then exit 1
fi

MO_NAME="dng4ps-2.mo"

# Update language table from:
# http://trac.wxwidgets.org/browser/wxWidgets/trunk/misc/languages/langtabl.txt

# Reset Languages.cpp, it will be generated here
#~ [ ! -f "Languages.cpp" ] &&
cp "Languages.cpp"{.in,}

find "langs/" -type f -iname '*.po' | sort |
while read L; do
    if [ -f "$L" ]; then
        LANG_CODE="$(basename "$L" '.po')"
        mkdir -p "$1/$LANG_CODE" &&
        msgfmt -D "./langs" -o "$1/$LANG_CODE/$MO_NAME" "${LANG_CODE}.po"
        
        # Generate 
        grep "[a-zA-Z0-9_]\+ \+${LANG_CODE}" langtabl.txt | head -n 1 |
        (
            read WXLANGCODE ISONAME LANG SUBLANG DIR NAME
            #~ echo $WXLANGCODE $ISONAME $LANG $SUBLANG $DIR $NAME
            echo Adding $NAME language
            
            #~ LANGINFO_INIT="{\"$ISONAME\", $NAME, $WXLANGCODE},"
            LANGINFO_INIT="\"${LANG_CODE}\","
            sed -i "s#/\* GENERATE LANGTABL \*/#${LANGINFO_INIT}\n    /* GENERATE LANGTABL */#"  "Languages.cpp"
        )
    fi
done
