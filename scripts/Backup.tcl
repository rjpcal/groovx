# $Id$

set date_ext [clock format [clock seconds] -format %Y_%m_%d]

set archive_name grsh_backup_${date_ext}.tar.gz

set sources "../grsh/Makefile \
		  ../grsh/RCS \
		  ../grsh/dep \
		  ../grsh/doc \
		  ../grsh/idep \
		  ../grsh/logs \
		  ../grsh/scripts \
		  ../grsh/src \
		  ../grsh/testing/ \
		  "
puts "generating archive \"$archive_name\" from \"$sources\"..."
eval exec tar cvfz $archive_name $sources
exec chmod -w $archive_name

puts "moving to ftp directory..."
exec mv $archive_name /cit/rjpeters/ftp/

puts "sending email notification..."
exec echo "made backup $archive_name" | mail rjpeters
