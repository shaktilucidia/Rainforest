namespace RainforestControlTool.Independent.Enums.Protocol;

/// <summary>
/// Payload type
/// </summary>
public enum PayloadType
{
    /// <summary>
    /// Command to station
    /// </summary>
    Command = 0x00,
    
    /// <summary>
    /// Response from station
    /// </summary>
    Response = 0x01
}