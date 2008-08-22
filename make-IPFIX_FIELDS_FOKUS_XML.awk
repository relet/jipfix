#!/usr/bin/awk -f


function trimLeft(x)  { sub(/^[ \t]*/,"",x); return x}
function trimRight(x) { sub(/[ \t]*$/,"",x); return x}
function trim(x)      { return trimRight(trimLeft(x))}
function removeQuotes(x) { gsub("\"","",x); return x}


BEGIN {
	FS = ","
	print "<!-- This is a generated file. Do not edit! -->\n\n<field-array>"
}


NF==0 { next }
/#/   { next }


match ($0, /\/\/|\*|\/\*/) {
	split($0, aux, substr($0, RSTART, RLENGTH))
	print substr($0, RSTART, RLENGTH)" "aux[2] }

/\*\// {
	print "*/"
}


NF==6 {
	print "\t<field>"
	print "\t\t<eno>12325</eno>"
	print "\t\t<ftype>" trim($1) "</ftype>"
	print "\t\t<length>" trim($3) "</length>"
	if (trim($4) == "IPFIX_CODING_INT")    print "\t\t<coding>1</coding>";
    if (trim($4) == "IPFIX_CODING_UINT")   print "\t\t<coding>2</coding>";
    if (trim($4) == "IPFIX_CODING_BYTES")  print "\t\t<coding>3</coding>";
    if (trim($4) == "IPFIX_CODING_STRING") print "\t\t<coding>4</coding>";
    if (trim($4) == "IPFIX_CODING_FLOAT")  print "\t\t<coding>5</coding>";
    if (trim($4) == "IPFIX_CODING_NTP")    print "\t\t<coding>6</coding>";
    if (trim($4) == "IPFIX_CODING_IPADDR") print "\t\t<coding>7</coding>";
    print "\t\t<name>" removeQuotes(trim($5)) "</name>"
    print "\t\t<documentation>" removeQuotes(trim($6)) "</documentation>"
	print "\t</field>"
}

END {
	print "</field-array>\n"
}
