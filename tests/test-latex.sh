#!/bin/sh

set -e

echo -n "Testing maximal matching scheme... "

${SWATH} -m max -d ${DICTDIR} -f latex -u u,u < ${srcdir}/thai-latex.tex \
  > thai-latex-out.tex

if ! cmp thai-latex-out.tex ${srcdir}/thai-latex-wseg.tex >/dev/null; then
  echo "FAIL!"
  exit 1
fi
echo "OK"


echo -n "Testing longest matching scheme... "

${SWATH} -m long -d ${DICTDIR} -f latex -u u,u < ${srcdir}/thai-latex.tex \
  > thai-latex-out.tex

if ! cmp thai-latex-out.tex ${srcdir}/thai-latex-wseg.tex >/dev/null; then
  echo "FAIL!"
  exit 1
fi
echo "OK"

