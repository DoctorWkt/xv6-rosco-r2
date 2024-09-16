all:
	(cd fs_skel; rsync -a . ../build)
	(cd kern; make)
	(cd tools; make)
	(cd lib; make)
	(cd cmds; make)
	tools/mkfs fs.img build

sdcard.img: all
	./mk_fs_img

sd-img.zip: sdcard.img
	zip -9 sd-img.zip sdcard.img

clean:
	(cd kern; make clean)
	(cd tools; make clean)
	(cd lib; make clean)
	(cd cmds; make clean)
	rm -rf build/*
	rm -f fs.img sdcard.img sd-img.zip
