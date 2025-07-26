using RainforestControlTool.Independent.Models;

namespace RainforestControlTool.Models;

/// <summary>
/// Main model
/// </summary>
public class MainModel
{
    /// <summary>
    /// Connection state
    /// </summary>
    public ConnectionState ConnectionState { get; set; } = ConnectionState.Disconnected;

    /// <summary>
    /// Connected station
    /// !! CHECK ConnectionState BEFORE USE !!
    /// </summary>
    public PairedBluetoothDevice? ConnectedStation { get; set; } = null;
}