using RainforestControlTool.Independent.Abstract.Services;
using RainforestControlTool.Independent.Models.Protocol;
using RainforestControlTool.Independent.Models.Protocol.Commands;

namespace RainforestControlTool.Independent.Implementations.Services;

public class ProtocolProcessor : IProtocolProcessor
{
    private readonly IStationLowLevelPacketsProcessor _stationLowLevelPacketsProcessor;

    public ProtocolProcessor
    (
        IStationLowLevelPacketsProcessor stationLowLevelPacketsProcessor
    )
    {
        _stationLowLevelPacketsProcessor = stationLowLevelPacketsProcessor;
    }
    
    /// <summary>
    /// Commands, not processed yet
    /// </summary>
    private List<CommandBase> _notProcessedCommands = new();

    private void EnqueueCommand(CommandBase command)
    {
        _notProcessedCommands.Add(command);

        while (_notProcessedCommands.Any())
        {
            var commandToProcess = _notProcessedCommands.First();
            _notProcessedCommands.RemoveAt(0);
            
            ProcessCommands(commandToProcess);
        }
    }

    private void ProcessCommands(CommandBase commandToProcess)
    {
        var commandHeader = commandToProcess.Header.ToBytes();
        var commandPayload = commandToProcess.GeneratePayload();
        
        var payload = new byte[commandHeader.Length + commandPayload.Length];
        Array.Copy(commandHeader, 0, payload, 0, commandHeader.Length);
        Array.Copy(commandPayload, 0, payload, commandHeader.Length, commandPayload.Length);
        
        _stationLowLevelPacketsProcessor.SendPacket(payload);
    }
    
    public void SetDateTime(DateTime dateTime, Task<bool> onCompletedAsync)
    {
        EnqueueCommand(new SetDateTimeCommand(dateTime));
    }
}