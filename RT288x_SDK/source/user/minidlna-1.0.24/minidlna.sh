#!/bin/sh

DLNA_FILE=/etc/minidlna.conf

if [ ! -n "$2" ]; then
  echo "insufficient arguments!"
  echo "Usage: $0 <name> <shared_dir> ..."
  echo "$0 MTK_DMS /media/sda1/DLNA .."
  exit 0
fi

DLNA_NAME="$1"
DLNA_DIR1="$2"
DLNA_DIR2="$3"
DLNA_DIR3="$4"
DLNA_DIR4="$5"

echo "# port for HTTP (descriptions, SOAP, media transfer) traffic" > $DLNA_FILE
echo "port=8200" >> $DLNA_FILE

echo "# network interfaces to serve, comma delimited" >> $DLNA_FILE
echo "network_interface=br0" >> $DLNA_FILE

echo "# set this to the directory you want scanned.
# * if have multiple directories, you can have multiple media_dir= lines
# * if you want to restrict a media_dir to a specific content type, you
#   can prepend the type, followed by a comma, to the directory:
#   + "A" for audio  (eg. media_dir=A,/home/jmaggard/Music)
#   + "V" for video  (eg. media_dir=V,/home/jmaggard/Videos)
#   + "P" for images (eg. media_dir=P,/home/jmaggard/Pictures)" >> $DLNA_FILE
echo "media_dir=$DLNA_DIR1" >> $DLNA_FILE

if [ ! -z $DLNA_DIR2 ]; then
echo "media_dir=$DLNA_DIR2" >> $DLNA_FILE
fi

if [ ! -z $DLNA_DIR3 ]; then
echo "media_dir=$DLNA_DIR3" >> $DLNA_FILE
fi

if [ ! -z $DLNA_DIR4 ]; then
echo "media_dir=$DLNA_DIR4" >> $DLNA_FILE
fi

echo "# set this if you want to customize the name that shows up on your clients
friendly_name=$DLNA_NAME" >> $DLNA_FILE

echo "# set this if you would like to specify the directory where you want MiniDLNA to store its database and album art cache
#db_dir=/var"  >> $DLNA_FILE

echo "# set this if you would like to specify the directory where you want MiniDLNA to store its log file
#log_dir=/var/log"  >> $DLNA_FILE

echo "# set this to change the verbosity of the information that is logged
# each section can use a different level: off, fatal, error, warn, info, or debug
#log_level=general,artwork,database,inotify,scanner,metadata,http,ssdp,tivo=warn

# this should be a list of file names to check for when searching for album art
# note: names should be delimited with a forward slash ("/")
album_art_names=Cover.jpg/cover.jpg/AlbumArtSmall.jpg/albumartsmall.jpg/AlbumArt.jpg/albumart.jpg/Album.jpg/album.jpg/Folder.jpg/folder.jpg/Thumb.jpg/thumb.jpg" >> $DLNA_FILE

echo "# set this to no to disable inotify monitoring to automatically discover new files
# note: the default is yes
inotify=no" >> $DLNA_FILE

echo "# set this to yes to enable support for streaming .jpg and .mp3 files to a TiVo supporting HMO
enable_tivo=no" >> $DLNA_FILE

echo "# set this to strictly adhere to DLNA standards.
# * This will allow server-side downscaling of very large JPEG images,
#   which may hurt JPEG serving performance on (at least) Sony DLNA products.
strict_dlna=no" >> $DLNA_FILE

echo "# default presentation url is http address on port 80
#presentation_url=http://www.mylan/index.php" >> $DLNA_FILE

echo "# notify interval in seconds. default is 895 seconds.
notify_interval=900" >> $DLNA_FILE

echo "# serial and model number the daemon will report to clients
# in its XML description
serial=12345678
model_number=1" >> $DLNA_FILE

echo "# specify the path to the MiniSSDPd socket
#minissdpdsocket=/var/run/minissdpd.sock" >> $DLNA_FILE

echo "# use different container as root of the tree
# possible values:
#   + "." - use standard container (this is the default)
#   + "B" - "Browse Directory"
#   + "M" - "Music"
#   + "V" - "Video"
#   + "P" - "Pictures"
# if you specify "B" and client device is audio-only then "Music/Folders" will be used as root
#root_container=." >> $DLNA_FILE
