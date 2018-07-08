Gumbo
=====

[Google Gumbo](https://github.com/google/gumbo-parser) is an implementation of 
the [HTML5 parsing algorithm](http://www.whatwg.org/specs/web-apps/current-work/multipage/#auto-toc-12) implemented
as a pure C99 library with no outside dependencies.  It's designed to serve
as a building block for other tools and libraries such as linters,
validators, templating languages, and refactoring and analysis tools.

Goals & features:

* Fully conformant with the [HTML5 spec](http://www.whatwg.org/specs/web-apps/current-work/multipage/).
* Robust and resilient to bad input.
* Simple API that can be easily wrapped by other languages.
* Support for source locations and pointers back to the original text.
* Relatively lightweight, with no outside dependencies.
* Passes all [html5lib-0.95 tests](https://github.com/html5lib/html5lib-tests).
* Tested on over 2.5 billion pages from Google's index.


Tclgumbo commands and variables
=====

Tclgumbo is Gumbo Tcl bindings.  

Implement commands:  
gumbo::parse  
gumbo::destroy_output  
gumbo::output_get_root  
gumbo::node_get_type  
gumbo::element_get_tag_name  
gumbo::element_get_children  
gumbo::text_get_text  
gumbo::element_get_attributes  
gumbo::element_get_tag_open  
gumbo::element_get_tag_close  

Add variables:  
gumbo::GUMBO_NODE_DOCUMENT  
gumbo::GUMBO_NODE_ELEMENT  
gumbo::GUMBO_NODE_TEXT  
gumbo::GUMBO_NODE_CDATA  
gumbo::GUMBO_NODE_COMMENT  
gumbo::GUMBO_NODE_WHITESPACE  


Install
=====

Installation (Linux/GCC):  
cd tclgumbo  
./configure  
make  
make install  

Installation (Winodws/MSYS/MinGW):  
cd tclgumbo  
./configure  
make  
make install  

Installation (Winodws/Visual Studio):  
VS does not support C std99. I don't know how to use VS to build this package.


Example
=====

    package require tclgumbo

    set html "<html><body><p>Hello World</p></body></html>"
    set output [gumbo::parse $html]
    set root [gumbo::output_get_root $output]

    proc getTextContent {node} {
        set type [gumbo::node_get_type $node]
        set textContent ""

        if {$type == $gumbo::GUMBO_NODE_ELEMENT} {
        set children [gumbo::element_get_children $node]
        foreach child_node $children {
            set returntext [getTextContent $child_node]
            append textContent $returntext
        }
        } elseif {$type == $gumbo::GUMBO_NODE_TEXT} {
        set returntext [gumbo::text_get_text $node];
        append textContent $returntext
        }

        return $textContent
    }

    set mytext [getTextContent $root]
    puts "Get text: $mytext"
    gumbo::destroy_output $output
