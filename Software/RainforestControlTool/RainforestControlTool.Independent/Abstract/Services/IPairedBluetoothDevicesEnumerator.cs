using RainforestControlTool.Independent.Models;
using RainforestControlTool.Independent.Models.Bluetooth;

namespace RainforestControlTool.Independent.Abstract.Services;

/// <summary>
/// Paired bluetooth devices enumerator
/// </summary>
public interface IPairedBluetoothDevicesEnumerator
{
    /// <summary>
    /// Enumerate paired devices
    /// </summary>
    Task<IReadOnlyCollection<PairedBluetoothDevice>> EnumerateAsync();
}