autobuild build && \
autobuild package | tee /tmp/cefpackage
URL="file://$(grep '^wrote' /tmp/cefpackage | sed -e 's/^wrote[[:space:]]*//')"
MD5SUM="$(grep '^md5' /tmp/cefpackage | sed -e 's/^md5[[:space:]]*//')"
configfile="/opt/secondlife/viewers/singularity/SingularityViewer/linden/autobuild.xml"
autobuild installables --config-file "$configfile" edit llceflib platform="linux64" hash="$MD5SUM" url="$URL"
