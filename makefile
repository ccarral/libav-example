
main: main.c
	clang main.c -o main `pkg-config --cflags --libs libavcodec libavdevice libavformat libavutil`
