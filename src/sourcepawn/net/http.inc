#if defined _net_http_included
#endinput
#endif

#define _net_http_included

typedef AsyncSendResult_t = function void(HTTPResponse response);

methodmap HTTPRequest < Handle {
    public native HTTPRequest();
    public native void SetURL(const char[] url);
    public native void SetMethod(const char[] method);
    public native void SetHeader(const char[] key, const char[] value);
    public native void SetBodyText(const char[] data);
    // Note: You need to manually free the JSON variable what you passed in.
    public native void SetBodyJSON(JSON json);
    public native void AppendFileToBody(const char[] path);
    public native void Prepare();
    // You need to free the result handle manually.
    public native HTTPResponse Send(bool use_ssl = false, int timeout = 5);

    // For async send, the result invoke, you do not need to free it manually.
    public native void SendAsync(AsyncSendResult_t resultFn, bool use_ssl = false, int timeout = 5);

    property int HTTPVersion {
        public native set(int val);
    }
}


methodmap HTTPResponse < Handle {
    property int StatusCode {
        public native get();
    }
    property int Length {
        public native get();
    }
    public native void GetBodyAsText(char[] data, int maxlen);
    public native JSON GetBodyAsJSON();
    public native void GetBodyAsFile(const char[] dir_to_save);
}
