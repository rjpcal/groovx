##############################################################################
###
### SoundTcl
### Rob Peters
### Jul-1999
### $Id$
###
##############################################################################

# Check if audio is connected; if not, we skip the sound tests.
#set AUDIO [Sound::haveAudio]
set AUDIO 0							  ;# disable sound tests

### Sound::setCmd ###
test "SoundTcl-Sound::set" "too few args" {
	 Sound::set
} {wrong \# args: should be "Sound::set sound_name \?new_filename\?"} $AUDIO
test "SoundTcl-Sound::set" "too many args" {
	 Sound::set j u n
} {wrong \# args: should be "Sound::set sound_name \?new_filename\?"} $AUDIO
test "SoundTcl-Sound::set" "normal filename set" {
	 catch {Sound::set test $::TEST_DIR/sound1.au}
} {^0$} $AUDIO
test "SoundTcl-Sound::set" "normal filename query" {
	 Sound::set test $::TEST_DIR/sound1.au
	 set f [Sound::set test]
	 expr [string compare $f "$::TEST_DIR/sound1.au"] == 0
} {^1$} $AUDIO
test "SoundTcl-Sound::set" "error on bad filename" {
	 exec rm -rf $::TEST_DIR/nonexistent_file
	 Sound::set test $::TEST_DIR/nonexistent_file
} "Sound::set: SoundError: bad or nonexistent file\
		  '$::TEST_DIR/nonexistent_file'" $AUDIO

### Sound::playCmd ###
test "SoundTcl-Sound::play" "too few args" {
	 Sound::play
} {wrong \# args: should be "Sound::play sound_name"} $AUDIO
test "SoundTcl-Sound::play" "too many args" {
	 Sound::play j u
} {wrong \# args: should be "Sound::play sound_name"} $AUDIO
test "SoundTcl-Sound::play" "normal play test sound" {
	 Sound::set test $::TEST_DIR/sound1.au
	 catch {Sound::play test}
} {^0$} $AUDIO
test "SoundTcl-Sound::play" "normal play ok sound" {
	 Sound::set ok $::TEST_DIR/sound1.au
	 catch {Sound::play ok}
} {^0$} $AUDIO
test "SoundTcl-Sound::play" "normal play err sound" {
	 Sound::set err $::TEST_DIR/sound2.au
	 catch {Sound::play err}
} {^0$} $AUDIO
test "SoundTcl-Sound::play" "error on bad sound name" {
	 Sound::play junk
} {Sound::play: sound does not exist} $AUDIO

unset AUDIO


