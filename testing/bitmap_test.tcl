##############################################################################
###
### BitmapTcl
### Rob Peters
### Jul-1999
### $Id$
###
##############################################################################

package require Bitmap
package require Glbitmap
package require Objlist
package require Objtogl
package require Tclgl
package require Tlist
package require Pos

set PBMFILE $TEST_DIR/pbmfile.PPM

set ::GLBITMAP [IO::new GLBitmap]
Bitmap::loadPbm $::GLBITMAP $::PBMFILE

source ${::TEST_DIR}/io_test.tcl

IO::testStringifyCmd GLBitmapTcl GLBitmap 1 $::GLBITMAP
IO::testDestringifyCmd GLBitmapTcl GLBitmap 1 $::GLBITMAP
IO::testWriteCmd GLBitmapTcl GLBitmap 1 $::GLBITMAP
IO::testReadCmd GLBitmapTcl GLBitmap 1 $::GLBITMAP

if { ![Togl::inited] } { Togl::init "-rgba false"; update }

set POS [IO::new Position]

### IO::new GLBitmap ###
test "GLBitmapTcl-IO::new GLBitmap" "too many args" {
	 IO::new GLBitmap junk
} {^wrong \# args: should be "IO::new typename"$}
test "GLBitmapTcl-IO::new GLBitmap" "normal use" {
	 set ::BITMAP [IO::new GLBitmap]
} "^${INT}$"

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
} "^Bitmap::loadPbm: bad magic number while reading pbm file.*$"
test "BitmapTcl-Bitmap::loadPbm" "error on junk binary file" {
	 Bitmap::loadPbm $::BITMAP $::TEST_DIR/junk_bin_file
} "^Bitmap::loadPbm: bad magic number while reading pbm file.*$"

### Bitmap rendering ###
test "BitmapTcl-rendering" "normal render" {
	 BlockList::reset
	 Tlist::reset
	 set ::BITMAP_TRIAL [Tlist::dealSingles $::BITMAP $::POS]
	 clearscreen
	 show $::BITMAP_TRIAL
	 set sum [pixelCheckSum]
	 return "[expr $sum != 0] $sum"
} {^1}

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
	 show $::BITMAP_TRIAL
	 set count1 [pixelCheckSum]
	 
	 Bitmap::flipContrast $::BITMAP
	 clearscreen
	 show $::BITMAP_TRIAL
	 set count2 [pixelCheckSum]
	 
	 return "[expr $count1 != $count2] $count1 $count2"
} {^1}
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
	 show $::BITMAP_TRIAL
	 set count1 [pixelCheckSum]
	 
	 Bitmap::flipVertical $::BITMAP
	 clearscreen
	 show $::BITMAP_TRIAL
	 set count2 [pixelCheckSum]
	 
	 expr { $count1 == $count2 }
} {^1$}
test "BitmapTcl-Bitmap::flipVertical" "error" {} {^$} $no_test

### Bitmap::centerCmd ###
test "BitmapTcl-Bitmap::center" "too few args" {
	 Bitmap::center
} {^wrong \# args: should be "Bitmap::center item_id\(s\)"$}
test "BitmapTcl-Bitmap::center" "too many args" {
	 Bitmap::center $::BITMAP junk
} {^wrong \# args: should be "Bitmap::center item_id\(s\)"$}
test "BitmapTcl-Bitmap::center" "normal use" {
	 Bitmap::center $::BITMAP
} {^$}

### Bitmap::rasterX/YCmd ###
test "BitmapTcl-Bitmap::rasterX/Y" "too few args" {
	 Bitmap::rasterX
} {^wrong \# args: should be "Bitmap::rasterX item_id\(s\) \?new_value\(s\)\?"$}
test "BitmapTcl-Bitmap::rasterX/Y" "too many args" {
	 Bitmap::rasterY $::BITMAP 3.5 junk
} {^wrong \# args: should be "Bitmap::rasterY item_id\(s\) \?new_value\(s\)\?"$}
test "BitmapTcl-Bitmap::rasterX/Y" "normal use" {
	 Bitmap::rasterX $::BITMAP 1.2
	 Bitmap::rasterY $::BITMAP 3.67
	 return "[Bitmap::rasterX $::BITMAP] [Bitmap::rasterY $::BITMAP]"
} {^1\.2 3\.67$}

### Bitmap::zoomX/YCmd ###
test "BitmapTcl-Bitmap::zoomX/Y" "too few args" {
	 Bitmap::zoomX
} {^wrong \# args: should be "Bitmap::zoomX item_id\(s\) \?new_value\(s\)\?"$}
test "BitmapTcl-Bitmap::zoomX/Y" "too many args" {
	 Bitmap::zoomY $::BITMAP 1.0 junk
} {^wrong \# args: should be "Bitmap::zoomY item_id\(s\) \?new_value\(s\)\?"$}
test "BitmapTcl-Bitmap::zoomX/Y" "normal use" {
	 GLBitmap::usingGlBitmap $::BITMAP no

	 Bitmap::rasterX $::BITMAP 0.0
	 Bitmap::rasterY $::BITMAP 0.0

	 Bitmap::zoomX $::BITMAP 0.5
	 Bitmap::zoomY $::BITMAP 0.5
	 clearscreen
	 show $::BITMAP_TRIAL
	 set count1 [pixelCheckSum]
	 
	 Bitmap::zoomX $::BITMAP 1.0
	 Bitmap::zoomY $::BITMAP 1.0
	 clearscreen
	 show $::BITMAP_TRIAL
	 set count2 [pixelCheckSum]

	 expr { $count1 != $count2 }
} {^1$}
test "BitmapTcl-Bitmap::zoomX/Y" "error" {} {^$} $no_test

### GLBitmap::usingGlBitmapCmd ###
test "GLBitmapTcl-GLBitmap::usingGlBitmap" "too few args" {
	 GLBitmap::usingGlBitmap
} {^wrong \# args: should be "GLBitmap::usingGlBitmap item_id\(s\) \?new_value\(s\)\?"$}
test "GLBitmapTcl-GLBitmap::usingGlBitmap" "too many args" {
	 GLBitmap::usingGlBitmap $::BITMAP yes junk
} {^wrong \# args: should be "GLBitmap::usingGlBitmap item_id\(s\) \?new_value\(s\)\?"$}
test "GLBitmapTcl-GLBitmap::usingGlBitmap" "normal use" {
	 GLBitmap::usingGlBitmap $::BITMAP no
	 clearscreen
	 set time1 [lindex [time {show $::BITMAP_TRIAL}] 0]

	 GLBitmap::usingGlBitmap $::BITMAP yes
	 clearscreen
	 set time2 [lindex [time {show $::BITMAP_TRIAL}] 0]

	 # Rendering should always be faster with glBitmap than with glDrawPixels
	 expr { $time1 > $time2 }
	 
} {^1$}
test "GLBitmapTcl-GLBitmap::usingGlBitmap" "error" {} {^$} $no_test

### Bitmap::stringifyCmd ###
### Bitmap::destringifyCmd ###
test "BitmapTcl-Bitmap::stringify" "stringify, destringify, compare" {
	 set str1 [Bitmap::stringify $::BITMAP]
	 ObjList::reset
	 set ::BITMAP [IO::new GLBitmap]
	 GLBitmap::destringify $::BITMAP $str1
	 set str2 [Bitmap::stringify $::BITMAP]
	 string equal $str1 $str2
} {^1$}

### cleanup
unset BITMAP
unset POS
