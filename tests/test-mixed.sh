#!/bin/sh

set -e


echo -n "Testing maximal matching scheme... "

${SWATH} -m max -d ${DICTDIR} -u u,u < ${srcdir}/mixed.txt > mixed-out.txt

if ! cmp mixed-out.txt ${srcdir}/mixed-wseg.txt >/dev/null; then
  echo "FAIL!"
  exit 1
fi
echo "OK"


echo -n "Testing longest matching scheme... "

${SWATH} -m long -d ${DICTDIR} -u u,u < ${srcdir}/mixed.txt > mixed-out.txt

if ! cmp mixed-out.txt ${srcdir}/mixed-wseg.txt >/dev/null; then
  echo "FAIL!"
  exit 1
fi
echo "OK"

