#!/usr/bin/env groovx

# Loads a chromatic scale of sound files and assign them to suitably named
# variables.


set notes {
    329.6 E
    349.2 F
    370.0 Fsh
    392.0 G
    415.3 Gsh
    440.0 A
    466.2 Ash
    493.9 B
    523.3 C
    554.4 Csh
    587.3 D
    622.3 Dsh
}

foreach {pitch name} $notes {
    set s [new Sound]
    -> $s file $::env(GRSH_LIB_DIR)/audio/saw25_${pitch}Hz_300ms.au
    -> $s play
    set $name $s
}
