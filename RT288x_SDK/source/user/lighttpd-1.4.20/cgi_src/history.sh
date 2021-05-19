#!/bin/sh

# define
HISTORY="./History"
PATCHLIST="./PatchList"

# HTTP header
echo "Content-type: text/html"
echo ""

# web header
echo "<html><head><title>RT288x SDK History</title>"

echo '<META HTTP-EQUIV="Pragma" CONTENT="no-cache">'
echo '<META HTTP-EQUIV="Expires" CONTENT="-1">'
echo '<meta http-equiv="content-type" content="text/html; charset=utf-8">'
echo '<link rel="stylesheet" href="/style/normal_ws.css" type="text/css">'
echo '</head>'

# web body
echo "<body><table class=body>"

# parse History file
echo "<tr><td>"
sed -e '
s_\(^Mediatek APSoC.*\)_<h1>\1</h1><hr />_
s_\(^Version.*\)_<h3>\1</h3>_
s_^- \(.*\)_- \1 <br />_
s_\(^$\)_\1 <br />_
s_\(^==.*\)__
' $HISTORY
echo "</td></tr>"

# parse Patch List file
if [ -f $PATCHLIST ]; then
	echo "<tr><td>"
	sed -e '
	s_\(^Mediatek APSoC.*\)_<h1>\1</h1><hr />_
	s_^- \(.*\)_- \1 <br />_
	s_\(^$\)_\1 <br />_
	s_\(^==.*\)__
	' $PATCHLIST
	echo "</td></tr>"
fi
echo "</table>"
echo "</body></html>"
