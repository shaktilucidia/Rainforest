using System.Text;
using RainforestControlTool.Independent.Abstract.Services;
using RainforestControlTool.Independent.Helpers;

namespace RainforestControlTool.Independent.Implementations.Services;

public class StationLowLevelPacketsProcessor : IStationLowLevelPacketsProcessor
{
    /// <summary>
    /// Receiver state machine state
    /// </summary>
    private enum ReceiverState
    {
        Listen,
        InProgress
    };
    
    /// <summary>
    /// Add this to payload to get full packet lenght. 1: lenght byte + 4: crc32 bytes
    /// </summary>
    private const int HeaderAndCrcLength = 5;

    private const int MinPayloadLength = 1;
    private const int MaxPayloadLength = 250;
    
    private readonly IBluetoothCommunicator _bluetoothCommunicator;

    private OnPacketReceivedDelegate? _onPacketReceived = null;
    
    private ReceiverState _receiverState = ReceiverState.Listen;
    
    private int _receiverExpectedPacketLength = 0;
    
    private readonly List<byte> _receiverRawPacketData = new List<byte>();

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

    public void Listen(OnPacketReceivedDelegate onPacketReceived)
    {
        _onPacketReceived = onPacketReceived ?? throw new ArgumentNullException(nameof(onPacketReceived));
        
        _receiverState = ReceiverState.Listen;
        _receiverRawPacketData.Clear();
    }

    public void OnDataReceived(byte[] data)
    {
        foreach (var b in data)
        {
            OnNextByteReceived(b);
        }
    }

    private void OnNextByteReceived(byte data)
    {
        switch (_receiverState)
        {
            case ReceiverState.Listen:
                _receiverExpectedPacketLength = data;

                if
                (
                    _receiverExpectedPacketLength < MinPayloadLength + HeaderAndCrcLength
                    ||
                    _receiverExpectedPacketLength > MaxPayloadLength + HeaderAndCrcLength
                )
                {
                    // Invalid packet
                    return;
                }
                
                _receiverRawPacketData.Add(data);
                _receiverState = ReceiverState.InProgress;
                break;
            
            case ReceiverState.InProgress:
                _receiverRawPacketData.Add(data);

                if (_receiverRawPacketData.Count == _receiverExpectedPacketLength)
                {
                    // We have new packet, checking CRC
                    var dataExceptCrcLength = _receiverExpectedPacketLength - sizeof(UInt32);
                    var calculatedCrc = STM32CrcGenerator
                        .CalculateStmCrc32
                        (
                            _receiverRawPacketData
                            .Take(dataExceptCrcLength)
                            .ToArray()
                        );
                    
                    var expectedCrc = BitConverter.ToUInt32
                        (
                            _receiverRawPacketData
                                .Skip(dataExceptCrcLength)
                                .Take(sizeof(UInt32))
                                .ToArray()
                        );
                    
                    if (expectedCrc == calculatedCrc && _onPacketReceived != null)
                    {
                        _onPacketReceived
                        (
                            _receiverRawPacketData
                                .Skip(1)
                                .Take(_receiverExpectedPacketLength - HeaderAndCrcLength)
                                .ToArray()
                        );
                    }
                    
                    _receiverRawPacketData.Clear();
                    _receiverState = ReceiverState.Listen;
                    return;
                }
                
                break;
            
            default:
                throw new InvalidOperationException($"Unknown receiver state { _receiverState }");
        }
    }
}