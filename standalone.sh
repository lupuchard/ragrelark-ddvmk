rm -r Standalones

mkdir Standalones

cp Ragrelark Standalones/
cp -R data      Standalones/
rsync -aq --progress resources Standalones/ --exclude raw-images
cp icon.bmp  Standalones/

