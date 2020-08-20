#include <inttypes.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavutil/error.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STR_LEN(x) sizeof(x) / sizeof(char)

AVDictionary *options = NULL;

static void save_gray_frame(unsigned char *buf, int wrap, int xsize, int ysize,
                            char *filename)
{
  FILE *f;
  int i;
  f = fopen(filename, "w");
  // writing the minimal required header for a pgm file format
  // portable graymap format ->
  // https://en.wikipedia.org/wiki/Netpbm_format#PGM_example
  fprintf(f, "P5\n%d %d\n%d\n", xsize, ysize, 255);

  // writing line by line
  for (i = 0; i < ysize; i++)
    fwrite(buf + i * wrap, 1, xsize, f);
  fclose(f);
}

int main(int argc, char **argv)
{

  AVFormatContext *pFormatCtx = avformat_alloc_context();

  const char *filename = argv[1];

  /* Read file header and context */
  if (avformat_open_input(&pFormatCtx, filename, NULL, NULL) != 0) {
    fprintf(stderr, "Error while opening file %s", filename);
  }

  printf("Format: %s,\nDuration: % ld\n", pFormatCtx->iformat->long_name,
         pFormatCtx->duration);

  /* Read data from media */
  avformat_find_stream_info(pFormatCtx, NULL);

  /* Loop through streams */
  /* for (int i = 0; i < pFormatCtx->nb_streams; i++) { */
  /* } */

  AVCodecParameters *pLocalCodecParameters = pFormatCtx->streams[0]->codecpar;

  AVCodec *pLocalCodec = avcodec_find_decoder(pLocalCodecParameters->codec_id);

  if (pLocalCodecParameters->codec_type == AVMEDIA_TYPE_VIDEO) {
    printf("Video Codec: res [ %d x %d ]\n", pLocalCodecParameters->width,
           pLocalCodecParameters->height);
  } else if (pLocalCodecParameters->codec_type == AVMEDIA_TYPE_AUDIO) {
    printf("Audio Codec: %d channels, sample rate %d",
           pLocalCodecParameters->channels, pLocalCodecParameters->sample_rate);
  }

  printf("Codec %s ID %d bit rate %ld\n", pLocalCodec->long_name,
         pLocalCodec->id, pLocalCodecParameters->bit_rate);

  AVCodecContext *pCodecContext = avcodec_alloc_context3(pLocalCodec);
  avcodec_parameters_to_context(pCodecContext, pLocalCodecParameters);
  avcodec_open2(pCodecContext, pLocalCodec, NULL);

  AVPacket *pPacket = av_packet_alloc();
  AVFrame *pFrame = av_frame_alloc();

  int j = 0;

  while (av_read_frame(pFormatCtx, pPacket) >= 0 && j < 4) {
    avcodec_send_packet(pCodecContext, pPacket);
    avcodec_receive_frame(pCodecContext, pFrame);

    printf("Frame %c (%d) pts %ld dts %ld key_frame %d [codec_picture_number %d, display_picture_number %d]\n",
           av_get_picture_type_char(pFrame->pict_type),
           pCodecContext->frame_number, pFrame->pts, pFrame->pkt_dts,
           pFrame->key_frame, pFrame->coded_picture_number,
           pFrame->display_picture_number);

    ++j;

    char ctr[3];
    sprintf(ctr, "%d", j);

    save_gray_frame(pFrame->data[0], pFrame->linesize[0], pFrame->width,
                    pFrame->height, ctr);
  }

  return 0;
}
