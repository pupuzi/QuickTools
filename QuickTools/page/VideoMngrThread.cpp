#include "VideoMngrThread.h"
#include <QDebug>
#include <QTextCodec>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}

VideoMngrThread::VideoMngrThread()
{
    qDebug() << av_version_info();
}

VideoMngrThread::~VideoMngrThread()
{

}

bool VideoMngrThread::compress_h265(const QString& strName)
{
    QString strSrcName = strName;
    QString strDestName = strName.mid(0, strName.lastIndexOf(".")) + "_h265.mp4";
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QByteArray utf8Dest = codec->fromUnicode(strDestName);
    QByteArray utf8Src = codec->fromUnicode(strSrcName);
    const char* outputFileName = utf8Dest;
    const char* inputFileName = utf8Src;

    AVFormatContext* inputFormatCtx = nullptr;
    AVFormatContext* outputFormatCtx = nullptr;
    AVCodecContext* videoCodecContext = nullptr;
    AVPacket* packet = av_packet_alloc();

    // 打开输入文件
    avformat_open_input(&inputFormatCtx, inputFileName, nullptr, nullptr);
    avformat_find_stream_info(inputFormatCtx, nullptr);

    // 创建输出文件上下文
    avformat_alloc_output_context2(&outputFormatCtx, nullptr, nullptr, outputFileName);
    if (!outputFormatCtx) {
       printf("Could not create output format context\n");
       return -1;
    }

    // 复制输入流的参数到输出流
    AVCodecContext* codecCtx = nullptr;
    int videoStreamIndex = -1;
    for (unsigned int i = 0; i < inputFormatCtx->nb_streams; i++) {
       AVStream* inStream = inputFormatCtx->streams[i];
       AVStream* outStream = avformat_new_stream(outputFormatCtx, nullptr);
       if (!outStream) {
           printf("Could not create new stream\n");
           return -1;
       }

       // 复制编解码器参数
       outStream->codecpar->codec_type = inStream->codecpar->codec_type;
       outStream->codecpar->codec_id = inStream->codecpar->codec_id;
       outStream->codecpar->bit_rate = inStream->codecpar->bit_rate;
       outStream->codecpar->width = inStream->codecpar->width;
       outStream->codecpar->height = inStream->codecpar->height;
       outStream->codecpar->format = inStream->codecpar->format;
       outStream->codecpar->sample_aspect_ratio = inStream->codecpar->sample_aspect_ratio;
       outStream->codecpar->channel_layout = inStream->codecpar->channel_layout;
       outStream->codecpar->channels = inStream->codecpar->channels;
       outStream->codecpar->sample_rate = inStream->codecpar->sample_rate;
       outStream->codecpar->block_align = inStream->codecpar->block_align;
       outStream->time_base = inStream->time_base;

       // 如果流是视频流并且需要转换编码，则设置新的编解码器
       if (inStream->codecpar->codec_type == AVMEDIA_TYPE_VIDEO &&
           inStream->codecpar->codec_id != AV_CODEC_ID_HEVC) {
           AVCodec* codec = (AVCodec*)avcodec_find_encoder(AV_CODEC_ID_HEVC);
           if (!codec) {
               printf("Could not find HEVC encoder\n");
               return -1;
           }

           videoStreamIndex = i;
           codecCtx = avcodec_alloc_context3(codec);
           avcodec_parameters_to_context(codecCtx, outStream->codecpar);
           avcodec_open2(codecCtx, codec, nullptr);
           outStream->codecpar->codec_id = AV_CODEC_ID_HEVC;
       }
    }

    // 获取视频流
    AVStream* videoStream = inputFormatCtx->streams[videoStreamIndex];

    // 获取视频解码器
    const AVCodec* videoCodec = avcodec_find_decoder(videoStream->codecpar->codec_id);
    if (!videoCodec) {
        avformat_close_input(&inputFormatCtx);
        return false;
    }

    // 创建并打开视频解码器上下文
    videoCodecContext = avcodec_alloc_context3(videoCodec);
    if (!videoCodecContext) {
        avformat_close_input(&inputFormatCtx);
        return false;
    }

    //视频流参数去填充上下文context
    avcodec_parameters_to_context(videoCodecContext, videoStream->codecpar);
    if (avcodec_open2(videoCodecContext, videoCodec, nullptr) < 0) {
        avformat_close_input(&inputFormatCtx);
        avcodec_free_context(&videoCodecContext);
        return false;
    }

    // 打开输出文件
    if (!(outputFormatCtx->oformat->flags & AVFMT_NOFILE)) {
       avio_open(&outputFormatCtx->pb, outputFileName, AVIO_FLAG_WRITE);
    }

    // 写入文件头
    avformat_write_header(outputFormatCtx, nullptr);

    // 读取数据包并写入输出文件
    int64_t next_pts = 0;
    AVFrame* frame = av_frame_alloc();
    AVPacket* transcodedPacket = av_packet_alloc();
    while (av_read_frame(inputFormatCtx, packet) >= 0) {
       // 找到输出流中对应的流
       AVStream *outStream = nullptr;
       for (unsigned int i = 0; i < outputFormatCtx->nb_streams; i++) {
           if (outputFormatCtx->streams[i]->codecpar->codec_type == packet->stream_index) {
               outStream = outputFormatCtx->streams[i];
               break;
           }
       }
       if (!outStream) {
           // 如果没有找到对应的输出流，则跳过该数据包
           av_packet_unref(packet);
           continue;
       }

       // 如果是视频流并且需要转码，则进行转码
       if (packet->stream_index == inputFormatCtx->streams[videoStreamIndex]->index &&
           outStream->codecpar->codec_id == AV_CODEC_ID_HEVC) {
           av_packet_unref(transcodedPacket);
           avcodec_send_packet(videoCodecContext, packet);

           while (avcodec_receive_frame(videoCodecContext, frame) == 0) {

               int64_t timebase_ticks_per_second = av_q2d(outputFormatCtx->streams[videoStreamIndex]->time_base) * 1000000;
               // 设置输出帧的PTS
               // pts_increment是每帧之间的时间增量，通常等于输入帧的PTS差值或帧率倒数
               int64_t pts_increment = av_rescale_q(1, inputFormatCtx->streams[videoStreamIndex]->time_base, outputFormatCtx->streams[videoStreamIndex]->time_base);
               frame->pts = next_pts;
               next_pts += pts_increment;

               avcodec_send_frame(codecCtx, frame);
               avcodec_receive_packet(codecCtx, transcodedPacket);

               transcodedPacket->pts = frame->pts;
               transcodedPacket->dts = frame->pts;
               transcodedPacket->duration = pts_increment;
               // 将转码后的数据包写入输出文件
               av_interleaved_write_frame(outputFormatCtx, transcodedPacket);
               av_packet_unref(transcodedPacket);
           }

           av_packet_unref(packet);
       } else {
           // 如果是其他流或者不需要转码的视频流，则直接写入输出文件
           av_interleaved_write_frame(outputFormatCtx, packet);
       }
    }

    // 写入文件尾
    av_write_trailer(outputFormatCtx);

    // 清理资源
    avcodec_free_context(&codecCtx);
    av_packet_free(&packet);
    avio_close(outputFormatCtx->pb);
    avformat_free_context(outputFormatCtx);

    return true;
}

void VideoMngrThread::startThread(const QStringList& strNames)
{
    m_strNames = strNames;

    start();
}

void VideoMngrThread::startThread(const QString& strName)
{
    m_strNames.append(strName);

    start();
}

void VideoMngrThread::run()
{
    foreach(QString strName, m_strNames)
    {
        compress_h265(strName);
    }
}
