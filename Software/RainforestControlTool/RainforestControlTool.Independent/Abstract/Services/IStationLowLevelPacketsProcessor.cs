namespace RainforestControlTool.Independent.Abstract.Services;

/// <summary>
/// Delegate, called when a new packet received
/// </summary>
public delegate void OnPacketReceivedDelegate(byte[] payload);

/// <summary>
/// Low-level packets processor (build a packet, add CRC and so on)
/// </summary>
public interface IStationLowLevelPacketsProcessor
{
    /// <summary>
    /// Send the packet with given payload to station
    /// </summary>
    void SendPacket(byte[] payload);

    /// <summary>
    /// Start to listen for incoming data
    /// </summary>
    void Listen(OnPacketReceivedDelegate onPacketReceived);
    
    /// <summary>
    /// Call this when new data came from source (like Bluetooth)
    /// </summary>
    void OnDataReceived(byte[] data);

    /// <summary>
    /// Stop listening (use it to stop internal timers and so on)
    /// </summary>
    void StopListening();
}