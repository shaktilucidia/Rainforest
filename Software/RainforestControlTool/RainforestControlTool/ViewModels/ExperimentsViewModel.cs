using System.ComponentModel;
using System.Runtime.CompilerServices;
using RainforestControlTool.Independent.Abstract.Services;
using RainforestControlTool.Independent.Models;

namespace RainforestControlTool.ViewModels;

/// <summary>
/// Experimental view model
/// </summary>
public class ExperimentsViewModel : INotifyPropertyChanged
{
    private readonly IPairedBluetoothDevicesEnumerator _devicesEnumerator;
    
    #region Bindable properties
    
    /// <summary>
    /// Paired devices
    /// </summary>
    public List<PairedBluetoothDevice> PairedDevices
    {
        get
        {
            return _devices.ToList();
        }
        set
        {
            _devices = value;
            OnPropertyChanged(nameof(PairedDevices));
        }
    }
    
    #endregion
    
    /// <summary>
    /// Paired devices
    /// </summary>
    private IReadOnlyCollection<PairedBluetoothDevice> _devices = new List<PairedBluetoothDevice>();
    
    public ExperimentsViewModel()
    {
        _devicesEnumerator = App.ServiceProvider.GetService<IPairedBluetoothDevicesEnumerator>() ?? throw new InvalidOperationException("No bluetooth devices enumerator found!");

        Task.WaitAll(RefreshDevicesListAsync());
    }
    
    public event PropertyChangedEventHandler? PropertyChanged;

    protected virtual void OnPropertyChanged([CallerMemberName] string? propertyName = null)
    {
        PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
    }

    protected bool SetField<T>(ref T field, T value, [CallerMemberName] string? propertyName = null)
    {
        if (EqualityComparer<T>.Default.Equals(field, value)) return false;
        field = value;
        OnPropertyChanged(propertyName);
        return true;
    }
    
    private async Task RefreshDevicesListAsync()
    {
        PairedDevices = (await _devicesEnumerator.EnumerateAsync()).ToList();
    }
}