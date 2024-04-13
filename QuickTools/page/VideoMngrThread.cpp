#include "VideoMngrThread.h"
#include <QDebug>
#include <QTextCodec>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}

extern "C" {
void log_callback(void* ptr, int level, const char* fmt, va_list vargs) {
    Q_UNUSED(ptr);
    Q_UNUSED(level);

    char line[1024];
    vsnprintf(line, sizeof(line), fmt, vargs);
    qDebug() << line;
}
}

VideoMngrThread::VideoMngrThread()
{
    qDebug() << av_version_info();
    //av_log_set_callback(log_callback);
}

VideoMngrThread::~VideoMngrThread()
{

}

void VideoMngrThread::printErrLog(int nRet)
{
    char error_buffer[AV_ERROR_MAX_STRING_SIZE];
    av_strerror(nRet, error_buffer, sizeof(error_buffer));
    emit sig_showLog(error_buffer);
}

bool VideoMngrThread::compress(const QString& strName)
{
    QString strSrcName = strName;
    QString strDestName = strName.mid(0, strName.lastIndexOf(".")) + "_c.mp4";
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QByteArray utf8Dest = codec->fromUnicode(strDestName);
    QByteArray utf8Src = codec->fromUnicode(strSrcName);
    const char* outputFileName = utf8Dest;
    const char* inputFileName = utf8Src;

    emit sig_showLog("启动压缩: " + strName);

    AVFormatContext* input_format_context = nullptr;
    if(avformat_open_input(&input_format_context, inputFileName, nullptr, nullptr) != 0)
    {
        return false;
    }
    avformat_find_stream_info(input_format_context, NULL);

    AVStream* video_stream = nullptr;
    for(unsigned int i = 0; i < input_format_context->nb_streams; i++)
    {
        if(input_format_context->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            video_stream = input_format_context->streams[i];
            break;
        }
    }

    AVStream* audio_stream = nullptr;
    for(unsigned int i = 0; i < input_format_context->nb_streams; i++)
    {
        if(input_format_context->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO)
        {
            audio_stream = input_format_context->streams[i];
            break;
        }
    }

    AVCodecParameters* video_codec_params = video_stream->codecpar;
    AVCodecParameters* audio_codec_params = audio_stream->codecpar;

    AVFormatContext* output_format_context = nullptr;
    if(avformat_alloc_output_context2(&output_format_context, nullptr, nullptr, outputFileName) < 0)
    {
        return false;
    }

    AVStream* output_video_stream = avformat_new_stream(output_format_context, nullptr);
    AVStream* output_audio_stream = avformat_new_stream(output_format_context, nullptr);

    avcodec_parameters_copy(output_video_stream->codecpar, video_codec_params);
    avcodec_parameters_copy(output_audio_stream->codecpar, audio_codec_params);

    if(avio_open(&output_format_context->pb, outputFileName, AVIO_FLAG_WRITE) < 0)
    {
        return false;
    }

    if(avformat_write_header(output_format_context, nullptr) < 0)
    {
        return false;
    }

    //解码器
    emit sig_showLog("创建解码器...");
    AVCodec* video_decodec = (AVCodec*)avcodec_find_decoder(video_codec_params->codec_id);
    AVCodecContext* video_codec_context = avcodec_alloc_context3(video_decodec);
    avcodec_parameters_to_context(video_codec_context, video_codec_params);
    avcodec_open2(video_codec_context, video_decodec, nullptr);

    AVCodec* audio_decodec = (AVCodec*)avcodec_find_decoder(audio_codec_params->codec_id);
    AVCodecContext* audio_codec_context = avcodec_alloc_context3(audio_decodec);
    avcodec_parameters_to_context(audio_codec_context, audio_codec_params);
    avcodec_open2(audio_codec_context, audio_decodec, nullptr);

    //编码器
    emit sig_showLog("创建编码器...");
    AVCodec* video_encodec = (AVCodec*)avcodec_find_encoder(video_codec_params->codec_id);
    AVCodecContext* video_encoder_context = avcodec_alloc_context3(video_encodec);
    avcodec_parameters_to_context(video_encoder_context, video_codec_params);
    video_encoder_context->time_base = video_stream->time_base;
    video_encoder_context->bit_rate = video_encoder_context->bit_rate / 4;
    if(video_codec_context->bit_rate < 500000)
    {
        video_encoder_context->bit_rate = 500000;
    }
    video_encoder_context->width = 1280;
    video_encoder_context->height = 720;
    avcodec_open2(video_encoder_context, video_encodec, NULL);

    AVCodec* audio_encodec = (AVCodec*)avcodec_find_encoder(audio_codec_params->codec_id);
    AVCodecContext* audio_encoder_context = avcodec_alloc_context3(audio_encodec);
    avcodec_parameters_to_context(audio_encoder_context, audio_codec_params);
    audio_encoder_context->time_base = audio_stream->time_base;
    avcodec_open2(audio_encoder_context, audio_encodec, NULL);

    //编解码写入
    emit sig_showLog("开始压缩...");
    AVFrame* video_frame = av_frame_alloc();
    AVPacket* video_packet = av_packet_alloc();
    AVFrame* audio_frame = av_frame_alloc();
    AVPacket* audio_packet = av_packet_alloc();
    if(!video_frame || !video_packet)
    {
        return false;
    }

    double duration = (double)video_stream->duration / av_q2d(video_stream->time_base);

    int64_t frame_number = 0;
    int64_t frame_total = video_stream->nb_frames;
    AVPacket packet;
    while(av_read_frame(input_format_context, &packet) >= 0)
    {
        if(packet.stream_index == video_stream->index)
        {
            frame_number++;
            if(avcodec_send_packet(video_codec_context, &packet) < 0)
            {
                return false;
            }

            while(avcodec_receive_frame(video_codec_context, video_frame) >= 0)
            {
                if(avcodec_send_frame(video_encoder_context, video_frame) < 0)
                {
                    return false;
                }

                while(avcodec_receive_packet(video_encoder_context, video_packet) >= 0)
                {
                    av_packet_rescale_ts(video_packet, video_encoder_context->time_base, output_video_stream->time_base);
                    video_packet->stream_index = output_video_stream->index;
                    av_interleaved_write_frame(output_format_context, video_packet);
                    av_packet_unref(video_packet);
                }
            }

            //计算转码进度
            //QString strDuration = QString("压制进度: %1 / %2").arg(frame_number).arg(frame_total);
            //emit sig_showLog(strDuration);
        }
        else if(packet.stream_index == audio_stream->index)
        {
            if(avcodec_send_packet(audio_codec_context, &packet) < 0)
            {
                return false;
            }

            while(avcodec_receive_frame(audio_codec_context, audio_frame) >= 0)
            {
                if(avcodec_send_frame(audio_encoder_context, audio_frame) < 0)
                {
                    return false;
                }

                while(avcodec_receive_packet(audio_encoder_context, audio_packet) >= 0)
                {
                    av_packet_rescale_ts(audio_packet, audio_encoder_context->time_base, output_audio_stream->time_base);
                    audio_packet->stream_index = output_audio_stream->index;
                    av_interleaved_write_frame(output_format_context, audio_packet);
                    av_packet_unref(video_packet);
                }
            }
        }

        av_packet_unref(video_packet);
        av_packet_unref(audio_packet);
    }

    av_write_trailer(output_format_context);

    avcodec_free_context(&video_codec_context);
    avcodec_free_context(&video_encoder_context);
    av_frame_free(&video_frame);
    av_packet_free(&video_packet);
    avcodec_free_context(&audio_codec_context);
    avcodec_free_context(&audio_encoder_context);
    av_frame_free(&audio_frame);
    av_packet_free(&audio_packet);
    avformat_close_input(&input_format_context);
    avio_closep(&output_format_context->pb);
    avformat_free_context(output_format_context);

    emit sig_showLog("压缩结束!");

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
        compress(strName);
    }
}
