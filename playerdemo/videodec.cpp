﻿
#include <QDebug>
#include <QTime>

#include "videodec.h"
#include "videoctl.h"
#include "videodataoprator.h"

# pragma execution_character_set("utf-8")

VideoDec::VideoDec()
{

}

void VideoDec::run()
{
    int ret;
    int got_frame = 0;

    AVFormatContext *ic = VideoCtl::GetInstance()->GetAVFormatCtx();
    VideoDataOprator *pVideoDataOprator = VideoCtl::GetInstance()->GetVideoDataOprator();

    AVRational tb = video_st->time_base;
    AVRational frame_rate = av_guess_frame_rate(ic, video_st, nullptr);

    double pts;
    double duration;

    AVRational frame_rate_t;
    frame_rate_t.num = frame_rate.den;
    frame_rate_t.den = frame_rate.num;

    duration = (frame_rate.num && frame_rate.den ? av_q2d(frame_rate_t) : 0);

	AVFrame *frame = av_frame_alloc();
	AVPacket pkt;
	AVFrame *picture = av_frame_alloc();
	SwsContext *pSwsContext = nullptr;

 

    while (m_bRunning)
    {
// 		QTime t;
// 		t.start();

        if (pVideoDataOprator->GetAVPacketData(pkt, VIDEO_DATA) == false)
        {
            msleep(10);
            continue;
        }

        memset(frame, 0, sizeof(AVFrame));
        avcodec_decode_video2(video_avctx, frame, &got_frame, &pkt);

        //qDebug() << "初始化图片空间";

		if (got_frame != 0)
		{
            pts = (frame->pts == AV_NOPTS_VALUE) ? NAN : frame->pts * av_q2d(tb);

            Frame stFrame;
            stFrame.pts = pts;
            stFrame.pos = av_frame_get_pkt_pos(frame);
            stFrame.duration = duration;
            memcpy(&stFrame.frame, frame, sizeof (AVFrame));
            
			while (pVideoDataOprator->PutAVFrameData(&stFrame, VIDEO_DATA) == false)
			{
				msleep(10);
			}
		}

		//qDebug("解码一帧消耗: %d ms", t.elapsed());
    }

	//sws_freeContext(pSwsContext);

	av_frame_free(&frame);
	//av_frame_free(&picture);
}

void VideoDec::OnStartDec()
{
    StartThread();
}
