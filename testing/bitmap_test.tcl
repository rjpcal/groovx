##############################################################################
###
### BitmapTcl
### Rob Peters
### Jul-1999
### $Id$
###
##############################################################################

package require Bitmap
package require Objlist
package require Objtogl
package require Tclgl
package require Tlist
package require Pos

if { ![Togl::inited] } { Togl::init "-rgba false"; update }

set PBMFILE $TEST_DIR/pbmfile.PPM

set POS [Pos::Pos]

### Bitmap::BitmapCmd ###
test "BitmapTcl-Bitmap::Bitmap" "too many args" {
	 Bitmap::Bitmap junk
} {^wrong \# args: should be "Bitmap::Bitmap"$}
test "BitmapTcl-Bitmap::Bitmap" "normal use" {
	 set ::BITMAP [Bitmap::Bitmap]
} "^${INT}$"
test "BitmapTcl-Bitmap::Bitmap" "error" {} {^$} $no_test

### Bitmap::loadPbmCmd ###
test "BitmapTcl-Bitmap::loadPbm" "too few args" {
	 Bitmap::loadPbm
} {^wrong \# args: should be "Bitmap::loadPbm bitmap_id filename"$}
test "BitmapTcl-Bitmap::loadPbm" "too many args" {
	 Bitmap::loadPbm $::BITMAP $::PBMFILE junk
} {^wrong \# args: should be "Bitmap::loadPbm bitmap_id filename"$}
test "BitmapTcl-Bitmap::loadPbm" "normal use" {
	 Bitmap::loadPbm $::BITMAP $::PBMFILE
	 Bitmap::flipVertical $::BITMAP
	 Bitmap::flipContrast $::BITMAP
	 Bitmap::center $::BITMAP
} {^$}
test "BitmapTcl-Bitmap::loadPbm" "error on non-existent file" {
	 exec rm -rf $::TEST_DIR/nonexistent_file
	 Bitmap::loadPbm $::BITMAP $::TEST_DIR/nonexistent_file
} "^Bitmap::loadPbm: couldn't open file: $::TEST_DIR/nonexistent_file$"
test "BitmapTcl-Bitmap::loadPbm" "error on junk text file" {
	 Bitmap::loadPbm $::BITMAP $::TEST_DIR/junk_text_file
} "^Bitmap::loadPbm: bad magic number while reading pbm file$"
test "BitmapTcl-Bitmap::loadPbm" "error on junk binary file" {
	 Bitmap::loadPbm $::BITMAP $::TEST_DIR/junk_bin_file
} "^Bitmap::loadPbm: bad magic number while reading pbm file$"

### Bitmap rendering ###
test "BitmapTcl-rendering" "normal render" {
	 Tlist::makeSingles $::POS
	 clearscreen
	 show $::BITMAP
	 expr {[pixelCheckSum] != 0}
} {^1$}

### Bitmap::flipContrastCmd ###
test "BitmapTcl-Bitmap::flipContrast" "too few args" {
	 Bitmap::flipContrast
} {^wrong \# args: should be "Bitmap::flipContrast item_id\(s\)"$}
test "BitmapTcl-Bitmap::flipContrast" "too many args" {
	 Bitmap::flipContrast $::BITMAP junk
} {^wrong \# args: should be "Bitmap::flipContrast item_id\(s\)"$}
test "BitmapTcl-Bitmap::flipContrast" "normal use" {
	 Bitmap::flipContrast $::BITMAP
	 clearscreen
	 show $::BITMAP
	 set count1 [pixelCheckSum]
	 
	 Bitmap::flipContrast $::BITMAP
	 clearscreen
	 show $::BITMAP
	 set count2 [pixelCheckSum]
	 
	 expr { $count1 != $count2 }
} {^1$}
test "BitmapTcl-Bitmap::flipContrast" "error" {} {^$} $no_test

### Bitmap::flipVerticalCmd ###
test "BitmapTcl-Bitmap::flipVertical" "too few args" {
	 Bitmap::flipVertical
} {^wrong \# args: should be "Bitmap::flipVertical item_id\(s\)"$}
test "BitmapTcl-Bitmap::flipVertical" "too many args" {
	 Bitmap::flipVertical $::BITMAP junk
} {^wrong \# args: should be "Bitmap::flipVertical item_id\(s\)"$}
test "BitmapTcl-Bitmap::flipVertical" "normal use" {
	 Bitmap::flipVertical $::BITMAP
	 clearscreen
	 show $::BITMAP
	 set count1 [pixelCheckSum]
	 
	 Bitmap::flipVertical $::BITMAP
	 clearscreen
	 show $::BITMAP
	 set count2 [pixelCheckSum]
	 
	 expr { $count1 == $count2 }
} {^1$}
test "BitmapTcl-Bitmap::flipVertical" "error" {} {^$} $no_test

### Bitmap::centerCmd ###
test "BitmapTcl-Bitmap::center" "too few args" {} {^$} $must_implement
test "BitmapTcl-Bitmap::center" "too many args" {} {^$} $must_implement
test "BitmapTcl-Bitmap::center" "normal use" {} {^$} $must_implement
test "BitmapTcl-Bitmap::center" "error" {} {^$} $no_test

### Bitmap::rasterX/YCmd ###
test "BitmapTcl-Bitmap::rasterX/Y" "too few args" {} {^$} $must_implement
test "BitmapTcl-Bitmap::rasterX/Y" "too many args" {} {^$} $must_implement
test "BitmapTcl-Bitmap::rasterX/Y" "normal use" {} {^$} $must_implement
test "BitmapTcl-Bitmap::rasterX/Y" "error" {} {^$} $no_test

### Bitmap::zoomX/YCmd ###
test "BitmapTcl-Bitmap::zoomX/Y" "too few args" {
	 Bitmap::zoomX
} {^wrong \# args: should be "Bitmap::zoomX item_id\(s\) \?new_value\(s\)\?"$}
test "BitmapTcl-Bitmap::zoomX/Y" "too many args" {
	 Bitmap::zoomY $::BITMAP 1.0 junk
} {^wrong \# args: should be "Bitmap::zoomY item_id\(s\) \?new_value\(s\)\?"$}
test "BitmapTcl-Bitmap::zoomX/Y" "normal use" {
	 Bitmap::usingGlBitmap $::BITMAP no

	 Bitmap::zoomX $::BITMAP 0.5
	 Bitmap::zoomY $::BITMAP 0.5
	 clearscreen
	 show $::BITMAP
	 set count1 [pixelCheckSum]
	 
	 Bitmap::zoomX $::BITMAP 1.0
	 Bitmap::zoomY $::BITMAP 1.0
	 clearscreen
	 show $::BITMAP
	 set count2 [pixelCheckSum]

	 expr { $count1 != $count2 }
} {^1$}
test "BitmapTcl-Bitmap::zoomX/Y" "error" {} {^$} $no_test

### Bitmap::usingGlBitmapCmd ###
test "BitmapTcl-Bitmap::usingGlBitmap" "too few args" {
	 Bitmap::usingGlBitmap
} {^wrong \# args: should be "Bitmap::usingGlBitmap item_id\(s\) \?new_value\(s\)\?"$}
test "BitmapTcl-Bitmap::usingGlBitmap" "too many args" {
	 Bitmap::usingGlBitmap $::BITMAP yes junk
} {^wrong \# args: should be "Bitmap::usingGlBitmap item_id\(s\) \?new_value\(s\)\?"$}
test "BitmapTcl-Bitmap::usingGlBitmap" "normal use" {
	 Bitmap::usingGlBitmap $::BITMAP no
	 clearscreen
	 set time1 [time {show $::BITMAP}]

	 Bitmap::usingGlBitmap $::BITMAP yes
	 clearscreen
	 set time2 [time {show $::BITMAP}]
	 
	 # Rendering should always be faster with glBitmap than with glDrawPixels
	 expr { $time1 > $time2 }
} {^1$}
test "BitmapTcl-Bitmap::usingGlBitmap" "error" {} {^$} $no_test

### Bitmap::stringifyCmd ###
### Bitmap::destringifyCmd ###
test "BitmapTcl-Bitmap::stringify" "stringify, destringify, compare" {
	 set str1 [Bitmap::stringify $::BITMAP]
	 ObjList::reset
	 set ::BITMAP [Bitmap::Bitmap]
	 Bitmap::destringify $::BITMAP $str1
	 set str2 [Bitmap::stringify $::BITMAP]
	 string equal $str1 $str2
} {^1$}

### cleanup
unset BITMAP
unset POS
