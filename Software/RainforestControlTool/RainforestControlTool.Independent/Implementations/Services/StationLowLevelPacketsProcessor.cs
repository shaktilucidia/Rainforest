using RainforestControlTool.Independent.Abstract.Services;
using RainforestControlTool.Independent.Helpers;

namespace RainforestControlTool.Independent.Implementations.Services;

public class StationLowLevelPacketsProcessor : IStationLowLevelPacketsProcessor
{
    /// <summary>
    /// Add this to payload to get full packet lenght. 1: lenght byte + 4: crc32 bytes
    /// </summary>
    private const int HeaderAndCrcLength = 5;

    private const int MinPayloadLength = 1;
    private const int MaxPayloadLength = 250;
    
    private readonly IBluetoothCommunicator _bluetoothCommunicator;

    public StationLowLevelPacketsProcessor
    (
        IBluetoothCommunicator bluetoothCommunicator
    )
    {
        _bluetoothCommunicator = bluetoothCommunicator;
    }
    
    public void SendPacket(byte[] payload)
    {
        _ = payload ?? throw new ArgumentNullException(nameof(payload));
        
        if (payload.Length < MinPayloadLength || payload.Length > MaxPayloadLength)
        {
            throw new ArgumentException("Wrong payload size", nameof(payload));
        }

        var packetLength = payload.Length + HeaderAndCrcLength;
        
        var fullPacket = new byte[packetLength];
        Array.Fill<byte>(fullPacket, 0x00);

        // Length
        fullPacket[0] = (byte)(payload.Length + HeaderAndCrcLength);

        // Payload
        Array.Copy(payload, 0, fullPacket, 1, payload.Length);
        
        // CRC32 (over length and payload)
        var dataExceptCrcLength = packetLength - sizeof(UInt32);
        var crc = STM32CrcGenerator
            .CalculateStmCrc32
            (
                fullPacket
                    .Take(dataExceptCrcLength)
                    .ToArray()
            );
        
        Array.Copy
        (
            BitConverter.GetBytes(crc),
            0,
            fullPacket,
            dataExceptCrcLength,
            sizeof(UInt32)
        );

        _bluetoothCommunicator.Send(fullPacket.ToArray());
    }
}