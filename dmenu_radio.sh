#!/bin/bash
######################################################################
# While I figure a proper way to display the million stations available,
# here is how I'm using this file: dmenu!
# This way we can use dmenu for search by keywords like "rock" or 
# something...
######################################################################

dl_streaming(){
    wget http://www.radiosure.com/rsdbms/stations2.zip &&
    unzip *.zip
    rm *.zip
    RADIO_RSD=*.rsd
    #echo $RADIO_RSD
    perl -F"\t" -lae '$t="@". "@F[0]" . "@" . "@F[2]"; (@F[$_ * -1] =~ m/http/ ? print @F[$_ * -1], $t : "") foreach (1 .. 6)' < $RADIO_RSD  > STATIONS_HUGE.txt
}

play_stuff(){
   mplayer $(cat STATIONS_HUGE.txt | dmenu | awk -F@ '{print $1}')
}

if [ -f STATIONS_HUGE.txt ]; then
    play_stuff
else
    dl_streaming
    play_stuff
fi

exit 0

# MarxBro - WTFPL - 2016

