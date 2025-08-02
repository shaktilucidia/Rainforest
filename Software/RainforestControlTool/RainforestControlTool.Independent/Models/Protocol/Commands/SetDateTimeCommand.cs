using RainforestControlTool.Independent.Enums.Protocol;
using RainforestControlTool.Independent.Helpers;

namespace RainforestControlTool.Independent.Models.Protocol.Commands;

public class SetDateTimeCommand : CommandBase
{
    private readonly DateTime _dateTime;
    
    public SetDateTimeCommand(DateTime dateTime)
    {
        Header = new PayloadHeader(PayloadType.Command);
        _dateTime = dateTime;
    }

    public override byte[] GeneratePayload()
    {
        var request = new byte[11];
        
        // 5-6 (0-1): Command code
        Array.Copy(BitConverter.GetBytes((UInt16)0x01), 0, request, 0, 2);
        
        // TODO: Set me!
        // 7-8 (2-3): Timeshift
        Array.Copy(BitConverter.GetBytes((UInt16)0), 0, request, 2, 2);
        
        // 9 (4): Year
        request[4] = (byte)(_dateTime.Year - 2000);
        
        // 10 (5): Month
        request[5] = (byte)_dateTime.Month;
        
        // 11 (6): Day (of month)
        request[6] = (byte)_dateTime.Day;
        
        // 12 (7): Day of week
        request[7] = (byte)STM32DateTimeHelper.ToStm32DayOfWeek(_dateTime.DayOfWeek);
        
        // 13 (8): Hour
        request[8] = (byte)_dateTime.Hour;
        
        // 14 (9): Minute
        request[9] = (byte)_dateTime.Minute;
        
        // 15 (10): Second
        request[10] = (byte)_dateTime.Second;

        return request;
    }

    public async override Task<Object> OnResponseReceivedAsync()
    {
        throw new NotImplementedException();
    }
}