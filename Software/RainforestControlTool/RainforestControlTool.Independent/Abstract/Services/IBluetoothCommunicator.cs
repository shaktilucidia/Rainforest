namespace RainforestControlTool.Independent.Abstract.Services;

/// <summary>
/// Delegate, called when connection established
/// </summary>
public delegate void OnConnectDelegate();

/// <summary>
/// Delegate, called when new data received
/// </summary>
public delegate void OnDataReceivedDelegate(byte[] data);

/// <summary>
/// Delegate, called when connection lost
/// </summary>
public delegate void OnDisconnectDelegate();

/// <summary>
/// Service to communicate with bluetooth devices
/// </summary>
public interface IBluetoothCommunicator
{
    /// <summary>
    /// Call this to initiate connection
    /// </summary>
    Task ConnectAsync
    (
        string mac,
        OnConnectDelegate onConnect,
        OnDataReceivedDelegate onDataReceived,
        OnDisconnectDelegate onDisconnect
    );

    /// <summary>
    /// Send data
    /// </summary>
    void Send(byte[] data);
    
    /// <summary>
    /// Disconnect
    /// </summary>
    void Disconnect();
}