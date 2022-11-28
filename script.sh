#!/bin/bash

function menu(){
    echo "1. Mostrar el codigo del programa"
    echo "2. Compilar el archivo .c del programa"
    echo "3. Ejecutar el programa"
    echo "4. Salir del menu"

    echo "Elige una de las opciones anteriormente mostradas por pantalla"
    read option
}

while true
do 
    menu
  
    case $option in 
        1) echo "Mostrar código del programa"
            if test -f practicaIntermedia.c
            then
                cat practicaIntermedia.c
            else
                echo "El archivo no existe"
            fi;;
        2) echo "Compilar el archivo .c del programa"
            gcc practicaIntermedia.c -o practicaIntermedia
            sleep 1
            echo "Completado";;
        3) echo "Ejecutar el programa"
            if test -f practicaIntermedia -a -x practicaIntermedia
            then
                echo "Dime el numero de asistentes"
                read numAsistentes
                if test $numAsistentes -ge 1
                then
                    echo "Ejecutando..."                
                    sleep 1 
                    ./practicaIntermedia $numAsistentes
                    exit 0
                else
                    echo "El numero de asistente debe ser mayor o igual a 1"
                fi
            fi;;
        4) echo "Saliendo del programa"
            exit 0;;
        *) echo "Error: opcion no valida"
    esac 
done
