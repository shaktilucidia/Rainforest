namespace RainforestControlTool.Independent.Abstract.Services;

/// <summary>
/// Host to station protocol processor
/// </summary>
public interface IProtocolProcessor
{
    /// <summary>
    /// Set station date and time
    /// </summary>
    void SetDateTime(DateTime dateTime, Task<bool> onCompletedAsync); 
}