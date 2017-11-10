#!/bin/sh

set -e

${SWATH} -d ${DICTDIR} -u u,u < ${srcdir}/mixed.txt > mixed-out.txt

if ! cmp mixed-out.txt ${srcdir}/mixed-wseg.txt >/dev/null; then
  exit 1
fi

