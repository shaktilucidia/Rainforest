namespace RainforestControlTool.Independent.Helpers;

/// <summary>
/// C# date and time <-> STM32 date and time
/// </summary>
public static class STM32DateTimeHelper
{
    private static readonly Dictionary<DayOfWeek, int> _dayOfWeekToStm32 = new()
    {
        { DayOfWeek.Sunday, 7 },
        { DayOfWeek.Monday, 1 },
        { DayOfWeek.Tuesday, 2 },
        { DayOfWeek.Wednesday, 3 },
        { DayOfWeek.Thursday, 4 },
        { DayOfWeek.Friday, 5 },
        { DayOfWeek.Saturday, 6 }
    };
    
    public static int ToStm32DayOfWeek(DayOfWeek dayOfWeek)
    {
        return _dayOfWeekToStm32[dayOfWeek];
    }
}