# This script downloads and patches SOAPpy
# The module must then be (manually) installed to Python

SOURCE_ARCHIVE=python-soappy_0.12.0.orig.tar.gz
SOURCE_HASH_KNOWN='d0d29f9b6fb27bfadc69b64a36321e20'
DIFF_ARCHIVE=python-soappy_0.12.0-4.diff.gz
DIFF_HASH_KNOWN='b7574745c985945c0532a6608822452f'

WGET_OPTIONS='--timestamping --no-verbose'

for item in "$SOURCE_ARCHIVE $SOURCE_HASH_KNOWN" "$DIFF_ARCHIVE $DIFF_HASH_KNOWN"
do
  set -- $item # $1 = _ARCHIVE ; $2 = _HASH_KNOWN
  wget $WGET_OPTIONS http://ftp.de.debian.org/debian/pool/main/p/python-soappy/$1
  HASH=$(md5sum $1)
  if [ ${HASH%% *} != $2 ]
  then
    echo "Downloaded file $1 has incorrect hash"
    exit
  fi
  gzip --decompress --force $1
done

tar -xf python-soappy_0.12.0.orig.tar

patch --directory='SOAPpy-0.12.0' --strip=1 < ./python-soappy_0.12.0-4.diff

cd ./SOAPpy-0.12.0
#dpatch --chdir --workdir ./SOAPpy-0.12.0 apply-all
dpatch apply-all
cd ..
