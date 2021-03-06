##############################################################################
###
### SoundTcl
### Rob Peters
### Jul-1999
###
##############################################################################

package require Sound

# Check if audio is connected; if not, we skip the sound tests.
#set AUDIO [Sound::haveAudio]
set AUDIO 0   ;# disable sound tests

set TEST_SOUND [Obj::new Sound]
-> $TEST_SOUND file $::TEST_DIR/sound1.au

source ${::TEST_DIR}/io_test.tcl

::testReadWrite SoundTcl $::TEST_SOUND

### Obj::new Sound ###
test "SoundTcl-Obj::new Sound" "normal sound create" {
    set s [new Sound]
    delete $s
} {^$}


### Sound::file ###
test "Sound::file" "too few args" {
    Sound::file
} {wrong \# args: should be}

test "Sound::file" "too many args" {
    Sound::file j u n
} {wrong \# args: should be}

test "Sound::file" "normal use" {
    set s [new Sound]
    -> $s file $::TEST_DIR/sound1.au
    set f [-> $s file]
    delete $s
    expr [string compare $f "$::TEST_DIR/sound1.au"]
} {^0$} $AUDIO

### Sound::forceLoad ###

test "Sound::forceLoad" "error on bad filename" {
    set s [new Sound]
    file delete $::TEST_DIR/nonexistent_file
    -> $s file $::TEST_DIR/nonexistent_file
    catch {-> $s forceLoad} result
    delete $s
    return $result
} {error while evaluating Sound::forceLoad }

test "Sound::forceLoad" "error on junk text file" {
    set s [new Sound]
    -> $s file $::TEST_DIR/junk_text_file
    catch {-> $s forceLoad} result
    delete $s
    return $result
} {error while evaluating Sound::forceLoad }

test "Sound::forceLoad" "error on junk binary file" {
    set s [new Sound]
    -> $s file $::TEST_DIR/junk_bin_file
    catch {-> $s forceLoad} result
    delete $s
    return $result
} {error while evaluating Sound::forceLoad }

### Sound::playCmd ###

test "Sound::play" "too few args" {
    Sound::play
} {wrong \# args: should be}

test "Sound::play" "too many args" {
    Sound::play item junk
} {wrong \# args: should be}

test "Sound::play" "normal play test sound" {
    set s [new Sound]
    -> $s file $::TEST_DIR/sound1.au
    -> $s play
} {^$} $AUDIO

test "Sound::play" "normal play ok sound" {
    -> $Sound::ok play
} {^$} $AUDIO

test "Sound::play" "normal play err sound" {
    -> $Sound::err play
} {^$} $AUDIO

test "Sound::play" "error on bad sound id" {
    Sound::play 0
} {attempted to access invalid object}


### cleanup
unset AUDIO
unset TEST_SOUND
