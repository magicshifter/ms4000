.PHONY:	build

all:	build

build:
	cp -fvp ../../MS4.proto public/MS4.proto
	npm run build && ls -alF dist/

clean:
	rm -rf dist/*

package:
	rsync -avpz dist/ ../../firmware/data/
	find ../../firmware/data -name "*.map" -exec rm -rf {} \;
	find ../../firmware/data -name "*.html" -exec gzip -f {} \;
	find ../../firmware/data -name "*.js" -exec gzip -f {} \;
	find ../../firmware/data -name "*.css" -exec gzip -f {} \;
	find ../../firmware/data -name "*.json" -exec gzip -f {} \;
	ls -alF ../../firmware/data
	du -h ../../firmware/data

deps:
	npm i
 
