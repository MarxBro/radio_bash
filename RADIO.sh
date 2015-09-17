#!/bin/bash
#######################################
# RADIO.sh -> Radio in your terminal. #
#######################################

STATION_FILE="STATIONS.txt"

function get_options_from_file {
    LISTA=$( awk -F'@' '!/^#/ { OFS="@"; print $2 }' $STATION_FILE | sed 's/ /_/g')
    echo $LISTA
}

function get_url_of_opt {
    RADIO_URL=$( grep $1 $STATION_FILE | awk -F'@' '{print $1}' )
    echo $RADIO_URL
}

function get_description {
    RADIO_D=$( grep $1 $STATION_FILE | awk -F'@' '{ print $3 }' )
    echo $RADIO_D
}

declare -a OPTIONS=( "$(get_options_from_file $STATION_FILE | sed 's/@/ /g' )" )

#This is the best function I ever programmed. ^_^
function line_draw {
    LINE='----------------------------------'
    echo $LINE
}

function do_menu {
    clear &&
    line_draw &&
    echo 'Quit with CTRL+C.' &&
    echo 'q for menu again.' 
    line_draw &&
    PS3="Your Choice: "

    select opt in ${OPTIONS[@]}
    do
        opt_name="$( echo $opt | sed 's/_/ /g' )"
        URL=$(get_url_of_opt $opt_name)
        DESCRIPTION=$(get_description $opt_name)
        echo $DESCRIPTION && echo $URL && mplayer $URL &> /dev/null
        do_menu
    done 
}

do_menu
exit 0;

#####################################
# Programado por MarxBro.           #
# Compartir bajo la licencia WTFPL. #
#####################################
