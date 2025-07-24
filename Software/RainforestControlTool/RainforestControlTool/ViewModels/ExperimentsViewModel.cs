using System.ComponentModel;
using System.Runtime.CompilerServices;
using System.Windows.Input;
using RainforestControlTool.Independent.Abstract.Services;
using RainforestControlTool.Independent.Models;
using RainforestControlTool.Models;

namespace RainforestControlTool.ViewModels;

/// <summary>
/// Experimental view model
/// </summary>
public class ExperimentsViewModel : INotifyPropertyChanged
{
    private readonly IPairedBluetoothDevicesEnumerator _devicesEnumerator;

    public event PropertyChangedEventHandler? PropertyChanged;
    
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
    
    /// <summary>
    /// Selected station index
    /// </summary>
    public int SelectedStationIndex
    {
        get
        {
            return _selectedStationIndex;
        }
        set
        {
            _selectedStationIndex = value;
            OnPropertyChanged(nameof(SelectedStationIndex));
            ((Command)OnConnectCommand).ChangeCanExecute();
        }
    }
    
    #endregion
    
    #region Commands
    
    /// <summary>
    /// Refresh paired devices list
    /// </summary>
    public ICommand RefreshDevicesListCommand { get; }
    
    public ICommand OnConnectCommand { get; }
    
    public ICommand OnDisconnectCommand { get; }
    
    #endregion
    
    #region Private
    
    /// <summary>
    /// Paired devices
    /// </summary>
    private IReadOnlyCollection<PairedBluetoothDevice> _devices = new List<PairedBluetoothDevice>();

    /// <summary>
    /// Selected station index
    /// </summary>
    private int _selectedStationIndex = -1;
    
    /// <summary>
    /// Main model
    /// </summary>
    private MainModel _mainModel = new MainModel();
    
    #endregion
    
    public ExperimentsViewModel()
    {
        _devicesEnumerator = App.ServiceProvider.GetService<IPairedBluetoothDevicesEnumerator>() ?? throw new InvalidOperationException("No bluetooth devices enumerator found!");

        #region Binding commands
        
        RefreshDevicesListCommand = new Command(async () => await OnRefreshDevicesListCommandAsync());
        OnConnectCommand = new Command(async () => await OnConnectCommandAsync(), CanExecuteConnectCommand);
        OnDisconnectCommand = new Command(async () => await OnDisconnectCommandAsync(), CanExecuteDisconnectCommand);
        
        #endregion
        
        Task.WaitAll(OnRefreshDevicesListCommandAsync());
    }
    
    private Task OnRefreshDevicesListCommandAsync()
    {
        return RefreshDevicesListAsync();
    }

    private async Task OnConnectCommandAsync()
    {
        _mainModel.ConnectionState = ConnectionState.Connected;
        
        ((Command)OnConnectCommand).ChangeCanExecute();
        ((Command)OnDisconnectCommand).ChangeCanExecute();
    }

    private bool CanExecuteConnectCommand()
    {
        return _mainModel.ConnectionState == ConnectionState.Disconnected && SelectedStationIndex >= 0;
    }

    private async Task OnDisconnectCommandAsync()
    {
        _mainModel.ConnectionState = ConnectionState.Disconnected;
        
        ((Command)OnConnectCommand).ChangeCanExecute();
        ((Command)OnDisconnectCommand).ChangeCanExecute();
    }

    private bool CanExecuteDisconnectCommand()
    {
        return _mainModel.ConnectionState == ConnectionState.Connected;
    }
    
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