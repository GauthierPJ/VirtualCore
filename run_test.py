#!/usr/bin/python3 

from sys import argv
import compilator as c

print("----* RUN TESTS UNITAIRES *----")
content = c.read_file("test_cmp_1.s")
    
c.verify_content(content,True)

