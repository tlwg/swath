#!/bin/sh

set -e

${SWATH} -d ${DICTDIR} -u u,u < ${srcdir}/long.txt > long-out.txt

if ! cmp long-out.txt ${srcdir}/long-wseg.txt >/dev/null; then
  exit 1
fi

