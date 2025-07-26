namespace RainforestControlTool.Independent.Abstract.Services;

/// <summary>
/// Low-level packets processor (build a packet, add CRC and so on)
/// </summary>
public interface IStationLowLevelPacketsProcessor
{
    /// <summary>
    /// Send the packet with given payload to station
    /// </summary>
    void SendPacket(byte[] payload);
}