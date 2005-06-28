#!/bin/sh

svn status src | grep "^\(A\|M\)" | cut -c 8-
