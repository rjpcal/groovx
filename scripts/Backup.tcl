# $Id$
set date_ext [clock format [clock seconds] -format %Y_%m_%d]
set filename grsh_backup_${date_ext}.tar.gz
set sources "../grsh/Makefile \
		  ../grsh/RCS \
		  ../grsh/doc \
		  ../grsh/idep \
		  ../grsh/logs \
		  ../grsh/scripts \
		  ../grsh/src \
		  ../grsh/testing/ \
		  "
puts "generating archive \"$filename\" from \"$sources\"..."
eval exec tar cvfz $filename $sources
puts "moving to ftp directory..."
exec mv $filename /cit/rjpeters/ftp/
puts "sending email notification..."
exec echo "made backup $filename" | mail rjpeters
