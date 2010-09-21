#!/bin/bash
for i in toki maca corpus2 pwrutils; do
	rm -rf /usr/local/include/lib$i
	rm -rf /usr/local/lib/lib$i*
	rm -rf /usr/local/lib/$i/
	rm -rf /usr/share/lib$i
done
