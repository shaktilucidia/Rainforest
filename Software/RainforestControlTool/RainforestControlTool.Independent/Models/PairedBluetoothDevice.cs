namespace RainforestControlTool.Independent.Models;

/// <summary>
/// Paired bluetooth device
/// </summary>
public class PairedBluetoothDevice
{
    /// <summary>
    /// Creature-readable name
    /// </summary>
    public string Name { get; }
    
    /// <summary>
    /// MAC address
    /// </summary>
    public string MACAddress { get; }

    public PairedBluetoothDevice
    (
        string name,
        string macAddress
    )
    {
        if (string.IsNullOrWhiteSpace(name)) { throw new ArgumentException("Name cannot be null or whitespace", nameof(name)); }
        if (string.IsNullOrWhiteSpace(macAddress)) { throw new ArgumentException("MAC address cannot be null or whitespace", nameof(macAddress)); }
        
        Name = name;
        MACAddress = macAddress;
    }
}