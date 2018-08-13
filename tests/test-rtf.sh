#!/bin/sh

set -e

echo -n "Testing RTF... "

${SWATH} -d ${DICTDIR} -f rtf < ${srcdir}/petavatthu1.rtf > petavatthu1-out.rtf

if ! cmp petavatthu1-out.rtf ${srcdir}/petavatthu1-wseg.rtf >/dev/null; then
  echo "FAIL!"
  exit 1
fi
echo "OK"

