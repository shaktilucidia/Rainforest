using Android.Bluetooth;
using RainforestControlTool.Independent.Abstract.Services;
using RainforestControlTool.Independent.Models;
using RainforestControlTool.Independent.Models.Bluetooth;

namespace RainforestControlTool.Implementations.Services;

public class PairedBluetoothDevicesEnumerator : IPairedBluetoothDevicesEnumerator
{
    public async Task<IReadOnlyCollection<PairedBluetoothDevice>> EnumerateAsync()
    {
        var adapter = BluetoothAdapter.DefaultAdapter;
        if (adapter == null)
        {
            throw new InvalidOperationException("No bluetooth adapters found!");
        }

        if (!adapter.IsEnabled)
        {
            throw new InvalidOperationException("Bluetooth is disabled!");
        }
        
        return adapter
            .BondedDevices
            .Select(d => new PairedBluetoothDevice(d.Name, d.Address))
            .ToList();
    }
}