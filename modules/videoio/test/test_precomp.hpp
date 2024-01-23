// This file is part of OpenCV project.
// It is subject to the license terms in the LICENSE file found in the top-level directory
// of this distribution and at http://opencv.org/license.html.
#ifndef __OPENCV_TEST_PRECOMP_HPP__
#define __OPENCV_TEST_PRECOMP_HPP__

#include <sstream>
#include <algorithm>
#include <numeric>

#include "opencv2/ts.hpp"
#include "opencv2/ts/ocl_test.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/videoio/registry.hpp"
#include "opencv2/core/private.hpp"

namespace cv {

static inline
std::ostream& operator<<(std::ostream& out, const VideoCaptureAPIs& api)
{
    out << cv::videoio_registry::getBackendName(api); return out;
}

static inline
std::ostream& operator<<(std::ostream& out, const VideoAccelerationType& va_type)
{
    struct {
        VideoAccelerationType va_type;
        const char* str;
    } va_types[] = {
            {VIDEO_ACCELERATION_ANY,   "ANY"},
            {VIDEO_ACCELERATION_NONE,  "NONE"},
            {VIDEO_ACCELERATION_D3D11, "D3D11"},
            {VIDEO_ACCELERATION_VAAPI, "VAAPI"},
            {VIDEO_ACCELERATION_MFX,   "MFX"},
    };
    for (const auto& va : va_types) {
        if (va_type == va.va_type) {
            out << va.str;
            return out;
        }
    }
    out << cv::format("UNKNOWN(0x%ux)", static_cast<unsigned int>(va_type));
    return out;
}

static inline void PrintTo(const cv::VideoCaptureAPIs& api, std::ostream* os)
{
    *os << cv::videoio_registry::getBackendName(api);
}

} // namespace


inline std::string fourccToString(int fourcc)
{
    return cv::format("%c%c%c%c",
        (char)(fourcc & 255),
        (char)((fourcc >> 8) & 255),
        (char)((fourcc >> 16) & 255),
        (char)((fourcc >> 24) & 255));
}

inline std::string fourccToStringSafe(int fourcc)
{
    std::string res = fourccToString(fourcc);
    // TODO: return hex values for invalid characters
    std::transform(res.begin(), res.end(), res.begin(), [](char c) -> char { return (c >= '0' && c <= 'z') ? c : (c == ' ' ? '_' : 'x'); });
    return res;
}

inline int fourccFromString(const std::string &fourcc)
{
    if (fourcc.size() != 4) return 0;
    return cv::VideoWriter::fourcc(fourcc[0], fourcc[1], fourcc[2], fourcc[3]);
}

inline void generateFrame(int i, int frame_count, cv::Mat & frame)
{
    using namespace cv;
    using namespace std;
    int offset = (((i * 5) % frame_count) - frame_count / 2) * (frame.cols / 2) / frame_count;
    frame(cv::Rect(0, 0, frame.cols / 2 + offset, frame.rows)) = Scalar(255, 255, 255);
    frame(cv::Rect(frame.cols / 2 + offset, 0, frame.cols - frame.cols / 2 - offset, frame.rows)) = Scalar(0, 0, 0);
    std::string str = cv::format("%02d", i+1);
    int baseLine = 0;
    Size box = getTextSize(str, FONT_HERSHEY_COMPLEX, 2, 5, &baseLine);
    putText(frame, str, Point((frame.cols - box.width) / 2, (frame.rows - box.height) / 2 + baseLine),
            FONT_HERSHEY_COMPLEX, 2, Scalar(0, 0, 255), 5, LINE_AA);
    Point p(i * frame.cols / (frame_count - 1), i * frame.rows / (frame_count - 1));
    circle(frame, p, 50, Scalar(200, 25, 55), 8, LINE_AA);
#if 0
    imshow("frame", frame);
    waitKey();
#endif
}

class BunnyParameters
{
public:
    inline static int    getWidth()  { return 672; }
    inline static int    getHeight() { return 384; }
    inline static int    getFps()    { return 24; }
    inline static double getTime()   { return 5.21; }
    inline static int    getCount()  { return cvRound(getFps() * getTime()); }
    inline static std::string getFilename(const std::string &ext)
    {
        return cvtest::TS::ptr()->get_data_path() + "video/big_buck_bunny" + ext;
    }
};


static inline bool isBackendAvailable(cv::VideoCaptureAPIs api, const std::vector<cv::VideoCaptureAPIs>& api_list)
{
    for (size_t i = 0; i < api_list.size(); i++)
    {
        if (api_list[i] == api)
            return true;
    }
    return false;
}

#endif
