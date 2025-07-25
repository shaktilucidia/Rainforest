using Android.Bluetooth;
using Java.Util;
using RainforestControlTool.Independent.Abstract.Services;

namespace RainforestControlTool.Implementations.Services;

public class BluetoothCommunicator : IBluetoothCommunicator
{
    /// <summary>
    /// Read no more than this amount at once
    /// </summary>
    private const int ReadBufferSize = 1024;
    
    private OnConnectDelegate? _onConnect;
    private OnDataReceivedDelegate? _onDataReceived;
    private OnDisconnectDelegate? _onDisconnect;
    
    private BluetoothSocket? _socket;
    private Thread? _readerThread;
    
    public async Task ConnectAsync
    (
        string mac,
        OnConnectDelegate onConnect,
        OnDataReceivedDelegate onDataReceived,
        OnDisconnectDelegate onDisconnect
    )
    {
        _onConnect = onConnect ?? throw new ArgumentNullException(nameof(onConnect));
        _onDataReceived = onDataReceived ?? throw new ArgumentNullException(nameof(onDataReceived));
        _onDisconnect = onDisconnect ?? throw new ArgumentNullException(nameof(onDisconnect));
        
        if (_socket != null && _socket.IsConnected)
        {
            throw new InvalidOperationException("Already connected!");
        }

        var device = BluetoothAdapter
            .DefaultAdapter
            .BondedDevices
            .First(d => d.Address.Equals(mac));
        
        try
        {
            // What does this magic UUID mean?
            _socket = device.CreateInsecureRfcommSocketToServiceRecord(UUID.FromString("00001101-0000-1000-8000-00805f9b34fb"))
                ?? throw new InvalidOperationException("Unable to create bluetooth socket!");

            await _socket.ConnectAsync();
            if (!_socket.IsConnected)
            {
                throw new InvalidOperationException("Unable to connect!");
            }

            // Starting a reader thread
            _readerThread = new Thread(ReaderThreadRun);
            _readerThread.Start();

            _onConnect();
        }
        catch
        {
            if (_socket != null && _socket.IsConnected)
            {
                _socket.Close();
            }

            throw;
        }
    }

    public void Send(byte[] data)
    {
        if (!_socket.IsConnected)
        {
            throw new InvalidOperationException("Not connected!");
        }

        _socket.OutputStream.Write(data, 0, data.Length);
    }

    public void Disconnect()
    {
        if (_socket != null && _socket.IsConnected)
        {
            _socket.Close();
            _onDisconnect();
        }
    }
    
    /// <summary>
    /// Entry point for reader thread
    /// </summary>
    private void ReaderThreadRun()
    {
        var buffer = new byte[ReadBufferSize];
        
        while (true)
        {
            try
            {
                var readSize = _socket.InputStream.Read(buffer, 0, ReadBufferSize);
                
                MainThread.BeginInvokeOnMainThread(() =>
                {
                    _onDataReceived(buffer.Take(readSize).ToArray());
                });
            }
            catch
            {
                // Exceptions occur when socket is getting closed on disconnect
                Disconnect();
                return;
            }
        }
    }
}