#!/bin/sh

for f in `./devscripts/list-sources.sh`; do

    sed -e "s:DO_DBG_REGISTER:GVX_DO_DEBUG_REGISTER:g" $f \
	| sed -e "s:DBG_REGISTER:GVX_DBG_REGISTER:g" \
	| sed -e "s:GVX_DO_DEBUG_REGISTER:GVX_DO_DBG_REGISTER:g" \
	| sed -e "s:GET_DBG_LEVEL:GVX_DBG_LEVEL:g" \
	| sed -e "s:DEBUG_H_\([1-8]\):GVX_DEBUG_H_\1:" \
	| sed -e "s:PANIC:GVX_PANIC:g" \
	| sed -e "s:ABORT_IF:GVX_ABORT_IF:g" \
	| sed -e "s:CONCAT:GVX_CONCAT:g" \
	| sed -e "s:ASSERT:GVX_ASSERT:g" \
	| sed -e "s:INVARIANT:GVX_INVARIANT:g" \
	| sed -e "s:PRECONDITION:GVX_PRECONDITION:g" \
	| sed -e "s:POSTCONDITION:GVX_POSTCONDITION:g" \
	| sed -e "s:DOTRACE:GVX_TRACE:g" \
	> ${f}.new

    diff $f ${f}.new

    mv $f ${f}.bkp
    mv ${f}.new $f
done
