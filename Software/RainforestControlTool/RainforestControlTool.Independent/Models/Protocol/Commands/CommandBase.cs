namespace RainforestControlTool.Independent.Models.Protocol.Commands;

/// <summary>
/// Command, sent to station
/// </summary>
public class CommandBase
{
    /// <summary>
    /// Payload header, we use it to check sequence ID
    /// </summary>
    public PayloadHeader Header { get; set; }

    /// <summary>
    /// Called by packets processor to generate command payload
    /// </summary>
    public virtual byte[] GeneratePayload()
    {
        throw new NotImplementedException("Override me!");
    }

    /// <summary>
    /// Called, when response received
    /// </summary>
    public virtual Task<Object> OnResponseReceivedAsync()
    {
        throw new NotImplementedException("Override me!");
    }
}