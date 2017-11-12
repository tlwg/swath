#!/bin/sh

set -e


echo -n "Testing maximal matching scheme... "

${SWATH} -m max -d ${DICTDIR} -u u,u < ${srcdir}/long.txt > long-out.txt

if ! cmp long-out.txt ${srcdir}/long-wseg.txt >/dev/null; then
  echo "FAIL!"
  exit 1
fi
echo "OK"


echo -n "Testing longest matching scheme... "

${SWATH} -m long -d ${DICTDIR} -u u,u < ${srcdir}/long.txt > long-out.txt

if ! cmp long-out.txt ${srcdir}/long-wseg.txt >/dev/null; then
  echo "FAIL!"
  exit 1
fi
echo "OK"

