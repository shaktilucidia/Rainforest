using Microsoft.Extensions.Logging;
using RainforestControlTool.Implementations.Services;
using RainforestControlTool.Independent.Abstract.Services;
using RainforestControlTool.Independent.Implementations.Services;

namespace RainforestControlTool;

public static class MauiProgram
{
    public static MauiApp CreateMauiApp()
    {
        var builder = MauiApp.CreateBuilder();
        builder
            .UseMauiApp<App>()
            .ConfigureFonts(fonts =>
            {
                fonts.AddFont("OpenSans-Regular.ttf", "OpenSansRegular");
                fonts.AddFont("OpenSans-Semibold.ttf", "OpenSansSemibold");
            });
        
        #region DI

        #region Singletons
        
        builder.Services.AddSingleton<IPairedBluetoothDevicesEnumerator, PairedBluetoothDevicesEnumerator>();
        builder.Services.AddSingleton<IBluetoothCommunicator, BluetoothCommunicator>();
        builder.Services.AddSingleton<IStationLowLevelPacketsProcessor, StationLowLevelPacketsProcessor>();
        builder.Services.AddSingleton<IProtocolProcessor, ProtocolProcessor>();
        
        #endregion

        #endregion

#if DEBUG
        builder.Logging.AddDebug();
#endif

        return builder.Build();
    }
}