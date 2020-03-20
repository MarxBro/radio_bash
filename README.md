# RADIO.sh


Simple script en bash para reproducir streaming-radio usando mplayer.

Necesita el archivo STATIONS.

Los valores en este último están separados por **@** y delimitan los siguientes campos:

* URL
* Nombre
* Descripcion

Algunas radios pueden no funcionar y el archivo STATIONS necesita una actualizacion.

Son muchas radios y es hora de hacer algo de mayor complejidad...

## Nueva interface ncurses

Vamos carajo!

Todavía es experimental (aka chiotta).

# TO-DO

* Limpir las radios que no funkan
* Describir las que si lo hacen con un poco mas de amor...


# NOTA

You dont need any of this tho, use:

    grep 'Folk' STATIONS.txt | awk -F@ '{print $1}' 
    grep 'Folk' STATIONS.txt | awk -F@ '{print $1}' |  xargs -I{} mplayer {} # If you're super sure
