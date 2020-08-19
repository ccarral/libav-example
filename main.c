
#include <inttypes.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/error.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

AVDictionary *options = NULL;

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

  return 0;
}
