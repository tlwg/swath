#/bin/sh

# Update dictionary from thailatex hyphenation source.
# Usage: update-dict.sh {thailatex-hyphenation-src-dir}

if [ $# -ne 1 ]; then
  echo "Usage: update-dict.sh {thailatex-hyphenation-src-dir}"
  exit 1
fi

TL_HYPH_SRC_DIR=$1

for f in ${TL_HYPH_SRC_DIR}/tdict*.txt; do
  sed 's/-//g' $f > `basename $f`
done

