#!/bin/sh

for f in `./devscripts/list-sources.sh`; do
    g=${f#./devscripts/../src/}

    datestring="UTC`date -u +%Y%m%d%H%M%S`"
    incguard=`echo $g | tr [a-z] [A-Z] | tr ./- ___ `
    incguard="GROOVX_${incguard}_${datestring}_DEFINED"

    vcidstring=`echo $g | tr [a-z] [A-Z] | tr ./- ___ `
    vcidstring="vcid_GROOVX_${vcidstring}_${datestring}"
    vcidstring=`echo $vcidstring | tr [A-Z] [a-z]`

    sed -e "s:^#ifndef .*DEFINED:#ifndef $incguard:" $f \
	| sed -e "s:^#define .*DEFINED:#define $incguard:" \
	| sed -e "s:^#endif // .*DEFINED:#endif // \!$incguard:" \
	| sed -e "s:^static const char vcid.*:static const char $vcidstring[] = \"\$Id\$ \$HeadURL\$\";:" \
	> ${f}.new

    diff $f ${f}.new

    mv $f ${f}.bkp
    mv ${f}.new $f
done
