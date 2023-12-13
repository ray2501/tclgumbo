# -*- tcl -*-
# Tcl package index file, version 1.1
#
if {[package vsatisfies [package provide Tcl] 9.0-]} {
    package ifneeded tclgumbo 0.2.7 \
	    [list load [file join $dir libtcl9tclgumbo0.2.7.so] [string totitle tclgumbo]]
} else {
    package ifneeded tclgumbo 0.2.7 \
	    [list load [file join $dir libtclgumbo0.2.7.so] [string totitle tclgumbo]]
}
