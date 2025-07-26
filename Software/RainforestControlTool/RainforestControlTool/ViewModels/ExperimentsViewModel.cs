using System.ComponentModel;
using System.Runtime.CompilerServices;
using System.Text;
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
    private readonly IBluetoothCommunicator _bluetoothCommunicator;
    private readonly IStationLowLevelPacketsProcessor _stationLowLevelPacketsProcessor;

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
            OnPropertyChanged();
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
            OnPropertyChanged();
            ((Command)ConnectCommand).ChangeCanExecute();
        }
    }
    
    /// <summary>
    /// Console text
    /// </summary>
    public string ConsoleText
    {
        get
        {
            return _consoleText;
        }
        set
        {
            _consoleText = value;
            OnPropertyChanged();
        }
    }
    
    #endregion
    
    #region Commands
    
    /// <summary>
    /// Refresh paired devices list
    /// </summary>
    public ICommand RefreshDevicesListCommand { get; }
    
    public ICommand ConnectCommand { get; }
    
    public ICommand DisconnectCommand { get; }

    public ICommand DoYiffCommand { get; }

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

    /// <summary>
    /// Text on console
    /// </summary>
    private string _consoleText = String.Empty;
    
    #endregion
    
    public ExperimentsViewModel()
    {
        #region DI
        
        _devicesEnumerator = App.ServiceProvider.GetService<IPairedBluetoothDevicesEnumerator>() ?? throw new InvalidOperationException("No bluetooth devices enumerator found!");
        _bluetoothCommunicator = App.ServiceProvider.GetService<IBluetoothCommunicator>() ?? throw new InvalidOperationException("No bluetooth communicator found!");
        _stationLowLevelPacketsProcessor = App.ServiceProvider.GetService<IStationLowLevelPacketsProcessor>() ?? throw new InvalidOperationException("No station low-level packets processor found!");
        
        #endregion

        #region Binding commands
        
        RefreshDevicesListCommand = new Command(async () => await OnRefreshDevicesListCommandAsync(), CanExecuteRefreshDevicesListCommand);
        ConnectCommand = new Command(async () => await OnConnectCommandAsync(), CanExecuteConnectCommand);
        DisconnectCommand = new Command(async () => await OnDisconnectCommandAsync(), CanExecuteDisconnectCommand);
        DoYiffCommand = new Command(OnDoYiffCommand);
        
        #endregion
        
        Task.WaitAll(OnRefreshDevicesListCommandAsync());
    }
    
    private Task OnRefreshDevicesListCommandAsync()
    {
        return RefreshDevicesListAsync();
    }
    
    private bool CanExecuteRefreshDevicesListCommand()
    {
        return _mainModel.ConnectionState == ConnectionState.Disconnected;
    }

    private async Task OnConnectCommandAsync()
    {
        _mainModel.ConnectionState = ConnectionState.Connecting;
        _mainModel.ConnectedStation = PairedDevices[SelectedStationIndex];

        await _bluetoothCommunicator.ConnectAsync
        (
            PairedDevices[SelectedStationIndex].MACAddress,
            OnConnected,
            OnDataReceived,
            OnDisconnected
        );
        
        ((Command)RefreshDevicesListCommand).ChangeCanExecute();
        ((Command)ConnectCommand).ChangeCanExecute();
        ((Command)DisconnectCommand).ChangeCanExecute();
    }

    private bool CanExecuteConnectCommand()
    {
        return _mainModel.ConnectionState == ConnectionState.Disconnected && SelectedStationIndex >= 0;
    }

    private async Task OnDisconnectCommandAsync()
    {
        _bluetoothCommunicator.Disconnect();
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

    private void OnConnected()
    {
        _mainModel.ConnectionState = ConnectionState.Connected;
    }

    private void OnDataReceived(byte[] data)
    {
        ConsoleText += Encoding.Default.GetString(data);
    }

    private void OnDisconnected()
    {
        _mainModel.ConnectionState = ConnectionState.Disconnected;
        _mainModel.ConnectedStation = null;
        
        ((Command)RefreshDevicesListCommand).ChangeCanExecute();
        ((Command)ConnectCommand).ChangeCanExecute();
        ((Command)DisconnectCommand).ChangeCanExecute();
    }

    private void OnDoYiffCommand()
    {
        _stationLowLevelPacketsProcessor.SendPacket(Encoding.Default.GetBytes("Yiff! Yiff! Yiff!"));
    }
}