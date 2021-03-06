#pragma once

#include "Codec/IEncoder.h"
#include "VideoCapturer.h"

namespace unity
{
namespace webrtc
{

    class ITexture2D;
    class IGraphicsDevice;
    class NvVideoCapturer : public cricket::VideoCapturer
    {
    public:
        NvVideoCapturer();
        bool EncodeVideoData();
        // Start the video capturer with the specified capture format.
        virtual cricket::CaptureState Start(const cricket::VideoFormat& Format) override
        {
            return cricket::CS_RUNNING;
        }
        // Stop the video capturer.
        virtual void Stop() override
        {
            captureStopped = true;
        }
        // Check if the video capturer is running.
        virtual bool IsRunning() override
        {
            return true;
        }
        // Returns true if the capturer is screencasting. This can be used to
        // implement screencast specific behavior.
        virtual bool IsScreencast() const override
        {
            return false;
        }
        void StartEncoder();
        void SetFrameBuffer(void* frameBuffer);
        void SetEncoder(IEncoder* encoder);
        void SetKeyFrame();
        void SetSize(int32 width, int32 height);
        void CaptureFrame(webrtc::VideoFrame& videoFrame);
        bool CaptureStarted() const { return captureStarted; }
        CodecInitializationResult GetCodecInitializationResult() const;
    private:
        // subclasses override this virtual method to provide a vector of fourccs, in
        // order of preference, that are expected by the media engine.
        bool GetPreferredFourccs(std::vector<uint32>* fourccs) override
        {
            fourccs->push_back(cricket::FOURCC_H264);
            return true;
        }
        void* unityRT = nullptr;

        IEncoder* encoder_;

        //just fake info
        int32 width = 1280;
        int32 height = 720;
        int32 framerate = 60;

        bool captureStarted = false;
        bool captureStopped = false;

    };

    class FrameBuffer : public webrtc::VideoFrameBuffer
    {
    public:
        FrameBuffer(int width,
            int height,
            std::vector<uint8>& data,
            const int encoderId)
            : m_frameWidth(width),
            m_frameHeight(height),
            m_encoderId(encoderId),
            m_buffer(data)
        {}

        //webrtc::VideoFrameBuffer pure virtual functions
        // This function specifies in what pixel format the data is stored in.
        virtual Type type() const override
        {
            //fake I420 to avoid ToI420() being called
            return Type::kI420;
        }
        // The resolution of the frame in pixels. For formats where some planes are
        // subsampled, this is the highest-resolution plane.
        virtual int width() const override
        {
            return m_frameWidth;
        }
        virtual int height() const override
        {
            return m_frameHeight;
        }

        std::vector<uint8>& buffer() const
        {
            return m_buffer;
        }

        // todo(kazuki): remove the method by refactoring video encoding.
        // The id is for identifying encoder which encoded this frame.
        int encoderId() const
        {
            return m_encoderId;
        }

        // Returns a memory-backed frame buffer in I420 format. If the pixel data is
        // in another format, a conversion will take place. All implementations must
        // provide a fallback to I420 for compatibility with e.g. the internal WebRTC
        // software encoders.
        virtual rtc::scoped_refptr<webrtc::I420BufferInterface> ToI420() override
        {
            return nullptr;
        }

    private:
        int m_frameWidth;
        int m_frameHeight;
        int m_encoderId;
        std::vector<uint8>& m_buffer;
    };
    
} // end namespace webrtc
} // end namespace unity
