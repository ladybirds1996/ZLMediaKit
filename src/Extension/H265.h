﻿/*
 * Copyright (c) 2016 The ZLMediaKit project authors. All Rights Reserved.
 *
 * This file is part of ZLMediaKit(https://github.com/xia-chu/ZLMediaKit).
 *
 * Use of this source code is governed by MIT license that can be found in the
 * LICENSE file in the root of the source tree. All contributing project authors
 * may be found in the AUTHORS file in the root of the source tree.
 */

#ifndef ZLMEDIAKIT_H265_H
#define ZLMEDIAKIT_H265_H

#include "Frame.h"
#include "Track.h"
#include "Util/base64.h"
#include "H264.h"
#define H265_TYPE(v) (((uint8_t)(v) >> 1) & 0x3f)
using namespace toolkit;

namespace mediakit {

bool getHEVCInfo(const string &strVps, const string &strSps, int &iVideoWidth, int &iVideoHeight, float &iVideoFps);

/**
 * 265帧类
 */
class H265Frame : public FrameImp {
public:
    using Ptr = std::shared_ptr<H265Frame>;
    enum {
        NAL_TRAIL_N = 0,
        NAL_TRAIL_R = 1,
        NAL_TSA_N = 2,
        NAL_TSA_R = 3,
        NAL_STSA_N = 4,
        NAL_STSA_R = 5,
        NAL_RADL_N = 6,
        NAL_RADL_R = 7,
        NAL_RASL_N = 8,
        NAL_RASL_R = 9,
        NAL_BLA_W_LP = 16,
        NAL_BLA_W_RADL = 17,
        NAL_BLA_N_LP = 18,
        NAL_IDR_W_RADL = 19,
        NAL_IDR_N_LP = 20,
        NAL_CRA_NUT = 21,
        NAL_RSV_IRAP_VCL22 = 22,
        NAL_RSV_IRAP_VCL23 = 23,

        NAL_VPS = 32,
        NAL_SPS = 33,
        NAL_PPS = 34,
        NAL_AUD = 35,
        NAL_EOS_NUT = 36,
        NAL_EOB_NUT = 37,
        NAL_FD_NUT = 38,
        NAL_SEI_PREFIX = 39,
        NAL_SEI_SUFFIX = 40,
    };

    bool keyFrame() const override;
    bool configFrame() const override;
    static bool isKeyFrame(int type, const char* ptr);

protected:
    friend class FrameImp;
    friend class ResourcePool_l<H265Frame>;
    H265Frame();
};

class H265FrameNoCacheAble : public FrameFromPtr {
public:
    using Ptr = std::shared_ptr<H265FrameNoCacheAble>;

    H265FrameNoCacheAble(char *ptr, size_t size, uint32_t dts,uint32_t pts, size_t prefix_size = 4);
    bool keyFrame() const override;
    bool configFrame() const override;
};

/**
* 265视频通道
*/
class H265Track : public VideoTrack {
public:
    using Ptr = std::shared_ptr<H265Track>;

    /**
     * 不指定sps pps构造h265类型的媒体
     * 在随后的inputFrame中获取sps pps
     */
    H265Track() = default;

    /**
     * 构造h265类型的媒体
     * @param vps vps帧数据
     * @param sps sps帧数据
     * @param pps pps帧数据
     * @param vps_prefix_len 265头长度，可以为3个或4个字节，一般为0x00 00 00 01
     * @param sps_prefix_len 265头长度，可以为3个或4个字节，一般为0x00 00 00 01
     * @param pps_prefix_len 265头长度，可以为3个或4个字节，一般为0x00 00 00 01
     */
    H265Track(const string &vps,const string &sps, const string &pps,int vps_prefix_len = 4, int sps_prefix_len = 4, int pps_prefix_len = 4);

    /**
     * 返回不带0x00 00 00 01头的vps/sps/pps
     */
    const string &getVps() const;
    const string &getSps() const;
    const string &getPps() const;

    bool ready() override;
    CodecId getCodecId() const override;
    int getVideoWidth() const override;
    int getVideoHeight() const override;
    float getVideoFps() const override;
    void inputFrame(const Frame::Ptr &frame) override;

private:
    void onReady();
    Sdp::Ptr getSdp() override;
    Track::Ptr clone() override;
    void inputFrame_l(const Frame::Ptr &frame);
    void insertConfigFrame(const Frame::Ptr &frame);

private:
    bool _is_idr = false;
    int _width = 0;
    int _height = 0;
    float _fps = 0;
    string _vps;
    string _sps;
    string _pps;
};

}//namespace mediakit
#endif //ZLMEDIAKIT_H265_H