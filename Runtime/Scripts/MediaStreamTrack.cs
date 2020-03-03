using System;
using System.Runtime.InteropServices;
using UnityEngine;

namespace Unity.WebRTC
{
    public class MediaStreamTrack : IDisposable
    {
        internal IntPtr self;
        private TrackKind kind;
        private string id;
        private bool disposed;
        private bool enabled;
        private TrackState readyState;
        internal Action<MediaStreamTrack> stopTrack;
        internal Func<MediaStreamTrack, RenderTexture[]> getRts;

        public bool Enabled
        {
            get
            {
                return NativeMethods.MediaStreamTrackGetEnabled(self);
            }
            set
            {
                NativeMethods.MediaStreamTrackSetEnabled(self, value);
            }
        }
        public TrackState ReadyState
        {
            get
            {
                return NativeMethods.MediaStreamTrackGetReadyState(self);
            }
            private set { }
        }

        public TrackKind Kind { get => kind; private set { } }
        public string Id { get => id; private set { } }

        internal MediaStreamTrack(IntPtr ptr)
        {
            self = ptr;
            WebRTC.Table.Add(self, this);
            kind = NativeMethods.MediaStreamTrackGetKind(self);
            id = Marshal.PtrToStringAnsi(NativeMethods.MediaStreamTrackGetID(self));
        }

        ~MediaStreamTrack()
        {
            this.Dispose();
            WebRTC.Table.Remove(self);
        }

        public void Dispose()
        {
            UnityEngine.Debug.Log("MediaStreamTrack Dispose");

            if (this.disposed)
            {
                return;
            }
            if (self != IntPtr.Zero && !WebRTC.Context.IsNull)
            {
                WebRTC.Context.DeleteMediaStreamTrack(self);
                self = IntPtr.Zero;
            }
            this.disposed = true;
            GC.SuppressFinalize(this);

            UnityEngine.Debug.Log("MediaStreamTrack Dispose end");
        }

        //Disassociate track from its source(video or audio), not for destroying the track
        public void Stop()
        {
            stopTrack(this);
        }
    }

    public enum TrackKind
    {
        Audio,
        Video
    }
    public enum TrackState
    {
        Live,
        Ended
    }

    public class RTCTrackEvent
    {
        private IntPtr self;
        private readonly MediaStreamTrack track;
        private readonly RTCRtpTransceiver transceiver;

        public MediaStreamTrack Track
        {
            get => track;
        }

        public RTCRtpTransceiver Transceiver
        {
            get => transceiver;
        }

        internal RTCTrackEvent(IntPtr ptr)
        {
            self = ptr;
            track = new MediaStreamTrack(NativeMethods.TransceiverGetTrack(self));
            transceiver = new RTCRtpTransceiver(self);
        }
    }

}

