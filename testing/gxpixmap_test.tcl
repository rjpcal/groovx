##############################################################################
###
### GxPixmap
### Rob Peters
### $Id$
###
##############################################################################

package require Gxpixmap
package require Tclgl
package require Tlist
package require Toglet

set IMGFILE $TEST_DIR/pbmfile.PPM

set ::PIXMAP [Obj::new GxPixmap]
GxPixmap::loadImage $::PIXMAP $::IMGFILE

source ${::TEST_DIR}/io_test.tcl

IO::testStringifyCmd GxPixmap IO 1 $::PIXMAP
IO::testDestringifyCmd GxPixmap IO 1 $::PIXMAP
IO::testWriteCmd GxPixmap IO 1 $::PIXMAP
IO::testReadCmd GxPixmap IO 1 $::PIXMAP

### Obj::new GxPixmap ###
test "Obj::new GxPixmap" "too many args" {
    Obj::new GxPixmap junk junk
} {^wrong \# args: should be}
test "Obj::new GxPixmap" "normal use" {
    set ::PIXMAP [Obj::new GxPixmap]
} "^${INT}$"

### GxPixmap::loadImage ###
test "GxPixmap::loadImage" "too few args" {
    GxPixmap::loadImage
} {^wrong \# args: should be}
test "GxPixmap::loadImage" "too many args" {
    GxPixmap::loadImage $::PIXMAP $::IMGFILE junk
} {^wrong \# args: should be}
test "GxPixmap::loadImage" "normal use" {
    GxPixmap::loadImage $::PIXMAP $::IMGFILE
    GxPixmap::flipContrast $::PIXMAP
    GxShapeKit::alignmentMode $::PIXMAP $GxShapeKit::CENTER_ON_CENTER
} {^$}
test "GxPixmap::loadImage" "error on non-existent file" {
    exec rm -rf $::TEST_DIR/nonexistent_file.ppm
    GxPixmap::loadImage $::PIXMAP $::TEST_DIR/nonexistent_file.ppm
} "^GxPixmap::loadImage: couldn't open file "
test "GxPixmap::loadImage" "error on junk text file" {
    file copy -force $::TEST_DIR/junk_text_file $::TEST_DIR/junk_text_file.pbm
    catch {GxPixmap::loadImage $::PIXMAP $::TEST_DIR/junk_text_file.pbm} result
    file delete -force $::TEST_DIR/junk_text_file.pbm
    return $result
} "^GxPixmap::loadImage: bad magic number while reading pnm file.*$"
test "GxPixmap::loadImage" "error on junk binary file" {
    file copy -force $::TEST_DIR/junk_bin_file $::TEST_DIR/junk_bin_file.pbm
    catch {GxPixmap::loadImage $::PIXMAP $::TEST_DIR/junk_bin_file.pbm} result
    file delete -force $::TEST_DIR/junk_bin_file.pbm
    return $result
} "^GxPixmap::loadImage: bad magic number while reading pnm file.*$"

### GxPixmap rendering ###
test "rendering" "normal render" {
    clearscreen
    see $::PIXMAP
    set sum [pixelCheckSum]
    return "[expr $sum != 0] $sum"
} {^1}

### GxPixmap::flipContrastCmd ###
test "GxPixmap::flipContrast" "too few args" {
    GxPixmap::flipContrast
} {^wrong \# args: should be "GxPixmap::flipContrast item_id\(s\)"$}
test "GxPixmap::flipContrast" "too many args" {
    GxPixmap::flipContrast $::PIXMAP junk
} {^wrong \# args: should be "GxPixmap::flipContrast item_id\(s\)"$}
test "GxPixmap::flipContrast" "normal use" {
    GxPixmap::flipContrast $::PIXMAP
    clearscreen
    see $::PIXMAP
    set count1 [pixelCheckSum]

    GxPixmap::flipContrast $::PIXMAP
    clearscreen
    see $::PIXMAP
    set count2 [pixelCheckSum]
    
    return "[expr $count1 != $count2] $count1 $count2"
} {^1}

### GxPixmap::flipVerticalCmd ###
test "GxPixmap::flipVertical" "too few args" {
    GxPixmap::flipVertical
} {^wrong \# args: should be "GxPixmap::flipVertical item_id\(s\)"$}
test "GxPixmap::flipVertical" "too many args" {
    GxPixmap::flipVertical $::PIXMAP junk
} {^wrong \# args: should be "GxPixmap::flipVertical item_id\(s\)"$}
test "GxPixmap::flipVertical" "normal use" {

    GxShapeKit::alignmentMode $::PIXMAP $GxShapeKit::CENTER_ON_CENTER

    GxPixmap::flipVertical $::PIXMAP
    clearscreen
    see $::PIXMAP
    set count1 [pixelCheckSum]

    GxPixmap::flipVertical $::PIXMAP
    clearscreen
    see $::PIXMAP
    set count2 [pixelCheckSum]
    
    return "[expr { $count1 == $count2 }] $count1 $count2"
} {^1}

### GxPixmap::zoomCmd ###
test "GxPixmap::zoom" "too few args" {
    GxPixmap::zoom
} {^wrong \# args: should be}
test "GxPixmap::zoom" "too many args" {
    GxPixmap::zoom $::PIXMAP 1.0 junk
} {^wrong \# args: should be}
test "GxPixmap::zoom" "normal use" {
    GxPixmap::asBitmap $::PIXMAP no
    GxPixmap::usingZoom $::PIXMAP yes

    GxPixmap::zoom $::PIXMAP {0.5 0.5}
    clearscreen
    see $::PIXMAP
    set count1 [pixelCheckSum]
    
    GxPixmap::zoom $::PIXMAP {1.0 1.0}
    clearscreen
    see $::PIXMAP
    set count2 [pixelCheckSum]

    return "[expr { $count1 != $count2 }] $count1 $count2"
} {^1}

### GxPixmap::asBitmapCmd ###
test "GxPixmap::asBitmap" "too few args" {
    GxPixmap::asBitmap
} {^wrong \# args: should be}
test "GxPixmap::asBitmap" "too many args" {
    GxPixmap::asBitmap $::PIXMAP yes junk
} {^wrong \# args: should be}
test "GxPixmap::asBitmap" "normal use" {
    GxPixmap::asBitmap $::PIXMAP no
    clearscreen
    set time1 [lindex [time {see $::PIXMAP}] 0]

    GxPixmap::asBitmap $::PIXMAP yes
    clearscreen
    set time2 [lindex [time {see $::PIXMAP}] 0]

    # Rendering should always be faster with glBitmap than with glDrawPixels
    expr { $time1 > $time2 }

} {^1$} $no_test

### GxPixmap::stringifyCmd ###
### GxPixmap::destringifyCmd ###
test "GxPixmap::stringify" "stringify, destringify, compare" {
    set b1 [Obj::new GxPixmap]
    GxPixmap::zoom $b1 {3.0 4.5}

    set str1 [IO::stringify $b1]

    Obj::delete $b1

    set b2 [Obj::new GxPixmap]
    IO::destringify $b2 $str1
    set str2 [IO::stringify $b2]

    return "[string equal $str1 $str2] $str1 $str2"
} {^1 }

### PNG format ###
test "GxPixmap::loadImage" "read PNG format" {
    glClearColor 0.2 0.8 0.2 0.0
    clearscreen
    -> $::PIXMAP loadImage testing/pngfile.png
    see $::PIXMAP

    return "[pixelCheckSum]  [-> $::PIXMAP checkSum]"
} {^30483306  8639262$}

### JPEG format ###
test "GxPixmap::loadImage" "read JPEG format" {
    glClearColor 1.0 1.0 1.0 0.0
    clearscreen
    -> $::PIXMAP loadImage testing/jpegfile.jpg
    see $::PIXMAP

    return "[pixelCheckSum]  [-> $::PIXMAP checkSum]"
} {^37328256  8638152$}

### GIF format ###
test "GxPixmap::loadImage" "read GIF format" {
    glClearColor 0.0 0.0 0.0 0.0
    clearscreen
    -> $::PIXMAP loadImage testing/giffile.gif
    see $::PIXMAP

    return "[pixelCheckSum]  [-> $::PIXMAP checkSum]"
} {^25917786  8639262$}

### cleanup
unset PIXMAP
