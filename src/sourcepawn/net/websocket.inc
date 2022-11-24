#if defined _net_websocket_included
#endinput
#endif

#define _net_websocket_included

methodmap WebSocket {
    public static native void Init();
    public static native void UseSSL(bool use_ssl);
    public static native void SetURL(const char[] url);
    public static native void Connect();
    public static native bool Connected();
    public static native void Disconnect();
    // Note: Once you called this function, then the JSON will be automatically freed!
    public static native void Send(JSON json);
}

forward void OnWebSocketMessageReceived(JSON json);