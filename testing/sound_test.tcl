##############################################################################
###
### SoundTcl
### Rob Peters
### Jul-1999
### $Id$
###
##############################################################################

if { [catch {package require Sound}] != 0 } { return }

# Check if audio is connected; if not, we skip the sound tests.
#set AUDIO [Sound::haveAudio]
set AUDIO 0   ;# disable sound tests

set TEST_SOUND [Obj::new Sound]
Sound::file $TEST_SOUND $::TEST_DIR/sound1.au

source ${::TEST_DIR}/io_test.tcl

IO::testStringifyCmd SoundTcl IO 1 $::TEST_SOUND
IO::testDestringifyCmd SoundTcl IO 1 $::TEST_SOUND
IO::testWriteCmd SoundTcl IO 1 $::TEST_SOUND
IO::testReadCmd SoundTcl IO 1 $::TEST_SOUND

### Obj::new Sound ###
test "SoundTcl-Obj::new Sound" "normal sound create" {
    set ::TEST_SOUND [Obj::new Sound]
    Sound::file $::TEST_SOUND $::TEST_DIR/sound1.au
    expr {$::TEST_SOUND >= 0}
} {^1$}


### Sound::fileCmd ###
test "SoundTcl-Sound::file" "too few args" {
    Sound::file
} {wrong \# args: should be}
test "SoundTcl-Sound::file" "too many args" {
    Sound::file j u n
} {wrong \# args: should be}
test "SoundTcl-Sound::file" "normal filename set" {
    catch {Sound::file $::TEST_SOUND $::TEST_DIR/sound1.au}
} {^0$} $AUDIO
test "SoundTcl-Sound::file" "normal filename query" {
    Sound::file $::TEST_SOUND $::TEST_DIR/sound1.au
    set f [Sound::file $::TEST_SOUND]
    expr [string compare $f "$::TEST_DIR/sound1.au"] == 0
} {^1$} $AUDIO
test "SoundTcl-Sound::file" "error on bad filename" {
    exec rm -rf $::TEST_DIR/nonexistent_file
    Sound::file $::TEST_SOUND $::TEST_DIR/nonexistent_file
} "^Sound::file:"
test "SoundTcl-Sound::file" "error on junk file" {
    Sound::file $::TEST_SOUND $::TEST_DIR/junk_text_file
} "^Sound::file:" \
  [expr [string equal $env(ARCH) "irix6"] ? $skip_known_bug : $normal_test]

### Sound::playCmd ###
test "SoundTcl-Sound::play" "too few args" {
    Sound::play
} {wrong \# args: should be "Sound::play item_id\(s\)"}
test "SoundTcl-Sound::play" "too many args" {
    Sound::play item junk
} {wrong \# args: should be "Sound::play item_id\(s\)"}
test "SoundTcl-Sound::play" "normal play test sound" {
    Sound::file $::TEST_SOUND $::TEST_DIR/sound1.au
    catch {Sound::play $::TEST_SOUND}
} {^0$} $AUDIO
test "SoundTcl-Sound::play" "normal play ok sound" {
    catch {Sound::play $Sound::ok}
} {^0$} $AUDIO
test "SoundTcl-Sound::play" "normal play err sound" {
    catch {Sound::play $Sound::err}
} {^0$} $AUDIO
test "SoundTcl-Sound::play" "error on bad sound id" {
    Sound::play -1
} {Sound::play: signed/unsigned conversion failed}


### cleanup
unset AUDIO
unset TEST_SOUND

