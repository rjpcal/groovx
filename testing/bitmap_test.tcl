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
package require Tclgl
package require Tlist
package require Toglet
package require Pos

set PBMFILE $TEST_DIR/pbmfile.PPM

set ::GLBITMAP [Obj::new GLBitmap]
Bitmap::loadPbm $::GLBITMAP $::PBMFILE

source ${::TEST_DIR}/io_test.tcl

IO::testStringifyCmd GLBitmapTcl IO 1 $::GLBITMAP
IO::testDestringifyCmd GLBitmapTcl IO 1 $::GLBITMAP
IO::testWriteCmd GLBitmapTcl IO 1 $::GLBITMAP
IO::testReadCmd GLBitmapTcl IO 1 $::GLBITMAP

if { ![Togl::inited] } { Togl::init; update }

set POS [Obj::new Position]

### Obj::new GLBitmap ###
test "GLBitmapTcl-Obj::new GLBitmap" "too many args" {
	 Obj::new GLBitmap junk junk
} {^wrong \# args: should be}
test "GLBitmapTcl-Obj::new GLBitmap" "normal use" {
	 set ::BITMAP [Obj::new GLBitmap]
} "^${INT}$"

### Bitmap::loadPbmCmd ###
test "BitmapTcl-Bitmap::loadPbm" "too few args" {
	 Bitmap::loadPbm
} {^wrong \# args: should be}
test "BitmapTcl-Bitmap::loadPbm" "too many args" {
	 Bitmap::loadPbm $::BITMAP $::PBMFILE junk
} {^wrong \# args: should be}
test "BitmapTcl-Bitmap::loadPbm" "normal use" {
	 Bitmap::loadPbm $::BITMAP $::PBMFILE
	 Bitmap::flipVertical $::BITMAP
	 Bitmap::flipContrast $::BITMAP
	 GrObj::alignmentMode $::BITMAP $GrObj::CENTER_ON_CENTER
} {^$}
test "BitmapTcl-Bitmap::loadPbm" "error on non-existent file" {
	 exec rm -rf $::TEST_DIR/nonexistent_file
	 Bitmap::loadPbm $::BITMAP $::TEST_DIR/nonexistent_file
} "^Bitmap::loadPbm: couldn't open file "
test "BitmapTcl-Bitmap::loadPbm" "error on junk text file" {
	 Bitmap::loadPbm $::BITMAP $::TEST_DIR/junk_text_file
} "^Bitmap::loadPbm: bad magic number while reading pbm file.*$"
test "BitmapTcl-Bitmap::loadPbm" "error on junk binary file" {
	 Bitmap::loadPbm $::BITMAP $::TEST_DIR/junk_bin_file
} "^Bitmap::loadPbm: bad magic number while reading pbm file.*$"

### Bitmap rendering ###
test "BitmapTcl-rendering" "normal render" {
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
	 
	 return "[expr { $count1 == $count2 }] $count1 $count2"
} {^1}

### Bitmap::rasterPos ###
test "BitmapTcl-Bitmap::rasterPos" "too few args" {
	 Bitmap::rasterPos
} {^wrong \# args: should be}
test "BitmapTcl-Bitmap::rasterPos" "too many args" {
	 Bitmap::rasterPos $::BITMAP 3.5 junk
} {^wrong \# args: should be}
test "BitmapTcl-Bitmap::rasterPos" "normal use" {
	 Bitmap::rasterPos $::BITMAP {1.2 3.67}
	 return "[Bitmap::rasterPos $::BITMAP]"
} {^1\.2 3\.67$}

### Bitmap::zoomCmd ###
test "BitmapTcl-Bitmap::zoom" "too few args" {
	 Bitmap::zoom
} {^wrong \# args: should be}
test "BitmapTcl-Bitmap::zoom" "too many args" {
	 Bitmap::zoom $::BITMAP 1.0 junk
} {^wrong \# args: should be}
test "BitmapTcl-Bitmap::zoom" "normal use" {
	 GLBitmap::usingGlBitmap $::BITMAP no

	 Bitmap::rasterPos $::BITMAP {0.0 0.0}

	 Bitmap::zoom $::BITMAP {0.5 0.5}
	 clearscreen
	 show $::BITMAP_TRIAL
	 set count1 [pixelCheckSum]
	 
	 Bitmap::zoom $::BITMAP {1.0 1.0}
	 clearscreen
	 show $::BITMAP_TRIAL
	 set count2 [pixelCheckSum]

	 return "[expr { $count1 != $count2 }] $count1 $count2"
} {^1}

### GLBitmap::usingGlBitmapCmd ###
test "GLBitmapTcl-GLBitmap::usingGlBitmap" "too few args" {
	 GLBitmap::usingGlBitmap
} {^wrong \# args: should be}
test "GLBitmapTcl-GLBitmap::usingGlBitmap" "too many args" {
	 GLBitmap::usingGlBitmap $::BITMAP yes junk
} {^wrong \# args: should be}
test "GLBitmapTcl-GLBitmap::usingGlBitmap" "normal use" {
	 GLBitmap::usingGlBitmap $::BITMAP no
	 clearscreen
	 set time1 [lindex [time {show $::BITMAP_TRIAL}] 0]

	 GLBitmap::usingGlBitmap $::BITMAP yes
	 clearscreen
	 set time2 [lindex [time {show $::BITMAP_TRIAL}] 0]

	 # Rendering should always be faster with glBitmap than with glDrawPixels
	 expr { $time1 > $time2 }
	 
} {^1$} $no_test

### Bitmap::stringifyCmd ###
### Bitmap::destringifyCmd ###
test "BitmapTcl-Bitmap::stringify" "stringify, destringify, compare" {
	 set str1 [IO::stringify $::BITMAP]
	 set ::BITMAP [Obj::new GLBitmap]
	 IO::destringify $::BITMAP $str1
	 set str2 [IO::stringify $::BITMAP]
	 string equal $str1 $str2
} {^1$}

### cleanup
unset BITMAP
unset POS
