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

set IMGFILE $TEST_DIR/pbmfile.PPM

set ::GLBITMAP [Obj::new GLBitmap]
Bitmap::loadImage $::GLBITMAP $::IMGFILE

source ${::TEST_DIR}/io_test.tcl

IO::testStringifyCmd GLBitmapTcl IO 1 $::GLBITMAP
IO::testDestringifyCmd GLBitmapTcl IO 1 $::GLBITMAP
IO::testWriteCmd GLBitmapTcl IO 1 $::GLBITMAP
IO::testReadCmd GLBitmapTcl IO 1 $::GLBITMAP

if { ![Togl::inited] } { Togl::init; update }

### Obj::new GLBitmap ###
test "GLBitmapTcl-Obj::new GLBitmap" "too many args" {
    Obj::new GLBitmap junk junk
} {^wrong \# args: should be}
test "GLBitmapTcl-Obj::new GLBitmap" "normal use" {
    set ::BITMAP [Obj::new GLBitmap]
} "^${INT}$"

### Bitmap::loadImage ###
test "BitmapTcl-Bitmap::loadImage" "too few args" {
    Bitmap::loadImage
} {^wrong \# args: should be}
test "BitmapTcl-Bitmap::loadImage" "too many args" {
    Bitmap::loadImage $::BITMAP $::IMGFILE junk
} {^wrong \# args: should be}
test "BitmapTcl-Bitmap::loadImage" "normal use" {
    Bitmap::loadImage $::BITMAP $::IMGFILE
    Bitmap::flipContrast $::BITMAP
    GxShapeKit::alignmentMode $::BITMAP $GxShapeKit::CENTER_ON_CENTER
} {^$}
test "BitmapTcl-Bitmap::loadImage" "error on non-existent file" {
    exec rm -rf $::TEST_DIR/nonexistent_file.ppm
    Bitmap::loadImage $::BITMAP $::TEST_DIR/nonexistent_file.ppm
} "^Bitmap::loadImage: couldn't open file "
test "BitmapTcl-Bitmap::loadImage" "error on junk text file" {
    file copy -force $::TEST_DIR/junk_text_file $::TEST_DIR/junk_text_file.pbm
    catch {Bitmap::loadImage $::BITMAP $::TEST_DIR/junk_text_file.pbm} result
    file delete -force $::TEST_DIR/junk_text_file.pbm
    return $result
} "^Bitmap::loadImage: bad magic number while reading pbm file.*$"
test "BitmapTcl-Bitmap::loadImage" "error on junk binary file" {
    file copy -force $::TEST_DIR/junk_bin_file $::TEST_DIR/junk_bin_file.pbm
    catch {Bitmap::loadImage $::BITMAP $::TEST_DIR/junk_bin_file.pbm} result
    file delete -force $::TEST_DIR/junk_bin_file.pbm
    return $result
} "^Bitmap::loadImage: bad magic number while reading pbm file.*$"
test "BitmapTcl-Bitmap::loadImage" "unknown format" {
    Bitmap::loadImage $::BITMAP $::TEST_DIR/junk_text_file
} "^Bitmap::loadImage: unknown file format.*$"

### Bitmap rendering ###
test "BitmapTcl-rendering" "normal render" {
    clearscreen
    see $::BITMAP
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
    see $::BITMAP
    set count1 [pixelCheckSum]

    Bitmap::flipContrast $::BITMAP
    clearscreen
    see $::BITMAP
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

    GxShapeKit::alignmentMode $::BITMAP $GxShapeKit::CENTER_ON_CENTER

    Bitmap::flipVertical $::BITMAP
    clearscreen
    see $::BITMAP
    set count1 [pixelCheckSum]

    Bitmap::flipVertical $::BITMAP
    clearscreen
    see $::BITMAP
    set count2 [pixelCheckSum]
    
    return "[expr { $count1 == $count2 }] $count1 $count2"
} {^1}

### Bitmap::zoomCmd ###
test "BitmapTcl-Bitmap::zoom" "too few args" {
    Bitmap::zoom
} {^wrong \# args: should be}
test "BitmapTcl-Bitmap::zoom" "too many args" {
    Bitmap::zoom $::BITMAP 1.0 junk
} {^wrong \# args: should be}
test "BitmapTcl-Bitmap::zoom" "normal use" {
    GLBitmap::usingGlBitmap $::BITMAP no

    Bitmap::zoom $::BITMAP {0.5 0.5}
    clearscreen
    see $::BITMAP
    set count1 [pixelCheckSum]
    
    Bitmap::zoom $::BITMAP {1.0 1.0}
    clearscreen
    see $::BITMAP
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
    set time1 [lindex [time {see $::BITMAP}] 0]

    GLBitmap::usingGlBitmap $::BITMAP yes
    clearscreen
    set time2 [lindex [time {see $::BITMAP}] 0]

    # Rendering should always be faster with glBitmap than with glDrawPixels
    expr { $time1 > $time2 }
    
} {^1$} $no_test

### Bitmap::stringifyCmd ###
### Bitmap::destringifyCmd ###
test "BitmapTcl-Bitmap::stringify" "stringify, destringify, compare" {
    set b1 [Obj::new GLBitmap]
    Bitmap::zoom $b1 {3.0 4.5}

    set str1 [IO::stringify $b1]

    Obj::delete $b1

    set b2 [Obj::new GLBitmap]
    IO::destringify $b2 $str1
    set str2 [IO::stringify $b2]

    return "[string equal $str1 $str2] $str1 $str2"
} {^1 }

### cleanup
unset BITMAP
